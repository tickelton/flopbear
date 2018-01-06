#include <stdio.h>
#include "flopbear.h"

error_t
parse_opt (int key, char *arg, struct argp_state *state)
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
            argp_usage (state);
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
    struct arguments arguments;

    if(argp_parse(&argp, argc, argv, 0, 0, &arguments)) {
        // TODO: this should never happen. argp_parse exits
        //       on error. Could be handled better.
        fprintf(stderr, "Error parsing arguments\n");
        return 1;
    }

    printf("ifname=%s\n", arguments.ifname);

    return 0;
}
