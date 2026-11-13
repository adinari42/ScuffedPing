#include "ft_ping.h"

static void print_stats(t_ping *p)
{
    double avg;
    double stddev;
    double loss;

    printf("--- %s ping statistics ---\n", p->host);
    loss = p->sent ? ((double)(p->sent - p->received) / p->sent) * 100.0 : 0.0;
    printf("%zu packets transmitted, %zu packets received, %.0f%% packet loss\n",
           p->sent, p->received, loss);
    if (p->received > 0)
    {
        double var;

        avg = p->sum / p->received;
        var = (p->sum_sq / p->received) - (avg * avg);
        stddev = (var > 0.0) ? sqrt(var) : 0.0;
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
               p->min, avg, p->max, stddev);
    }
}

static int init_target(t_ping *p)
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct timeval tv = {1, 0};
    int ttl = 64;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    if (getaddrinfo(p->host, NULL, &hints, &res) != 0)
        return (fprintf(stderr, "ft_ping: unknown host %s\n", p->host), -1);
    memcpy(&p->addr, res->ai_addr, sizeof(struct sockaddr_in));
    inet_ntop(AF_INET, &p->addr.sin_addr, p->ip, sizeof(p->ip));
    freeaddrinfo(res);
    p->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (p->sockfd < 0)
        return (perror("ft_ping: socket"), -1);
    setsockopt(p->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    setsockopt(p->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    return (0);
}

int main(int argc, char **argv)
{
    t_ping p;
    int opt;

    memset(&p, 0, sizeof(p));
    p.pid = (uint16_t)getpid();
    while ((opt = getopt(argc, argv, "v?")) != -1)
    {
        if (opt == 'v')
            p.verbose = true;
        else
        {
            printf("Usage: ft_ping [-v?] destination\n");
            return (optopt == '?' ? 0 : 64);
        }
    }
    if (optind >= argc)
        return (fprintf(stderr, "ft_ping: missing host operand\n"), 2);
    p.host = argv[optind];
    if (getuid() != 0)
        return (fprintf(stderr, "ft_ping: root privileges required\n"), 1);
    if (init_target(&p) < 0)
        return (2);
    printf("PING %s (%s): %d data bytes", p.host, p.ip, PAYLOAD_SIZE);
    if (p.verbose)
        printf(", id 0x%04x = %u", p.pid, p.pid);
    printf("\n");
    ping_loop(&p);
    print_stats(&p);
    close(p.sockfd);
    return (0);
}