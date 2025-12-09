#pragma once

#include <atomic>

struct ServerStats {
    std::atomic<bool> running = false;
    std::atomic<int> curr_clients = 0;
    std::atomic<int> alltime_clients = 0;
};
