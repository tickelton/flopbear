#ifndef __FLOPBEAR_H__
#define __FLOPBEAR_H__
#include <stdint.h>

enum _verbosity {
	V_DEFAULT = 0,
	V_INFO = 1,
	V_DEBUG = 2,
	V_TRACE = 3,
};

struct arguments {
	char   *ifname;
	enum _verbosity verbosity;
};

//flopbear internals
typedef struct _flopbear_config_s {
	uint8_t incremental_leases;
	char   *if_name;
	struct sockaddr *if_addr;
	struct sockaddr *start_addr;
} 	fb_config_s;

#endif
