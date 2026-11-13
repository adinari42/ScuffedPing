#include "../include/ft_ping.h"

static volatile sig_atomic_t g_running = 1;
static volatile sig_atomic_t g_send_ready = 1;

static void on_sigint(int sig)
{
    (void)sig;
    g_running = 0;
}

static void on_sigalrm(int sig)
{
    (void)sig;
    g_send_ready = 1;
}

void setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = on_sigint;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = on_sigalrm;
    sigaction(SIGALRM, &sa, NULL);
}

static void wait_for_reply(t_ping *ctx)
{
    uint8_t buf[1024];
    struct sockaddr_in from;
    socklen_t from_len;
    ssize_t bytes;

    from_len = sizeof(from);
    bytes = recvfrom(ctx->sockfd, buf, sizeof(buf), 0,
                     (struct sockaddr *)&from, &from_len);
    if (bytes > 0)
        printf("Received %zd bytes (parser coming in Segment 5)\n", bytes);
}

void ping_loop(t_ping *ctx)
{
    setup_signals();
    while (g_running)
    {
        if (g_send_ready)
        {
            g_send_ready = 0;
            send_ping(ctx);
            alarm(1);
        }
        if (g_running)
            wait_for_reply(ctx);
    }
}