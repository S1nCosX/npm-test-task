#include "server/Server.h"

int main (int argc, char *argv[]) {
    Server server;
    switch (argc) {
        case 1: 
            server.init("localhost", 8080);
            break;
        case 2:
            server.init(std::atoi(argv[1]));
            break;
        case 3:
            server.init(std::string(argv[1]), std::atoi(argv[2]));
            break;
    }
    server.run();
}
