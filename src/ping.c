#include "ft_ping.h"

static volatile sig_atomic_t g_running = 1;
static volatile sig_atomic_t g_send = 1;

static void sig_handler(int sig)
{
    if (sig == SIGINT)
        g_running = 0;
    else if (sig == SIGALRM)
        g_send = 1;
}

uint16_t checksum(void *buf, size_t len)
{
    uint16_t *w = buf;
    uint32_t sum = 0;

    while (len > 1)
    {
        sum += *w++;
        len -= 2;
    }
    if (len == 1)
        sum += *(uint8_t *)w;
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return ((uint16_t)~sum);
}

static void send_packet(t_ping *p)
{
    uint8_t pkt[PKT_SIZE];
    struct icmphdr *icmp = (struct icmphdr *)pkt;
    struct timeval now;

    memset(pkt, 0, sizeof(pkt));
    icmp->type = ICMP_ECHO;
    icmp->un.echo.id = htons(p->pid);
    icmp->un.echo.sequence = htons(p->seq);
    gettimeofday(&now, NULL);
    memcpy(pkt + sizeof(*icmp), &now, sizeof(now));
    icmp->checksum = checksum(pkt, sizeof(pkt));
    if (sendto(p->sockfd, pkt, sizeof(pkt), 0,
               (struct sockaddr *)&p->addr, sizeof(p->addr)) > 0)
    {
        p->seq++;
        p->sent++;
    }
}

static void recv_packet(t_ping *p)
{
    uint8_t buf[512];
    struct sockaddr_in from;
    socklen_t flen = sizeof(from);
    ssize_t n;

    n = recvfrom(p->sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &flen);
    if (n <= 0)
        return;
    struct iphdr *ip = (struct iphdr *)buf;
    size_t ip_len = ip->ihl * 4;
    if (n < (ssize_t)(ip_len + sizeof(struct icmphdr)))
        return;
    struct icmphdr *icmp = (struct icmphdr *)(buf + ip_len);
    char src[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from.sin_addr, src, sizeof(src));
    if (icmp->type == ICMP_ECHOREPLY && ntohs(icmp->un.echo.id) == p->pid)
    {
        struct timeval tv, now;
        memcpy(&tv, buf + ip_len + sizeof(*icmp), sizeof(tv));
        gettimeofday(&now, NULL);
        double rtt = (now.tv_sec - tv.tv_sec) * 1000.0 + (now.tv_usec - tv.tv_usec) / 1000.0;
        if (p->received == 0 || rtt < p->min)
            p->min = rtt;
        if (rtt > p->max)
            p->max = rtt;
        p->sum += rtt;
        p->sum_sq += rtt * rtt;
        p->received++;
        printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n",
               n - ip_len, src, ntohs(icmp->un.echo.sequence), ip->ttl, rtt);
    }
    else if (p->verbose && icmp->type != ICMP_ECHO && icmp->type != ICMP_ECHOREPLY)
    {
        const char *msg = (icmp->type == ICMP_TIME_EXCEEDED)
                              ? "Time to live exceeded"
                              : "Destination Unreachable";
        printf("%zd bytes from %s: %s\n", n - ip_len, src, msg);
    }
}

void ping_loop(t_ping *p)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);
    while (g_running)
    {
        if (g_send)
        {
            g_send = 0;
            send_packet(p);
            alarm(1);
        }
        if (g_running)
            recv_packet(p);
    }
}