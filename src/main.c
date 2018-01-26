/*
 * Copyright (c) 2018 tick <tickelton@gmail.com>
 * 
 * SPDX-License-Identifier:	ISC
 */

#define _BSD_SOURCE
#include "flopbear.h"

void dhcp_msg_dump(FILE *stream, struct dhcp_msg *msg)
{
	fprintf(stream,
		"DHCP message:\n"
		"\tOP %hhu [%s]\n"
		"\tHTYPE %hhu HLEN %hhu\n"
		"\tHOPS %hhu\n"
		"\tXID %8X\n"
		"\tSECS %hu FLAGS %hu\n"
		"\tCIADDR %s YIADDR %s SIADDR %s GIADDR %s\n"
		"\tCHADDR %s\n"
		"\tMAGIC %8X\n"
		"\tMSG TYPE %s\n",
		*DHCP_MSG_F_OP(msg->data),
		(*DHCP_MSG_F_OP(msg->data) == 1 ? "REQUEST" : "REPLY"),
		*DHCP_MSG_F_HTYPE(msg->data),
		*DHCP_MSG_F_HLEN(msg->data),
		*DHCP_MSG_F_HOPS(msg->data),
		ntohl(*DHCP_MSG_F_XID(msg->data)),
		ntohs(*DHCP_MSG_F_SECS(msg->data)),
		ntohs(*DHCP_MSG_F_FLAGS(msg->data)),
		msg->ciaddr, msg->yiaddr, msg->siaddr, msg->giaddr,
		msg->chaddr,
		*(uint32_t*)DHCP_MSG_F_MAGIC(msg->data),
		(msg->type == DHCPDISCOVER ? "DHCPDISCOVER" :
		 msg->type == DHCPOFFER ? "DHCPOFFER" :
		 msg->type == DHCPREQUEST ? "DHCPREQUEST" :
		 msg->type == DHCPDECLINE ? "DHCPDECLINE" :
		 msg->type == DHCPACK ? "DHCPACK" :
		 msg->type == DHCPNAK ? "DHCPNAK" :
		 msg->type == DHCPRELEASE ? "DHCPRELEASE" :
		 msg->type == DHCPINFORM ? "DHCPINFORM" : "unknown"));

	struct dhcp_opt cur_opt;
	uint8_t *options = DHCP_MSG_F_OPTIONS(msg->data);

	while (dhcp_opt_next(&options, &cur_opt, msg->end))
	{
		switch (cur_opt.code)
		{
			case DHCP_OPT_STUB:
				fprintf(stream, "\tOPTION STUB\n");
				break;
			case DHCP_OPT_NETMASK:
				fprintf(stream, "\tOPTION NETMASK %s\n",
					inet_ntop(AF_INET, cur_opt.data,
						(char[]){[INET_ADDRSTRLEN] = 0}, INET_ADDRSTRLEN));
				break;
			case DHCP_OPT_ROUTER:
				fprintf(stream, "\tOPTION ROUTERS %u\n", cur_opt.len / 4);
				for (off_t o = 0; o < cur_opt.len / 4; ++o)
					fprintf(stream, "\t\t%s\n", inet_ntop(AF_INET, cur_opt.data+(o*4),
							(char[]){[INET_ADDRSTRLEN] = 0}, INET_ADDRSTRLEN));
				break;
			case DHCP_OPT_DNS:
				fprintf(stream, "\tOPTION DNS %u\n", cur_opt.len / 4);
				for (off_t o = 0; o < cur_opt.len / 4; ++o)
					fprintf(stream, "\t\t%s\n", inet_ntop(AF_INET, cur_opt.data+(o*4),
							(char[]){[INET_ADDRSTRLEN] = 0}, INET_ADDRSTRLEN));
				break;
			case DHCP_OPT_LEASETIME:
				fprintf(stream, "\tOPTION LEASETIME %u\n", *(uint32_t*)cur_opt.data);
				break;
			case DHCP_OPT_SERVERID:
				fprintf(stream, "\tOPTION SERVERID %s\n",
					inet_ntop(AF_INET, cur_opt.data,
						(char[]){[INET_ADDRSTRLEN] = 0}, INET_ADDRSTRLEN));
				break;
			case DHCP_OPT_REQIPADDR:
				fprintf(stream, "\tOPTION REQIPADDR %s\n",
					inet_ntop(AF_INET, cur_opt.data,
						(char[]){[INET_ADDRSTRLEN] = 0}, INET_ADDRSTRLEN));
				break;
			default:
				fprintf(stream, "\tOPTION %02hhX(%hhu)\n", cur_opt.code, cur_opt.len);
				break;
		}
	}
}

static void msg_debug(struct dhcp_msg *msg, int dir)
{
	if (dir == 0)
		fprintf(stderr, "--- INCOMING ---\n");
	else if (dir == 1)
		fprintf(stderr, "--- OUTGOING ---\n");

	dhcp_msg_dump(stderr, msg);
}

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

void get_config(struct fb_config *config, const struct arguments const *args)
{
	struct ifaddrs		*ifaddr;

	memset(&config->if_addr, 0, sizeof(struct sockaddr_in));
	memset(&config->clt_addr, 0, sizeof(struct sockaddr_in));

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
			union fb_in_addr tmp_addr;

			memcpy(&config->if_addr,
				((struct sockaddr_in *)ifa->ifa_addr),
				sizeof(struct sockaddr_in));

			tmp_addr.addr = config->if_addr.sin_addr.s_addr;
			tmp_addr.s_addr.b4++;
			config->clt_addr.sin_addr.s_addr = tmp_addr.addr;
			break;
		}
	}


	config->if_name = strdup(arguments.ifname);

	freeifaddrs(ifaddr);
}

int do_listen(struct fb_config *config)
{
	int fd;
	const int one = 1;
	struct sockaddr_in bind_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(67),
		.sin_addr = {INADDR_ANY}
	};

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		perror("socket");
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int))) {
		perror("setsockopt SO_REUSEADDR");
		return -1;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof(int))) {
		perror("setsockopt SO_BROADCAST");
		return -1;
	}
	setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, arguments.ifname, strlen(arguments.ifname));

	config->if_addr.sin_family = AF_INET;
	config->if_addr.sin_port = htons(SERVER_PORT);
	bind(fd, (struct sockaddr *)&bind_addr, sizeof(struct sockaddr));

	return fd;
}

static uint32_t netmask_from_prefixlen(uint8_t prefixlen)
{
	return htonl(0xFFFFFFFFU - (1 << (32 - prefixlen)) + 1);
}

uint8_t *dhcp_opt_add_lease(uint8_t *options, size_t *_send_len, struct dhcp_lease *lease)
{
	size_t send_len = 0;

	if (lease->prefixlen > 0)
	{
		options[0] = DHCP_OPT_NETMASK;
		options[1] = 4;
		ARRAY_COPY((options + 2), (uint8_t*)((uint32_t[]){netmask_from_prefixlen(lease->prefixlen)}), 4);
		DHCP_OPT_CONT(options, send_len);
	}

	if (lease->routers_cnt > 0)
	{
		options[0] = DHCP_OPT_ROUTER;
		options[1] = lease->routers_cnt * 4;
		for (size_t i = 0; i < lease->routers_cnt; ++i)
			*(struct in_addr *)(options + 2 + (i * 4)) = lease->routers[i];
		DHCP_OPT_CONT(options, send_len);
	}

	if (lease->leasetime > 0)
	{
		options[0] = DHCP_OPT_LEASETIME;
		options[1] = 4;
		*(uint32_t*)(options + 2) = htonl(lease->leasetime);
		DHCP_OPT_CONT(options, send_len);
	}

	if (lease->nameservers_cnt > 0)
	{
		options[0] = DHCP_OPT_DNS;
		options[1] = lease->nameservers_cnt * 4;
		for (size_t i = 0; i < lease->nameservers_cnt; ++i)
			*(struct in_addr *)(options + 2 + (i * 4)) = lease->nameservers[i];
		DHCP_OPT_CONT(options, send_len);
	}

	if (_send_len != NULL)
		*_send_len += send_len;

	return options;
}

static void
discover_cb(int sock, struct dhcp_msg *msg)
{

	struct dhcp_lease lease = DHCP_LEASE_EMPTY;


#if 0
	lease = (struct dhcp_lease){
		.routers = cfg.routers,
		.routers_cnt = cfg.routers_cnt,
		.nameservers = cfg.nameservers,
		.nameservers_cnt = cfg.nameservers_cnt,
		.leasetime = cfg.leasetime,
		.prefixlen = cfg.prefixlen
	};
#endif

	//lease.address.s_addr = htonl(config.clt_addr.sin_addr.s_addr);
	lease.address.s_addr = config.clt_addr.sin_addr.s_addr;
	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &lease.address, ip_str, INET_ADDRSTRLEN);


	size_t send_len;
	uint8_t *options;
	dhcp_msg_reply(send_buffer, &options, &send_len, msg, DHCPOFFER);

	ARRAY_COPY(DHCP_MSG_F_YIADDR(send_buffer), &lease.address, 4);

	options[0] = DHCP_OPT_SERVERID;
	options[1] = 4;
	ARRAY_COPY((options + 2), &msg->sid->sin_addr, 4);
	DHCP_OPT_CONT(options, send_len);

	options = dhcp_opt_add_lease(options, &send_len, &lease);

	*options = DHCP_OPT_END;
	DHCP_OPT_CONT(options, send_len);

	if (debug)
		msg_debug(&((struct dhcp_msg){.data = send_buffer, .length = send_len }), 1);
	int err = sendto(sock,
		send_buffer, send_len,
		MSG_DONTWAIT, (struct sockaddr *)&broadcast, sizeof broadcast);

	if (err < 0)
		dhcpd_error(errno, 1, "Could not send DHCPOFFER");

}

static void
request_cb(int sock, struct dhcp_msg *msg)
{
	struct in_addr *requested_server;
	uint8_t *options;
	struct dhcp_opt current_opt;
	struct dhcp_lease lease = DHCP_LEASE_EMPTY;

	requested_server = (struct in_addr *)DHCP_MSG_F_SIADDR(msg->data);
	options = DHCP_MSG_F_OPTIONS(msg->data);

	while (dhcp_opt_next(&options, &current_opt, msg->end))
		switch (current_opt.code)
		{
			case DHCP_OPT_REQIPADDR:
				lease.address = *(struct in_addr *)current_opt.data;
				break;
			case DHCP_OPT_SERVERID:
				requested_server = (struct in_addr *)current_opt.data;
				break;
		}

	if (requested_server->s_addr != msg->sid->sin_addr.s_addr)
		return;

	int err;


#if 0
	lease = (struct dhcp_lease){
		.address = *requested_addr,
		.routers = cfg.routers,
		.routers_cnt = cfg.routers_cnt,
		.nameservers = cfg.nameservers,
		.nameservers_cnt = cfg.nameservers_cnt,
		.leasetime = cfg.leasetime,
		.prefixlen = cfg.prefixlen
	};
#endif


	size_t send_len;
	dhcp_msg_reply(send_buffer, &options, &send_len, msg, DHCPACK);

	ARRAY_COPY(DHCP_MSG_F_YIADDR(send_buffer), &lease.address, 4);

	options[0] = DHCP_OPT_SERVERID;
	options[1] = 4;
	ARRAY_COPY((options + 2), &msg->sid->sin_addr, 4);
	DHCP_OPT_CONT(options, send_len);

	options = dhcp_opt_add_lease(options, &send_len, &lease);

	*options = DHCP_OPT_END;
	DHCP_OPT_CONT(options, send_len);

	if (debug)
		msg_debug(&((struct dhcp_msg){.data = send_buffer, .length = send_len }), 1);
	err = sendto(sock,
		send_buffer, send_len,
		MSG_DONTWAIT, (struct sockaddr *)&broadcast, sizeof broadcast);

	if (err < 0)
		dhcpd_error(0, 1, "Could not send DHCPACK");


	return;
}

static void
req_cb(int sock)
{
	/* Initialize address struct passed to recvfrom */
	struct sockaddr_in src_addr = {
		.sin_addr = {INADDR_ANY}
	};
	socklen_t src_addrlen = AF_INET;

	/* Receive data from socket */
	ssize_t recvd = recvfrom(
		sock,
		recv_buffer,
		RECV_BUF_LEN,
		MSG_DONTWAIT,
		(struct sockaddr * restrict)&src_addr, &src_addrlen);

	/* Detect errors */
	if (recvd < 0)
		return;
	/* Detect too small messages */
	if (recvd < DHCP_MSG_HDRLEN)
		return;
	/* Check magic value */
	uint8_t *magic = DHCP_MSG_F_MAGIC(recv_buffer);
	if (!DHCP_MSG_MAGIC_CHECK(magic))
		return;

	/* Convert addresses to strings */
	char ciaddr[INET_ADDRSTRLEN],
			 yiaddr[INET_ADDRSTRLEN],
			 siaddr[INET_ADDRSTRLEN],
			 giaddr[INET_ADDRSTRLEN],
			 chaddr[MAC_ADDRSTRLEN],
			 srcaddr[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, DHCP_MSG_F_CIADDR(recv_buffer), ciaddr, sizeof ciaddr);
	inet_ntop(AF_INET, DHCP_MSG_F_YIADDR(recv_buffer), yiaddr, sizeof yiaddr);
	inet_ntop(AF_INET, DHCP_MSG_F_SIADDR(recv_buffer), siaddr, sizeof siaddr);
	inet_ntop(AF_INET, DHCP_MSG_F_GIADDR(recv_buffer), giaddr, sizeof giaddr);
	inet_ntop(AF_INET, &src_addr.sin_addr, srcaddr, sizeof srcaddr);
	mac_ntop(DHCP_MSG_F_CHADDR(recv_buffer), chaddr, sizeof chaddr);

	/* Extract message type from options */
	uint8_t *options = DHCP_MSG_F_OPTIONS(recv_buffer);
	struct dhcp_opt current_option;

	enum dhcp_msg_type msg_type = 0;

	while (dhcp_opt_next(&options, &current_option, (uint8_t*)(recv_buffer + recvd)))
		if (current_option.code == 53)
			msg_type = (enum dhcp_msg_type)current_option.data[0];

	struct dhcp_msg msg = {
		.data = recv_buffer,
		.end = recv_buffer + recvd,
		.length = recvd,
		.type = msg_type,
		.ciaddr = ciaddr,
		.yiaddr = yiaddr,
		.siaddr = siaddr,
		.giaddr = giaddr,
		.chaddr = chaddr,
		.srcaddr = srcaddr,
		.source = (struct sockaddr *)&src_addr,
		.sid = (struct sockaddr_in *)&server_id
	};

	if (debug)
		msg_debug(&msg, 0);

	switch (msg_type)
	{
		case DHCPDISCOVER:
			TRACE("DHCPDISCOVER\n");
			discover_cb(sock, &msg);
			break;

		case DHCPREQUEST:
			TRACE("DHCPREQUEST\n");
			request_cb(sock, &msg);
			break;

		case DHCPRELEASE:
			TRACE("DHCPRELEASE\n");
			//release_cb(EV_A_ w, &msg);
			break;

		case DHCPDECLINE:
			TRACE("DHCPDECLINE\n");
			//decline_cb(EV_A_ w, &msg);
			break;

		case DHCPINFORM:
			TRACE("DHCPINFORM\n");
			//inform_cb(EV_A_ w, &msg);
			break;

		default:
			fprintf(stderr, "%s", BROKEN_SOFTWARE_NOTIFICATION);
			msg_debug(&msg, 0);
			break;
	}
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
