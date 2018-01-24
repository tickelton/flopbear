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

#define SERVER_PORT	67
#define MESSAGE_TYPE	0x35
#define BOOTREPLY	2
#define DHCP_MAGIC	0x63825363
#define DHCP_END	0xff
#define DHCP_MESSAGE_TYPE	0x35
#define DHCP_SERVER_ID		0x36

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
