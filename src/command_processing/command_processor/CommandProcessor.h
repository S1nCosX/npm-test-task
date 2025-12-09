#pragma once

#include "server/server_stats/ServerStats.h"
#include <string>

class CommandProcessor {
private:
    ServerStats *stats;
public:
    std::string processRequest(std::string request);
    CommandProcessor();
    CommandProcessor(ServerStats *stats);
private:
    std::string processTime();
    std::string processStats();
    std::string processShutdown();
};