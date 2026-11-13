#include "../include/ft_ping.h"

int resolve_target(t_ping *ctx)
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct sockaddr_in *addr;
    int err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    err = getaddrinfo(ctx->target, NULL, &hints, &res);
    if (err != 0)
    {
        fprintf(stderr, "ft_ping: %s: %s\n", ctx->target, gai_strerror(err));
        return (-1);
    }
    addr = (struct sockaddr_in *)res->ai_addr;
    memcpy(&ctx->dest_addr, addr, sizeof(struct sockaddr_in));
    if (!inet_ntop(AF_INET, &addr->sin_addr, ctx->ip_str, sizeof(ctx->ip_str)))
    {
        perror("ft_ping: inet_ntop");
        freeaddrinfo(res);
        return (-1);
    }
    freeaddrinfo(res);
    return (0);
}

int setup_socket(t_ping *ctx)
{
    struct timeval tv;
    int ttl;

    ctx->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ctx->sockfd < 0)
    {
        perror("ft_ping: socket");
        return (-1);
    }
    ttl = 64;
    if (setsockopt(ctx->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("ft_ping: setsockopt IP_TTL");
        close(ctx->sockfd);
        return (-1);
    }
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(ctx->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("ft_ping: setsockopt SO_RCVTIMEO");
        close(ctx->sockfd);
        return (-1);
    }
    return (0);
}