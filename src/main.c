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

static struct arguments arguments;

#define INFO(...) if (arguments.verbosity >= V_INFO) \
	fprintf(stderr, __VA_ARGS__)
#define DEBUG(...) if (arguments.verbosity >= V_DEBUG) \
	fprintf(stderr, __VA_ARGS__)
#define TRACE(...) if (arguments.verbosity >= V_TRACE) \
	fprintf(stderr, __VA_ARGS__)

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
	struct ifaddrs *ifaddr;

	parse_opts(&argc, argv);
	INFO("Using interface %s\n", arguments.ifname);

	if (getifaddrs(&ifaddr) == -1) {
		err(1, "getifaddrs");
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
				errx(1, "getnameinfo() failed: %s\n",
				     gai_strerror(s));
			}
			INFO("Got address %s\n", ipaddr);
		}
	}

	freeifaddrs(ifaddr);

	return 0;
}
