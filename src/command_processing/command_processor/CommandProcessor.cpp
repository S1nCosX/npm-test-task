#include "CommandProcessor.h"
#include "command_processing/command_descriptor/CommandDesc.h"
#include "command_processing/enums/ECommandType.h"
#include "server/server_stats/ServerStats.h"
#include <cstdio>
#include <ctime>
#include <sstream>
#include <iostream>

CommandProcessor::CommandProcessor(){}
CommandProcessor::CommandProcessor(ServerStats *_stats) {
    this->stats = _stats;
}

std::string CommandProcessor::processRequest(std::string request) {
    ECommandType reqType = CommandDesc::desc(request);

    switch (reqType) {
        case ECommandType::NONE:
            return request + '\n';
        case ECommandType::TIME:
            return processTime();
        case ECommandType::STATS:
            return processStats();
        case ECommandType::SHUTDOWN:
            return processShutdown();

    }
}

std::string CommandProcessor::processTime() {
    std::time_t timestamp = std::time(nullptr);
    std::tm data_time = *std::localtime(&timestamp);
    std::stringstream ss;
    ss << data_time.tm_year + 1900 << '-' << data_time.tm_mon + 1 << '-' << data_time.tm_mday << " "
        << data_time.tm_hour << ':' << data_time.tm_min << ':' << data_time.tm_sec << std::endl;
    std::cout << "got process time command, response: " << ss.str();
    return ss.str();
}

std::string CommandProcessor::processStats() {
    std::stringstream ss;
    ss << "All time connections count: " << stats->alltime_clients << "\nCurrent connections count: " <<  stats->curr_clients << std::endl;
    std::cout << "got stats command, response: " << ss.str();
    return ss.str();
}

std::string CommandProcessor::processShutdown() {
    std::stringstream ss;
    ss << "server shutted down" << std::endl;
    std::cout << "got shutdown command, response: " << ss.str();
    stats->running = false;
    return ss.str();
}