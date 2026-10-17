#ifndef SERVER_HPP
#define SERVER_HPP
#include "db.hpp"
#include <netinet/in.h>

class Server {
    public:
        Server(int port);
        int run();
    private:
        Database db;
        void handleConnection(int client_fd);
        int port;
};

#endif
