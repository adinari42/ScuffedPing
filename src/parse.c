#include "../include/ft_ping.h"

void print_usage(void)
{
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    printf("  -v                 verbose output\n");
    printf("  -?                 give this help list\n");
}

static int parse_flag(const char *arg, t_ping *ctx)
{
    size_t i;

    i = 1;
    while (arg[i])
    {
        if (arg[i] == 'v')
            ctx->verbose = true;
        else if (arg[i] == '?')
        {
            print_usage();
            exit(0);
        }
        else
        {
            fprintf(stderr, "ft_ping: invalid option -- '%c'\n", arg[i]);
            fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
            return (-1);
        }
        i++;
    }
    return (0);
}

int parse_args(int argc, char **argv, t_ping *ctx)
{
    int i;

    i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            if (parse_flag(argv[i], ctx) < 0)
                return (-1);
        }
        else if (!ctx->target)
            ctx->target = argv[i];
        else
        {
            fprintf(stderr, "ft_ping: unexpected extra argument '%s'\n", argv[i]);
            return (-1);
        }
        i++;
    }
    if (!ctx->target)
    {
        fprintf(stderr, "ft_ping: missing host operand\n");
        if (!ctx->target)
        {
            fprintf(stderr, "ft_ping: missing host operand\n");
            fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
            return (-1);
        }
        return (-1);
    }
    return (0);
}