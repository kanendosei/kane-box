#include "kb_utils.h"

/* -------------------------------------------------------------------------- */
void form_sys_result(int res, char *data) {
	printf("<result>\n");
	printf("\t<success>%s</success>\n", res?"true":"false");
	if (!res) { /* error was orrured */
		if (data) { /* which error */
			printf("\t\t<error name=\"System Error\">\n");
			printf("\t\t\t<description>%s</description>\n", data);
			printf("\t\t</error>\n");
		}
	}
	printf("<result>\n");
}

/* -------------------------------------------------------------------------- */
void form_error(char *name, char *field, char *desc) {
	printf("<result>\n");
	printf("\t<success>false</success>\n");
	if (NULL != name) {
		printf("\t\t<error name=\"%s\">\n", name);
			if (NULL != field) {
				printf("\t\t\t<field>%s</field>\n", field);
			}
			if (NULL != desc) {
				printf("\t\t\t<description>%s</description>\n", desc);
			}
		printf("\t\t</error>\n");
	}
	printf("<result>\n");
}

static int schema_error_flag = 0;

void schema_error_start(void) {
	if (0 == schema_error_flag) {
		schema_error_flag = 1;
		printf("<result>\n");
		printf("\t<success>false</success>\n");
	}
}

void schema_error_end(void) {
	printf("<result>\n");
	schema_error_flag = 0;
}

void schema_error_body(char *name, char *field, int field_len, char *desc, int desc_len) {
	if ((NULL != name) && (NULL != field) && (NULL != desc)) {
		printf("\t\t<error name=\"%s\">\n", name);
		printf("\t\t\t<field>%.*s</field>\n", field_len, field);
		printf("\t\t\t<description>%.*s</description>\n", desc_len, desc);
		printf("\t\t</error>\n");
	}
}

