#ifndef FT_PING_H
#define FT_PING_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#define PING_PKT_SIZE 64
#define ICMP_PAYLOAD 56

typedef struct s_stats
{
    size_t sent;
    size_t received;
    double min_rtt;
    double max_rtt;
    double sum_rtt;
    double sum_sq_rtt;

} t_stats;

typedef struct s_ping
{
    int sockfd;
    uint16_t pid;
    uint16_t seq;
    bool verbose;
    char *target;
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in dest_addr;
    t_stats stats;

} t_ping;

int parse_args(int argc, char **argv, t_ping *ctx);
void print_usage(void);
int resolve_target(t_ping *ctx);
int setup_socket(t_ping *ctx);
uint16_t	calc_checksum(const void *data, size_t len);
int			send_ping(t_ping *ctx);

#endif
