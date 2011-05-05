#ifndef KB_UTILS_H
#define KB_UTILS_H

#include <stdio.h>

/* -----------------------------------------------------------------------------
 DEFINES
----------------------------------------------------------------------------- */
#define KB_OK 1
#define KB_ERR 0

/* error types */
#define KB_ERR_INT "Internal error"
#define KB_ERR_PARSE "Parsing error"

/* error messages */
#define KB_ERR_INT_BAD_PARM "Bad parameter passed"
#define KB_ERR_BAD_VAL "Bad value"
#define KB_ERR_NO_VAL "Can't obtain value"
#define KB_ERR_BAD_ITEM "Unknown element received"
#define KB_ERR_NO_MEM "Can't allocate memory"
#define KB_ERR_NO_NAME "Can't obtain item name"

/* -----------------------------------------------------------------------------
form xml with result
	res = 1 - success
	res = 0 - error was occured
	data - error description, or NULL
----------------------------------------------------------------------------- */
void form_sys_result(int res, char *data);

/* -----------------------------------------------------------------------------
form error with complete description
	name - error name
	field - plase of the error
	desc - error description
----------------------------------------------------------------------------- */
void form_error(char *name, char *field, char *desc);

/* -----------------------------------------------------------------------------
form error list during schema validity checking
----------------------------------------------------------------------------- */
void schema_error_start(void);
void schema_error_end(void);
void schema_error_body(char *name, char *field, int field_len, char *desc, int desc_len);

#endif
