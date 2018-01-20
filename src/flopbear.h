/*
 * Copyright (c) 2018 tick <tickelton@gmail.com>
 * 
 * SPDX-License-Identifier:	ISC
 */

#ifndef __FLOPBEAR_H__
#define __FLOPBEAR_H__
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

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
struct fb_config {
	char   *if_name;
	struct sockaddr_in if_addr;
	struct sockaddr_in clt_addr;
};

struct fb_in_addr_s {
	uint8_t b1, b2, b3, b4;
};

union fb_in_addr {
	struct fb_in_addr_s s_addr;
	uint32_t addr;
};

#endif
