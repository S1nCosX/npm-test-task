#include "ServerProcessor.h"

#include <cerrno>
#include <cstddef>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

ServerProcessor::ServerProcessor( int _epoll_fd, int _processor_fd, ServerStats* _stats) {
    processor_fd = _processor_fd;
    epoll_fd = _epoll_fd;
    command_processor = CommandProcessor(_stats);
    owner_stats = _stats;
}


void ServerProcessor::process_tcp_reg() {
    while (1) {            
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(processor_fd, (sockaddr*)&client_addr, &len);
        if (client_fd == -1)
        return;
        
        fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);
        
        epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
            close(client_fd);
    }
}

void ServerProcessor::process_tcp() {
    char buff[1024];
    std::string readed;
    while (1) {
        size_t size = recv(processor_fd, buff, sizeof(buff), 0);

        if (size > 0) {
            readed.append((char*) buff, size);

            int message_end;
            while ((message_end = readed.find('\n')) != std::string::npos){
                std::string message = readed.substr(0, message_end);

                std::string response = command_processor.processRequest(message);
                
                send(processor_fd, response.c_str(), response.length(), 0);
                readed.erase(0, message_end + 1);
            }
        }
        else if (size <= 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                if (size == 0)
                    perror("recv");
                close(processor_fd);
                owner_stats->curr_clients--;
            }
            break;
        }
    }
}

void ServerProcessor::process_udp() {
    char buff[4096];
    sockaddr_in client_addr;

    while (1) {
        socklen_t len = sizeof(client_addr);
        ssize_t size = recvfrom(processor_fd, buff, sizeof(buff), 0, (sockaddr*) &client_addr, &len);
    
        if (size > 0) {
            std::string message(buff, size);

            std::string response = command_processor.processRequest(message);
            
            sendto(processor_fd, response.c_str(), response.length(), 0, (sockaddr*) &client_addr, len);
        }
        else {
            if (size < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                perror("recv");
            break;
        }
    }
}
