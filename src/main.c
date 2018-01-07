#define _BSD_SOURCE
#include <err.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "flopbear.h"

/* getifaddrs + getnameinfo */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

extern char *optarg;
extern int optind;

static const char const *version = "0.1.0";
static const char const *progname = "flopbear";
static const char const *progdesc =
"A configuration-less DHCP server.";

static void
usage(const int ret)
{
	fprintf(stderr, "Usage: %s [-hV] IF\n%s\n\n"
		"-h\tGive this help message\n"
		"-V\tPrint program version\n\n"
		"IF is the name of the interface to listen on.\n",
		progname, progdesc);
	exit(ret);
}

static void
parse_opts(int *argc, char **argv, struct arguments * arguments)
{
	int 	ch;

	while ((ch = getopt(*argc, argv, "hV")) != -1) {
		switch (ch) {
		case 'h':
			usage(0);
			break;
		case 'V':
			printf("%s %s\n", progname, version);
			exit(0);
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
	if ((arguments->ifname = strdup(argv[0])) == NULL)
		err(1, NULL);
}

int
main(int argc, char **argv)
{
	struct ifaddrs *ifaddr;
	struct arguments arguments;

	parse_opts(&argc, argv, &arguments);
	printf("ifname=%s\n", arguments.ifname);

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return 1;
	}
	for (struct ifaddrs * ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		int 	family;

		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET &&
		    !strcmp(ifa->ifa_name, arguments.ifname)) {
			char 	ipaddr[NI_MAXHOST];
			int 	s;

			s = getnameinfo(
					ifa->ifa_addr,
					sizeof(struct sockaddr_in),
					ipaddr, sizeof(ipaddr),
					NULL, 0, NI_NUMERICHOST
				);
			if (s != 0) {
				printf("getnameinfo() failed: %s\n",
				       gai_strerror(s));
				return 1;
			}
			printf("paddr='%s'\n", ipaddr);

		}
	}

	freeifaddrs(ifaddr);

	return 0;
}
