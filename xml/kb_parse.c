#include <string.h>
#include "kb_parse.h"
#include "kb_utils.h"
#include "kb_process.h"

/* -----------------------------------------------------------------------------
DEFINES
----------------------------------------------------------------------------- */
#define MAX_IFACE_NUMBER 10

/* Nodes*/
#define KB_XML_IFACE "interface"
#define KB_XML_NAME "name"
#define KB_XML_IFACE_TYPE "type"
#define KB_XML_IFACE_TYPE_INT "Internal"
#define KB_XML_IFACE_TYPE_EXT "External"
#define KB_XML_IFACE_TYPE_WL "Wireless"
#define KB_XML_IFACE_IP "ip"
#define KB_XML_IFACE_DEV "device"
#define KB_XML_IFACE_HOSTS "hosts"
#define KB_XML_IFACE_GW "gateway"
#define KB_XML_IFACE_DHCP "dhcp"
#define KB_XML_IFACE_WL_OPT "wireless_options"
#define KB_XML_IFACE_SECURITY "security"
#define KB_XML_IFACE_SERVICE "service"
#define KB_XML_IFACE_DHCP_SERV "dhcp_server"
#define KB_XML_IFACE_PRINTER "printer"
#define KB_XML_IFACE_STORAGE "storage"

#define KB_XML_WLOPT_TYPE "type"
#define KB_XML_WLOPT_TYPE_WPA "WPA"
#define KB_XML_WLOPT_TYPE_WPA2 "WPA2"
#define KB_XML_WLOPT_TYPE_WEP "WEP"
#define KB_XML_WLOPT_TYPE_OPEN "OPEN"
#define KB_XML_WLOPT_SSID "ssid"
#define KB_XML_WLOPT_PWD "password"

#define KB_XML_SEC_NAT "nat"
#define KB_XML_SEC_SCRUB "scrub"
#define KB_XML_SEC_EXPOSURE "exposure"
#define KB_XML_SEC_VIRUS "virus"

#define KB_XML_SERVICE_IP "ip"
#define KB_XML_SERVICE_PORT "port"
#define KB_XML_SERVICE_PROTO "proto"
#define KB_XML_SERVICE_PROTO_UDP "UDP"
#define KB_XML_SERVICE_PROTO_TCP "TCP"
#define KB_XML_SERVICE_PROTO_ALL "ALL"

#define KB_XML_DHCP_SRV_ENABLE "enable"
#define KB_XML_DHCP_SRV_START_IP "start_ip"
#define KB_XML_DHCP_SRV_END_IP "end_ip"

#define KB_XML_PRINTER_SHARED "shared"

#define KB_XML_STORAGE_SHARED "shared"
#define KB_XML_STORAGE_WIN "windows"
#define KB_XML_STORAGE_MAC "mac"
#define KB_XML_STORAGE_LINX "linux"
#define KB_XML_STORAGE_LOGIN "login"
#define KB_XML_STORAGE_PWD "password"

#define KB_XML_TRUE "true"
#define KB_XML_FALSE "false"

/* -------------------------------------------------------------------------- */
void kb_process_xml(xmlDocPtr doc, char *iface, int dry_run) {

	kb_interface interfaces[MAX_IFACE_NUMBER];
	int curr_interface = 0;

	if (NULL == doc) {
		form_sys_result(KB_ERR, "Bad document passed");	
	}
	xmlNodePtr elem = xmlDocGetRootElement(doc);
	if ((NULL != elem) && (XML_ELEMENT_NODE == elem->type)) {
		/* process childs */
		xmlNodePtr child = elem->children;
		int res = 0;
		while ((NULL != child) && (0 == res)) {
			if (XML_ELEMENT_NODE == child->type) { /* check for "interface" node */
				if (0 == strcmp((char *)child->name, KB_XML_IFACE)) {
					/* obtain interface name */
					xmlChar *val = xmlGetProp(child, (xmlChar *)KB_XML_NAME);
					if (NULL != val) { /* obtain name */
						if ((0 == strlen(iface)) || (0 == strcmp((char *)val, iface)) || dry_run) {	
							/* process for all interfaces or for one interface only, or testing  */
							if (0 != kb_parse_interface(child, &interfaces[curr_interface])) {
								res = -1;
							}
							curr_interface++;
						}
						xmlFree(val);
					} 
				}
			}
			child = child->next;
		}
		if (0 == res) {
			if (!dry_run) {
				int i, res2 = 0;
				/* process interface/stuff applying */
				for (i = 0; i < curr_interface; i++) {
					if (0 != kb_apply_interface(&interfaces[i])) {
						res2 = -1; break;
					}
				}
				if (0 == res2) {
					form_sys_result(KB_OK, "All is ok!");
				}
			} else {
				form_sys_result(KB_OK, "All is ok!");
			}
		}
		/* Free resources */
		int i;
		for (i = 0; i < curr_interface; i++) {
			kb_free_interface(&interfaces[i]);
		}
	}
}

/* -------------------------------------------------------------------------- */
int kb_parse_interface(xmlNodePtr node, kb_interface* iface){
	int res = 0;
	if (NULL == iface) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	kb_clean_interface(iface);
	xmlChar* content;
	if ((NULL != node) && (0 == res)) {
		xmlNodePtr child = node->children;
		while (NULL != child) { /* go through all children */
			if (XML_ELEMENT_NODE == child->type) {
				if (0 == strcmp((char *)child->name, KB_XML_IFACE_TYPE)) {
					/* obtain type value */
					content = xmlNodeGetContent(child);
					if (NULL != content) {
						if (0 == strcmp((char *)content, KB_XML_IFACE_TYPE_EXT)) {
							iface->type = 0;
						} else if (0 == strcmp((char *)content, KB_XML_IFACE_TYPE_INT)) {
							iface->type = 1;
						} else if (0 == strcmp((char *)content, KB_XML_IFACE_TYPE_WL)) {
							iface->type = 2;
						} else {
							form_error(KB_ERR_PARSE, KB_XML_IFACE_TYPE, KB_ERR_BAD_VAL); res = - 1;
						}
						xmlFree(content);
					} else {
						form_error(KB_ERR_PARSE, KB_XML_IFACE_TYPE, KB_ERR_NO_VAL); res = - 1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_IP)) {
					content = xmlNodeGetContent(child);
					iface->ip = (char *)content;
					if (NULL == content) {
						form_error(KB_ERR_PARSE, KB_XML_IFACE_IP, KB_ERR_NO_VAL); res = - 1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_DEV)) {
					content = xmlNodeGetContent(child);
					iface->device = (char *)content;
					if (NULL == content) {
						form_error(KB_ERR_PARSE, KB_XML_IFACE_DEV, KB_ERR_NO_VAL); res = - 1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_HOSTS)) {
					content = xmlNodeGetContent(child);
					if (NULL != content) {
						iface->hosts = atoi((char *)content);
						xmlFree(content);
						if (0 == iface->hosts) {
							form_error(KB_ERR_PARSE, KB_XML_IFACE_HOSTS, KB_ERR_BAD_VAL); res = - 1;
						}
					} else {
						form_error(KB_ERR_PARSE, KB_XML_IFACE_HOSTS, KB_ERR_NO_VAL); res = - 1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_GW)) {
					content = xmlNodeGetContent(child);
					iface->gateway = (char *)content;
					if (NULL != content) {
						form_error(KB_ERR_PARSE, KB_XML_IFACE_GW, KB_ERR_NO_VAL); res = - 1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_DHCP)) {
					content = xmlNodeGetContent(child);
					if (NULL != content) {
						if (0 == strcmp((char *)content, KB_XML_TRUE)) {
							iface->dhcp = 1;
						} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
							iface->dhcp = 0;
						} else {
							form_error(KB_ERR_PARSE, KB_XML_IFACE_DHCP, KB_ERR_BAD_VAL); res = - 1;
						}
						xmlFree(content);
					} else {
						form_error(KB_ERR_PARSE, KB_XML_IFACE_DHCP, KB_ERR_NO_VAL); res = - 1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_WL_OPT)) {
                                	if (0 != kb_parse_wireless_opt(child, &iface->wireless_opt)) {
						res = -1;
					}
				} else if (0 == strcmp((char *)child->name, KB_XML_IFACE_SECURITY)) {
					if (0 != kb_parse_security(child, &iface->security)) {
						res = -1;
					}
                                } else if (0 == strcmp((char *)child->name, KB_XML_IFACE_SERVICE)) {
					kb_service *data = (kb_service *)malloc(sizeof(kb_service));
					if (NULL != data) {
						if (0 != kb_parse_service(child, data)) {
							res = -1;
						}
						data->next = iface->service;
						iface->service = data;
					} else {
						form_error(KB_ERR_INT, KB_XML_IFACE_SERVICE, KB_ERR_NO_MEM);
					}
                                } else if (0 == strcmp((char *)child->name, KB_XML_IFACE_DHCP_SERV)) {
					if (0 != kb_parse_dhcp_server(child, &iface->dhcp_server)) {
						res = -1;
					}
                                } else if (0 == strcmp((char *)child->name, KB_XML_IFACE_PRINTER)) {
					kb_printer *data = (kb_printer *) malloc(sizeof(kb_printer));
					if (NULL != data) {
						if (0 != kb_parse_printer(child, data)) {
							res = -1;
						}
						data->next = iface->printer;
						iface->printer = data;
					} else {
						form_error(KB_ERR_INT, KB_XML_IFACE_PRINTER, KB_ERR_NO_MEM);
					}	
                                } else if (0 == strcmp((char *)child->name, KB_XML_IFACE_STORAGE)){
					kb_storage *data = (kb_storage *) malloc(sizeof(kb_storage));
					if (NULL != data) {
						if (0 != kb_parse_storage(child, data)) {
							res = -1;
						}
						data->next = iface->storage;
						iface->storage = data;
					} else {
						form_error(KB_ERR_INT, KB_XML_IFACE_STORAGE, KB_ERR_NO_MEM);
					}
				} else { /* unknown node */
					form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
				}
			}
			child = child->next;
		}
	}
	return res;
}

/* -------------------------------------------------------------------------- */
int kb_parse_wireless_opt(xmlNodePtr node, kb_wireless_opt *parm) {
	int res = 0;
	xmlChar* content;
	if ((NULL == parm) || (NULL == node)) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	parm->type = -1; parm->ssid = NULL; parm->password = NULL;
	xmlNodePtr child = node->children;
	while ((NULL != child) && (0 == res)) { /* go through all children */
		if (XML_ELEMENT_NODE == child->type) {
			content = xmlNodeGetContent(child);
			if (0 == strcmp((char *)child->name, KB_XML_WLOPT_TYPE)) {
				/* obtain type value */
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_WLOPT_TYPE_WPA)) {
						parm->type = 0;
					} else if (0 == strcmp((char *)content, KB_XML_WLOPT_TYPE_WPA2)) {
						parm->type = 1;
					} else if (0 == strcmp((char *)content, KB_XML_WLOPT_TYPE_WEP)) {
						parm->type = 2;
					} else if (0 == strcmp((char *)content, KB_XML_WLOPT_TYPE_OPEN)) {
						parm->type = 3;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_WLOPT_TYPE, KB_ERR_BAD_VAL); res = -1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_WLOPT_TYPE, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_WLOPT_SSID)) {
				parm->ssid = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_WLOPT_SSID, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_WLOPT_PWD)) {
				parm->password = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_WLOPT_PWD, KB_ERR_NO_VAL); res = - 1;
				}
			} else {
				xmlFree(content);
				form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
			}
		}
		child = child->next;
	}
	return res;
}

/* -------------------------------------------------------------------------- */
int kb_parse_security(xmlNodePtr node, kb_security *parm) {
	int res = 0;
	xmlChar* content;
	if ((NULL == parm) || (NULL == node)) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	parm->nat = -1; parm->scrub = -1; parm->exposure = -1; parm->virus = -1;
	xmlNodePtr child = node->children;
	while ((NULL != child) && (0 == res)) { /* go through all children */
		if (XML_ELEMENT_NODE == child->type) {
			content = xmlNodeGetContent(child);
			if (0 == strcmp((char *)child->name, KB_XML_SEC_NAT)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->nat = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->nat = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_SEC_NAT, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_SEC_NAT, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_SEC_SCRUB)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->scrub = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->scrub = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_SEC_SCRUB, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_SEC_SCRUB, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_SEC_EXPOSURE)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->exposure = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->exposure = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_SEC_EXPOSURE, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_SEC_EXPOSURE, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_SEC_VIRUS)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->virus = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->virus = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_SEC_VIRUS, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_SEC_VIRUS, KB_ERR_BAD_VAL); res = - 1;
				}
			} else {
				form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
			}
		}
		child = child->next;
	}
	return res;
}

/* -------------------------------------------------------------------------- */
int kb_parse_service(xmlNodePtr node, kb_service *parm) {
	int res = 0;
	xmlChar* content;
	if ((NULL == parm) || (NULL == node)) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	parm->name = NULL; parm->ip = NULL; parm->port = 0; parm->proto = -1; parm->next = NULL;
	/* obtain service name */
	xmlChar *name = xmlGetProp(node, (xmlChar *)KB_XML_NAME);
	if ((NULL == name) || (0 == strlen((char*)name))) {
		form_error(KB_ERR_PARSE, KB_XML_IFACE_SERVICE, KB_ERR_NO_NAME);
		if (NULL != name) {
			xmlFree(name);
		}
		return -1;
	} else {
		parm->name = (char *)name;
	}
	xmlNodePtr child = node->children;
	while ((NULL != child) && (0 == res)) { /* go through all children */
		if (XML_ELEMENT_NODE == child->type) {
			content = xmlNodeGetContent(child);
			if (0 == strcmp((char *)child->name, KB_XML_SERVICE_IP)) {
				parm->ip = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_SERVICE_IP, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_SERVICE_PORT)) {
				if (NULL != content) {
					parm->port = atoi((char *)content);
					xmlFree(content);
					if (0 == parm->port) {
						form_error(KB_ERR_PARSE, KB_XML_SERVICE_PORT, KB_ERR_BAD_VAL); res = - 1;
					}
				} else {
					form_error(KB_ERR_PARSE, KB_XML_SERVICE_PORT, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_SERVICE_PROTO)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_SERVICE_PROTO_UDP)) {
						parm->proto = 0;
					} else if (0 == strcmp((char *)content, KB_XML_SERVICE_PROTO_TCP)) {
						parm->proto = 1;
					} else if (0 == strcmp((char *)content, KB_XML_SERVICE_PROTO_ALL)) {
						parm->proto = 2;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_SERVICE_PROTO, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_SERVICE_PROTO, KB_ERR_NO_VAL); res = - 1;
				}
			} else {
				form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
			}
		}
		child = child->next;
	}
	return res;
}

/* -------------------------------------------------------------------------- */
int kb_parse_dhcp_server(xmlNodePtr node, kb_dhcp_server *parm) {
	int res = 0;
	xmlChar* content;
	if ((NULL == parm) || (NULL == node)) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	parm->enable = -1; parm->start_ip = NULL; parm->end_ip = NULL;
	xmlNodePtr child = node->children;
	while ((NULL != child) && (0 == res)) { /* go through all children */
		if (XML_ELEMENT_NODE == child->type) {
			content = xmlNodeGetContent(child);
			if (0 == strcmp((char *)child->name, KB_XML_DHCP_SRV_ENABLE)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->enable = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->enable = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_DHCP_SRV_ENABLE, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_DHCP_SRV_ENABLE, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_DHCP_SRV_START_IP)) {
				parm->start_ip = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_DHCP_SRV_START_IP, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_DHCP_SRV_END_IP)) {
				parm->end_ip = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_DHCP_SRV_END_IP, KB_ERR_NO_VAL); res = - 1;
				}
			} else {
				form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
			}
		}
		child = child->next;
	}
	return res;
}

/* -------------------------------------------------------------------------- */
int kb_parse_printer(xmlNodePtr node, kb_printer *parm) {
	int res = 0;
	xmlChar* content;
	if ((NULL == parm) || (NULL == node)) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	parm->name = NULL; parm->shared=-1; parm->next = NULL;
	/* obtain printer name */
	xmlChar *name = xmlGetProp(node, (xmlChar *)KB_XML_NAME);
	if ((NULL == name) || (0 == strlen((char*)name))) {
		form_error(KB_ERR_PARSE, KB_XML_IFACE_PRINTER, KB_ERR_NO_NAME);
		if (NULL != name) {
			xmlFree(name);
		}
		return -1;
	} else {
		parm->name = (char *)name;
	}
	xmlNodePtr child = node->children;
	while ((NULL != child) && (0 == res)) { /* go through all children */
		if (XML_ELEMENT_NODE == child->type) {
			content = xmlNodeGetContent(child);
			if (0 == strcmp((char *)child->name, KB_XML_PRINTER_SHARED)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->shared = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->shared = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_PRINTER_SHARED, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_PRINTER_SHARED, KB_ERR_NO_VAL); res = - 1;
				}
			} else {
				form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
			}
		}
		child = child->next;
	}
	return res;
}

/* -------------------------------------------------------------------------- */
int kb_parse_storage(xmlNodePtr node, kb_storage *parm) {
	int res = 0;
	xmlChar* content;
	if ((NULL == parm) || (NULL == node)) {
		form_error(KB_ERR_INT, "", KB_ERR_INT_BAD_PARM);
		return -1;
	}
	parm->name = NULL; parm->shared = -1; parm->windows = -1; parm->mac = -1; parm->linux = -1; parm->login = NULL; parm->password = NULL;
	/* obtain service name */
	xmlChar *name = xmlGetProp(node, (xmlChar *)KB_XML_NAME);
	if ((NULL == name) || (0 == strlen((char*)name))) {
		form_error(KB_ERR_PARSE, KB_XML_IFACE_STORAGE, KB_ERR_NO_NAME);
		if (NULL != name) {
			xmlFree(name);
		}
		return -1;
	} else {
		parm->name = (char *)name;
	}
	xmlNodePtr child = node->children;
	while ((NULL != child) && (0 == res)) { /* go through all children */
		if (XML_ELEMENT_NODE == child->type) {
			content = xmlNodeGetContent(child);
			if (0 == strcmp((char *)child->name, KB_XML_STORAGE_SHARED)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->shared = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->shared = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_STORAGE_SHARED, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_STORAGE_SHARED, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_STORAGE_WIN)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->windows = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->windows = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_STORAGE_WIN, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_STORAGE_WIN, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_STORAGE_MAC)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->mac = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->mac = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_STORAGE_MAC, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_STORAGE_MAC, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_STORAGE_LINX)) {
				if (NULL != content) {
					if (0 == strcmp((char *)content, KB_XML_TRUE)) {
						parm->linux = 1;
					} else if (0 == strcmp((char *)content, KB_XML_FALSE)) {
						parm->linux = 0;
					} else {
						form_error(KB_ERR_PARSE, KB_XML_STORAGE_LINX, KB_ERR_BAD_VAL); res = - 1;
					}
					xmlFree(content);
				} else {
					form_error(KB_ERR_PARSE, KB_XML_STORAGE_LINX, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_STORAGE_LOGIN)) {
				parm->login = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_STORAGE_LOGIN, KB_ERR_NO_VAL); res = - 1;
				}
			} else if (0 == strcmp((char *)child->name, KB_XML_STORAGE_PWD)) {
				parm->password = (char *)content;
				if (NULL == content) {
					form_error(KB_ERR_PARSE, KB_XML_STORAGE_PWD, KB_ERR_NO_VAL); res = - 1;
				}
			} else {
				form_error(KB_ERR_PARSE, (char *)child->name, KB_ERR_BAD_ITEM); res = - 1;
			}
		}
		child = child->next;
	}
	return res;
}

/* -------------------------------------------------------------------------- */
void kb_clean_interface(kb_interface* iface) {
	if (NULL != iface) {
		iface->type = -1;	
		iface->ip = NULL;
		iface->device = NULL;
		iface->hosts = 0;
		iface->gateway = NULL;
		iface->dhcp = -1;
		iface->service = NULL;
		iface->printer = NULL;
		iface->storage = NULL;
		iface->wireless_opt.ssid = NULL;
		iface->wireless_opt.password = NULL;
		iface->dhcp_server.start_ip = NULL;
		iface->dhcp_server.end_ip = NULL;	
	}
}

/* -------------------------------------------------------------------------- */
void kb_free_interface(kb_interface* iface) {
	void *tmp;
	if (NULL == iface) {
		return;
	}
	if (NULL != iface->ip) {
		xmlFree((xmlChar *)iface->ip);
	}
	if (NULL != iface->device) {
		xmlFree((xmlChar *)iface->device);
	}
	if (NULL != iface->gateway) {
		xmlFree((xmlChar *)iface->gateway);
	}
	if (NULL != iface->wireless_opt.ssid) {
		xmlFree((xmlChar *)iface->wireless_opt.ssid);
	}
	if (NULL != iface->wireless_opt.password) {
		xmlFree((xmlChar *)iface->wireless_opt.password);
	}
	kb_service *service = iface->service;
	while (NULL != service) { /* clean every service stored */
		if (NULL != service->name) {
			xmlFree((xmlChar *)service->name);
		}
		if (NULL != service->ip) {
			xmlFree((xmlChar *)service->ip);
		}
		tmp = service->next;
		free(service);
		service = tmp;
	}
	if (NULL != iface->dhcp_server.start_ip) {
		xmlFree((xmlChar *)iface->dhcp_server.start_ip);
	}
	if (NULL != iface->dhcp_server.end_ip) {
		xmlFree((xmlChar *)iface->dhcp_server.end_ip);
	}
	kb_printer *printer = iface->printer;
	while (NULL != printer) { /* clean every printer stored */
		if (NULL != printer->name) {
			xmlFree((xmlChar *)printer->name);
		}
		tmp = printer->next;
		free(printer);
		printer = tmp; 
	}
	kb_storage *storage = iface->storage;
	while (NULL != storage) { /* clean every storage */
		if (NULL != storage->name) {
			xmlFree((xmlChar *)storage->name);
		}
		if (NULL != storage->login) {
			xmlFree((xmlChar *)storage->login);
		}
		if (NULL != storage->password) {
			xmlFree((xmlChar *)storage->password);
		}
		tmp = storage->next;
		free(storage);
		storage = tmp;
	}
}

/* -------------------------------------------------------------------------- */

