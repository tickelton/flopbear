/*
 * Copyright (c) 2018 tick <tickelton@gmail.com>
 * 
 * SPDX-License-Identifier:	ISC
 */

#define _BSD_SOURCE
#include "flopbear.h"

static void
usage(const int ret)
{
	fprintf(stderr, "Usage: %s [-hVv] IF\n%s\n\n"
		"-h\tGive this help message\n"
		"-V\tPrint program version\n"
		"-v\tIncrease output verbosity\n"
		"\tcan be given up to three times\n\n"
		"IF is the name of the interface to listen on.\n",
		progname, progdesc);
	exit(ret);
}

static void
parse_opts(int *argc, char **argv)
{
	int 	ch;

	arguments.verbosity = V_DEFAULT;
	while ((ch = getopt(*argc, argv, "hVv")) != -1) {
		switch (ch) {
		case 'h':
			usage(0);
			break;
		case 'V':
			printf("%s %s\n", progname, version);
			exit(0);
			break;
		case 'v':
			if (arguments.verbosity < 3)
				arguments.verbosity++;
			break;
		default:
			usage(1);
		}
	}
	*argc -= optind;
	argv += optind;
	if (*argc != 1) {
		fprintf(stderr, "Error: Missing required argument IF.\n\n");
		usage(1);
	}
	if ((arguments.ifname = strdup(argv[0])) == NULL)
		err(1, NULL);
}

int
main(int argc, char **argv)
{
	int sock = -1;
	int pollret;
	struct pollfd fds[1];

	parse_opts(&argc, argv);
	INFO("Using interface %s\n", arguments.ifname);

	get_config(&config, &arguments);
	DEBUG("Got config:\n"
	      " if_name=%s\n"
	      " if_addr=%X\n"
	      " clt_addr=%X\n",
	      config.if_name,
	      config.if_addr.sin_addr.s_addr,
	      config.clt_addr.sin_addr.s_addr);

	broadcast.sin_port = htons(68);

	for (;;) {
		if (sock < 0) {
			sock = do_listen(&config);
		}
		if (sock < 0)
			err(1, "do_listen");


		fcntl(sock, F_SETFD, FD_CLOEXEC);
		fds[0].fd = sock;
		fds[0].events = POLLIN;

		pollret = poll(fds, 1, -1);
		if(pollret < 1) {
			warn("poll");
			continue;
		}

		req_cb(sock);

	}

	return 0;
}
