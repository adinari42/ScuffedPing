#include "../include/ft_ping.h"

static void	init_context(t_ping *ctx)
{
	memset(ctx, 0, sizeof(t_ping));
	ctx->sockfd = -1;
	ctx->pid = (uint16_t)getpid();
	ctx->stats.min_rtt = -1.0;
}

int	main(int argc, char **argv)
{
	t_ping	ctx;

	init_context(&ctx);
	if (parse_args(argc, argv, &ctx) < 0)
		return (2);
	if (getuid() != 0)
	{
		fprintf(stderr, "ft_ping: socket: Operation not permitted (run as root)\n");
		return (1);
	}
	printf("Target: %s (verbose: %d)\n", ctx.target, ctx.verbose);
	return (0);
}