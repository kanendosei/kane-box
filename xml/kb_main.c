/* 
 -----------------------------------------------------------------------------
 kane|box XML Configuration Utility
 (c) 2010, kane|box, Inc.
 
 Licensed under the Simplifed BSD License (see LICENSE for details)

 ( Read README for more information )
 
 LOCATIONs: 
	<config>
		Default is <curdir>/config
	<etc>
		Default is <curdir>/etc
		
	(edit this source to change defaults)

 Usage:
 	kane-config -create  // create an XML template from interface.xml
 	kane-config -update  // update the system with new networks & firewall info
 	kane-config -test    // test the interface.cfg file for valid XML
 	kane-config -reset   // reset interface.cfg back to defaults
 
 Workflow:
 1. Reads <config>/interface.cfg file
 2. Writes <etc>/interface.xxx files
 3. Writes <etc>/samba.conf
 4. Writes <etc>/pf.conf
 5. Writes other files, as necessary, from the interface.cfg file
 
 Requires:
 + XML template interface.xml (in <config>)
 + OpenBSD, netBSD or FreeBSD (does not work on Linux)
 -----------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <pcre.h>
//#include "include/pcre.h" 	// Future code for a PCRE include directory
#include "kb_utils.h"
#include "kb_parse.h"																			 

/* DEFINES
 *
 *	Be sure to change etc/ to /etc/ for the final version.
 *
 *	Also change /config to /opt/config or wherever you want
 * the interface.cfg (and other template files) to go.
 */

#define KB_SYS_CONFIG_DIR 	"etc"
#define KB_CONFIG_DIR 		"config"
#define KB_IFACE_TMPL 		"interface.cfg.template"
#define KB_IFACE_CFG 		"interface.cfg"
#define KB_IFACE_SCHEMA 	"interface.cfg.schema"

#define KB_MAX_BUF_SIZE 256

/* Scheme Parse Error Processing */

void schemaWarningCallback(void* dummy, const char* message, ...) {
	(void)dummy;
	(void)message;
}

void schemaErrorCallback(void* dummy, const char* message, ...) {
	va_list varArgs;
	va_start(varArgs, message);
	char schema_error[KB_MAX_BUF_SIZE];
	vsprintf(schema_error, message, varArgs);
	va_end(varArgs);
	(void)dummy;

	schema_error_start();
	/* error name : ... 'field' : message */
	const char *error;
	int erroffset;
	pcre *re = pcre_compile("^.*\'(.*)\'.*:.(.*)$", 0, &error, &erroffset, NULL);	
	if (NULL != re) {
		#define OUT_COUNT 9
		int out_vector[OUT_COUNT];
		int rc = pcre_exec(re, NULL, schema_error, strlen(schema_error), 0, 0, out_vector, OUT_COUNT);
		if (0 <= rc) {

/* Previous, potentially faulty, routines
		int i;
			for (i = 1; i < 3; i++) {
				char *substring_start = schema_error + out_vector[2*i];
				int substring_length = out_vector[2*i+1] - out_vector[2*i];
				printf("%2d: %.*s\n", i, substring_length, substring_start);
			}
*/
			char *field = schema_error + out_vector[2];
			int field_len = out_vector[3] - out_vector[2];
			char *msg = schema_error + out_vector[4];
			int msg_len = out_vector[5] - out_vector[4];
			schema_error_body("Schemas validity error", field, field_len, msg, msg_len); 
		}
		pcre_free(re);
	}
}

/* Create interface.cfg from interface.xml template */
void opt_create(void) {
	char tmpl_file_name[FILENAME_MAX], out_file_name[FILENAME_MAX];
	sprintf(tmpl_file_name, "%s/%s", KB_CONFIG_DIR, KB_IFACE_TMPL);
	sprintf(out_file_name, "%s/%s", KB_CONFIG_DIR, KB_IFACE_CFG);

	FILE *f1 = fopen(tmpl_file_name, "r");
	if (NULL != f1) {
		FILE *f2 = fopen(out_file_name, "w");
		if (NULL != f2) {
			/* read template line by line and output to interface.cfg */
			char line[KB_MAX_BUF_SIZE];
			while (NULL != fgets(line, sizeof(line), f1)) {
				fputs(line, f2);
			}
			fclose(f2);
		} else {
			form_sys_result(KB_ERR, "Can't open config file for writing");
		}
		fclose(f1);
	} else {
		/* can't open template file */
		form_sys_result(KB_ERR, "Can't open template file");
	}
	
}

/* process interface.cfg file
	iface - name of the interface to process, or "" for all interfaces
	dry_run = 1 - test xml without applying anything
	dry_run = 0 - test + apply
 */
void opt_process(char *iface, int dry_run) {
	char cfg_file_name[FILENAME_MAX];
	sprintf(cfg_file_name, "%s/%s", KB_CONFIG_DIR, KB_IFACE_CFG);
	xmlDocPtr document = xmlReadFile(cfg_file_name, NULL, 0);
	if (NULL == document) { /* can't load */
		form_sys_result(KB_ERR, "Can't parse config file");
	} else { /* process */
		/* check schema compliance */
		char schema_file_name[FILENAME_MAX];
		sprintf(schema_file_name, "%s/%s", KB_CONFIG_DIR, KB_IFACE_SCHEMA);
		xmlSchemaParserCtxtPtr schemaParser = xmlSchemaNewParserCtxt(schema_file_name);
		if (NULL != schemaParser) {
			xmlSchemaPtr schema = xmlSchemaParse(schemaParser);
			if (NULL != schema) {
				xmlSchemaValidCtxtPtr validityContext = xmlSchemaNewValidCtxt(schema);
				xmlSchemaSetValidErrors(validityContext, schemaErrorCallback, schemaWarningCallback, 0);
				if (NULL != validityContext) {
					if (0 != xmlSchemaValidateFile(validityContext, cfg_file_name, 0)) {
						/* validation error */
						schema_error_end();
					} else {
						/* additional processing */
						kb_process_xml(document, iface, dry_run);
					}
					xmlSchemaFreeValidCtxt(validityContext);
				} else {
					form_sys_result(KB_ERR, "Can't create validation context");
				}
				xmlSchemaFree(schema);
			} else {
				form_sys_result(KB_ERR, "Can't parse schema");
			}
			xmlSchemaFreeParserCtxt(schemaParser);
		} else {
			form_sys_result(KB_ERR, "Can't parse schema file");
		}
		xmlFreeDoc(document);
	}
}

/* main */
int main (int argc, char **argv) {

	int curr_index = 0, option_index = -1;
	static struct option avail_options[] = {
		{"create", no_argument, 0, 0},
		{"update", no_argument, 0, 0},
		{"test", no_argument, 0, 0},
		{"reset", no_argument, 0, 0},
		{0, 0, 0, 0}
	};

	/* Parse command line arguments */
	int tmp;
	while (-1 != (tmp = getopt_long_only (argc, argv, "", avail_options, &curr_index))) {
		switch (tmp) {
		case 0:
			if (-1 == option_index) {
				option_index = curr_index;
			} else {
				form_sys_result(KB_ERR, "Too many parameters!");
				exit(0);
			}
		break;
		default:
			form_sys_result(KB_ERR, "Unknown option passed!");
	        	exit(0);
		}
        }
	if (optind < argc) {
		form_sys_result(KB_ERR, "Bad parameter passed!");
		exit(0);
	}
	
	/* Process as per option passed */
	switch (option_index) {
		case 0: /* create */
			opt_create();
		break;
		case -1: /* no arguments were passed */
		case 1: /* update */
			opt_process("", 0);
		break;
		case 2: /* test */
			opt_process("", 1);
		break;
		case 3: /* reset */
			opt_create();
			opt_process("", 0);
		break;
		default:
			form_sys_result(KB_ERR, "Unknown option passed!");
			exit(0);
	}
	return 0;
}
