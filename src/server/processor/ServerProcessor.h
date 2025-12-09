#pragma once

#include "command_processing/command_processor/CommandProcessor.h"
#include "server/server_stats/ServerStats.h"

class ServerProcessor {
    int processor_fd;
    int epoll_fd;
    CommandProcessor command_processor;
    ServerStats* owner_stats;
public:
    ServerProcessor(int processor_fd, int epoll_fd, ServerStats* stats);

    void process_tcp_reg();

    void process_tcp();

    void process_udp();
};
