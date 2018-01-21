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

struct dhcp_msg {
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t ciaddr;
	uint32_t yiaddr;
	uint32_t siaddr;
	uint32_t gatewayp;
	uint8_t chaddr[16];
	uint8_t sname[64];
	uint8_t file[128];
	uint32_t cookie;
	uint8_t options[312];
} __attribute__((packed));

#endif
