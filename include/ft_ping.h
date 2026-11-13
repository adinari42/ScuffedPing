#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PKT_SIZE 64
#define PAYLOAD_SIZE 56

typedef struct s_ping
{
    int sockfd;
    uint16_t pid;
    uint16_t seq;
    bool verbose;
    char *host;
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in addr;
    size_t sent;
    size_t received;
    double min;
    double max;
    double sum;
    double sum_sq;
} t_ping;

void ping_loop(t_ping *p);
uint16_t checksum(void *buf, size_t len);

#endif