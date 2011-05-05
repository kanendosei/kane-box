#ifndef KB_COMMON_H
#define KB_COMMON_H

typedef struct {
	int type; /* -1 error; 0 - WPA; 1 - WPA2; 2 - WEP; 3 - OPEN */
	char *ssid;
	char *password;
} kb_wireless_opt;

typedef struct {
	int nat; /* -1 - error; 0 - false; 1 - true */
	int scrub; /* -1 - error; 0 - false; 1 - true */
	int exposure; /* -1 - error; 0 - false; 1 - true */
	int virus; /* -1 - error; 0 - false; 1 - true */
} kb_security;

typedef struct {
	char *name;
	char *ip;
	int port;
	int proto; /* -1 - error; 0 - UDP; 1 - TCP; 2 - ALL */
	void *next;
} kb_service;

typedef struct {
	int enable; /* -1 - error; 0 - false; 1 - true */
	char *start_ip;
	char *end_ip;
} kb_dhcp_server;

typedef struct {
	char *name;
	int shared; /* -1 - error; 0 - false; 1 - true */
	void *next;
} kb_printer;

typedef struct {
	char *name;
	int shared; /* -1 - error; 0 - false; 1 - true */
	int windows; /* -1 - error; 0 - false; 1 - true */
	int mac; /* -1 - error; 0 - false; 1 - true */
	int linux; /* -1 - error; 0 - false; 1 - true */
	char *login;
	char *password;
	void *next;
} kb_storage;

/* Parsed interface */
typedef struct {
	int type; /* -1 - error; 0 - External; 1 - Internal; 2 - Wireless */
	char *ip;
	char *device;
	int hosts;
	char *gateway;
	int dhcp; /* -1 - error; 0 - false; 1 - true */
	kb_wireless_opt wireless_opt;
	kb_security security;
	kb_service *service;
	kb_dhcp_server dhcp_server;
	kb_printer *printer;
	kb_storage *storage;
} kb_interface;

#endif
