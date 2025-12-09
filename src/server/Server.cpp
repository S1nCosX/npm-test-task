#include "Server.h"

#include <algorithm>
#include <atomic>
#include <arpa/inet.h>
#include <chrono>
#include <fcntl.h>
#include <future>
#include <ostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>

void Server::init(int port) {
    init("127.0.0.1", port);
}
void Server::init(std::string addr, int port) {
    tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (tcp_socket_fd < 0)
        throw std::runtime_error("Failed to open new tcp socket");
    if (udp_socket_fd < 0)
        throw std::runtime_error("Failed to open new tcp socket");

    sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    if (addr == "localhost")
        addr = "127.0.0.1";
    
    if (addr == "0.0.0.0" || addr == "*")
        sock_addr.sin_addr.s_addr = INADDR_ANY;
    else {
        if (inet_pton(AF_INET, addr.c_str(), &sock_addr.sin_addr) <= 0)
            throw std::runtime_error("Invalid IP address: " + addr);
    }

    fcntl(tcp_socket_fd, F_SETFL, fcntl(tcp_socket_fd, F_GETFL, 0) | O_NONBLOCK);
    fcntl(udp_socket_fd, F_SETFL, fcntl(udp_socket_fd, F_GETFL, 0) | O_NONBLOCK);

    if (bind(tcp_socket_fd, (sockaddr*)&sock_addr, sizeof(sock_addr)) != 0) 
        throw std::runtime_error("Failed to bind tcp socket");

    if (bind(udp_socket_fd, (sockaddr*)&sock_addr, sizeof(sock_addr)) != 0) 
        throw std::runtime_error("Failed to bind udp socket");

    if (listen(tcp_socket_fd, 2) != 0)
        throw std::runtime_error("Failed to listen tcp socket");

    epoll_fd = epoll_create1(0);
    ev.events = EPOLLIN | EPOLLET;

    ev.data.fd = tcp_socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_socket_fd, &ev);
    ev.data.fd = udp_socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_socket_fd, &ev);

    std::cout << "Initialized server on " << addr << ":" << port << std::endl;
}

void Server::run() {
    stats.running = true;
    std::cout << "Server is running" << std::endl;

    epoll_event events[16];
    std::vector<std::future<void>> tasks;

    while (stats.running) {
        int events_num = epoll_wait(epoll_fd, events, 16, -1);      
        for (int i = 0; i < events_num; i++) {
            int fd_to_process = events[i].data.fd;
            if (fd_to_process == tcp_socket_fd) {
                stats.curr_clients++;
                stats.alltime_clients++;
                std::cout << "Got new connection to tcp" << std::endl;
                tasks.emplace_back(std::async(process_conn, EServerProcessorType::TCP_REG, epoll_fd, fd_to_process, &stats));
            } else if (fd_to_process == udp_socket_fd) {
                stats.curr_clients++;
                stats.alltime_clients++;
                tasks.emplace_back(std::async(process_conn, EServerProcessorType::UDP, epoll_fd, fd_to_process, &stats));
                std::cout << "Got new connection to udp" << std::endl;
                stats.curr_clients--;
            } else {
                tasks.emplace_back(std::async(process_conn, EServerProcessorType::TCP_MESSAGE, epoll_fd, fd_to_process, &stats));
                std::cout << "Got message to tcp" << std::endl;
            }
        }
        tasks.erase(
            std::remove_if(
                tasks.begin(), 
                tasks.end(), 
                [](std::future<void>& task){
                    return (task.wait_for(std::chrono::seconds(0))) == std::future_status::ready;
                }
            ),
            tasks.end()
        );
    }
    for (auto& t : tasks) t.wait();
}

void Server::process_conn(EServerProcessorType type, int epoll_fd, int processor_fd, ServerStats* stats) {
    ServerProcessor processor(epoll_fd, processor_fd, stats);
    switch (type) {
        case EServerProcessorType::TCP_REG:
            processor.process_tcp_reg();
            break;
        case EServerProcessorType::TCP_MESSAGE:
            processor.process_tcp();
            break;
        case EServerProcessorType::UDP:
            processor.process_udp();
            break;
    }
}
