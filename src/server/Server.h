#pragma once

#include <sys/epoll.h>
#include "server/processor/ServerProcessor.h"
#include "server/processor/enum/EServerProcessorType.h"
#include "server/server_stats/ServerStats.h"

class Server {
    ServerStats stats;
private:
    int tcp_socket_fd;
    int udp_socket_fd;
    int epoll_fd;
    epoll_event ev;
public:
    void init(int port);
    void init(std::string addr, int port);

    void run();
private:
    static void process_conn(EServerProcessorType type, int epoll_fd, int processor_fd, ServerStats* stats);
};
