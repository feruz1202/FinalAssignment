#include "../tcp_server.hpp"

int main () {
    Server s = Server(6379);
    return s.run();
}

