#ifndef SERVER_HPP
#define SERVER_HPP
#include <netinet/in.h>

class Server {
    public:
        Server(int port);
        int run();
    private:
        void handleConnection(int client_fd);
        int port;
};

#endif
