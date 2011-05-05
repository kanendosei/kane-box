#ifndef KB_PARSE_H
#define KB_PARSE_H

#include "kb_common.h"
#include <libxml/parser.h>

/* -----------------------------------------------------------------------------
process xml document
	doc = xml tree for processing
	iface - name of the interface to process, or "" for all interfaces
	dry_run = 1 - test xml without applying anything
	dry_run = 0 - test + apply
----------------------------------------------------------------------------- */
void kb_process_xml(xmlDocPtr doc, char *iface, int dry_run);


/* -----------------------------------------------------------------------------
INTERNAL
----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
Parse appropriate section of the config
Return:
0 - all is OK
-1 - error occured
----------------------------------------------------------------------------- */
int kb_parse_interface(xmlNodePtr node, kb_interface* iface);
int kb_parse_wireless_opt(xmlNodePtr node, kb_wireless_opt *parm);
int kb_parse_security(xmlNodePtr node, kb_security *parm);
int kb_parse_service(xmlNodePtr node, kb_service *parm);
int kb_parse_dhcp_server(xmlNodePtr node, kb_dhcp_server *parm);
int kb_parse_printer(xmlNodePtr node, kb_printer *parm);
int kb_parse_storage(xmlNodePtr node, kb_storage *parm);

/* -----------------------------------------------------------------------------
Clean interface structure before usage
----------------------------------------------------------------------------- */
void kb_clean_interface(kb_interface* iface);

/* -----------------------------------------------------------------------------
Free resources
----------------------------------------------------------------------------- */
void kb_free_interface(kb_interface* iface);

#endif
