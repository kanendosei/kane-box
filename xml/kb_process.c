#include "kb_process.h"
#include "kb_utils.h"
#include <stdio.h>

/* -------------------------------------------------------------------------- */
int kb_apply_interface(kb_interface* iface) {
	int res = 0;
	if (NULL == iface) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	return res;
}

/* -------------------------------------------------------------------------- */

