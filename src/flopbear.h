/*
 * Copyright (c) 2018 tick <tickelton@gmail.com>
 * 
 * SPDX-License-Identifier:	ISC
 */

#ifndef __FLOPBEAR_H__
#define __FLOPBEAR_H__
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "dhcp.h"
#include "config.h"

#ifndef RECV_BUF_LEN
#define RECV_BUF_LEN 4096
#endif

#ifndef SEND_BUF_LEN
#define SEND_BUF_LEN 4096
#endif

#define MAC_ADDRSTRLEN 18

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

extern struct sockaddr_in server_id;
extern struct sockaddr_in broadcast;

extern uint8_t recv_buffer[RECV_BUF_LEN];
extern uint8_t send_buffer[SEND_BUF_LEN];

extern char *optarg;
extern int optind;

extern const char const *version;
extern const char const *progname;
extern const char const *progdesc;

extern struct arguments arguments;

extern uint8_t debug;

extern struct fb_config	config;

#define INFO(...) if (arguments.verbosity >= V_INFO) \
	fprintf(stderr, __VA_ARGS__)
#define DEBUG(...) if (arguments.verbosity >= V_DEBUG) \
	fprintf(stderr, __VA_ARGS__)
#define TRACE(...) if (arguments.verbosity >= V_TRACE) \
	fprintf(stderr, __VA_ARGS__)

extern const char *BROKEN_SOFTWARE_NOTIFICATION;

int mac_ntop(char *addr, char *dst, size_t s);
void dhcpd_error(int _exit, int _errno, const char *fmt, ...);

#endif
