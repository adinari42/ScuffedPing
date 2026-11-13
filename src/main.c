#include "../include/ft_ping.h"

static void init_context(t_ping *ctx)
{
    memset(ctx, 0, sizeof(t_ping));
    ctx->sockfd = -1;
    ctx->pid = (uint16_t)getpid();
    ctx->stats.min_rtt = -1.0;
}

int main(int argc, char **argv)
{
    t_ping ctx;

    init_context(&ctx);
    if (parse_args(argc, argv, &ctx) < 0)
        return (2);
    if (getuid() != 0)
    {
        fprintf(stderr, "ft_ping: socket: Operation not permitted (run as root)\n");
        return (1);
    }
    if (resolve_target(&ctx) < 0)
        return (2);
    if (setup_socket(&ctx) < 0)
        return (1);
    printf("PING %s (%s): %d data bytes",
           ctx.target, ctx.ip_str, ICMP_PAYLOAD);
    if (ctx.verbose)
        printf(", id 0x%04x = %u", ctx.pid, ctx.pid);
    printf("\n");
    send_ping(&ctx);
    close(ctx.sockfd);
    return (0);
}