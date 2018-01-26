/*
 * Copyright (c) 2018 tick <tickelton@gmail.com>
 * 
 * SPDX-License-Identifier:	ISC
 */

#define _BSD_SOURCE
#include "flopbear.h"

struct sockaddr_in server_id;
struct sockaddr_in broadcast = {
	.sin_family = AF_INET,
	.sin_addr = {INADDR_BROADCAST},
};

uint8_t recv_buffer[RECV_BUF_LEN];
uint8_t send_buffer[SEND_BUF_LEN];

extern char *optarg;
extern int optind;

const char const *version = "0.1.0";
const char const *progname = "flopbear";
const char const *progdesc =
"A configuration-less DHCP server.";

struct arguments arguments;

uint8_t debug = 0;

struct fb_config	config;

const char *BROKEN_SOFTWARE_NOTIFICATION = 
"#################################### ALERT ####################################\n"
"  BROKEN SOFTWARE NOTIFICATION - SOMETHING SENDS INVALID DHCP MESSAGES IN YOUR\n"
"                                    NETWORK\n";


int mac_ntop(char *addr, char *dst, size_t s)
{
	return snprintf(dst, s,
		"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX", 
		addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

inline void dhcpd_error(int _exit, int _errno, const char *fmt, ...)
{

	va_list ap;
	va_start(ap, fmt);

	if (_errno != 0)
		fprintf(stderr, "%s: ", strerror(_errno));

	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);

	if (_exit > 0)
		exit(_exit);
}
