#include <argp.h>

const char *argp_program_version =
"flopbear 0.1.0";
const char *argp_program_bug_address =
"<tickelton@gmail.com>";

struct arguments {
    char *ifname;
};
char args_doc[] = "INTERFACE";
char doc[] =
"flopbear -- A configurtion-less DHCP server";
error_t parse_opt (int key, char *arg, struct argp_state *state);
struct argp argp = {0, parse_opt, args_doc, doc};


