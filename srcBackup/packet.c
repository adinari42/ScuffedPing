#include "../include/ft_ping.h"
uint16_t calc_checksum(const void *data, size_t len)
{
    const uint16_t *words;
    uint32_t sum;

    words = (const uint16_t *)data;
    sum = 0;
    while (len > 1)
    {
        sum += *words++;
        len -= 2;
    }
    if (len == 1)
        sum += *(const uint8_t *)words;
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return ((uint16_t)(~sum));
}

int send_ping(t_ping *ctx)
{
    uint8_t packet[PING_PKT_SIZE];
    struct icmphdr *icmp;
    struct timeval now;
    ssize_t sent;
    size_t i;

    memset(packet, 0, sizeof(packet));
    icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(ctx->pid);
    icmp->un.echo.sequence = htons(ctx->seq);
    if (gettimeofday(&now, NULL) == 0)
        memcpy(packet + sizeof(struct icmphdr), &now, sizeof(now));
    i = sizeof(struct icmphdr) + sizeof(now);
    while (i < PING_PKT_SIZE)
    {
        packet[i] = (uint8_t)i;
        i++;
    }
    icmp->checksum = 0;
    icmp->checksum = calc_checksum(packet, sizeof(packet));
    sent = sendto(ctx->sockfd, packet, sizeof(packet), 0,
                  (struct sockaddr *)&ctx->dest_addr, sizeof(ctx->dest_addr));
    if (sent < 0)
    {
        perror("ft_ping: sending packet");
        return (-1);
    }
    ctx->seq++;
    ctx->stats.sent++;
    return (0);
}