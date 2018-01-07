#include <ifaddrs.h>
#include <stdio.h>
#include "flopbear.h"

/* getifaddrs + getnameinfo */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


error_t
parse_opt(int key, char *arg, struct argp_state * state)
{
	struct arguments *arguments = state->input;

	switch (key) {
	case ARGP_KEY_ARG:
		if (state->arg_num >= 1) {
			argp_usage(state);
		}
		arguments->ifname = arg;
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1) {
			argp_usage(state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	struct ifaddrs *ifaddr;
	struct arguments arguments;

	if (argp_parse(&argp, argc, argv, 0, 0, &arguments)) {
		/*
		 * TODO: this should never happen. argp_parse exits
	         *       on error. Could be handled better.
		 */
		fprintf(stderr, "Error parsing arguments\n");
		return 1;
	}
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
