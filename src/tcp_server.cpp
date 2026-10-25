#include <iostream>
#include "../include/resp.hpp"
#include "db.hpp"
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unistd.h>
#include <thread>
#include "tcp_server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#define MAX_BUFFER 4096

Server::Server(int port) {
    this->port = port;
    this->db = Database();
}

// FIXME: use the readResp from the resp.hpp, the single source of truth
// Problem is we have not implemented parsing multi bulk strings in that function
// so we are gonna use a seperate function for server side parsing
static std::vector<std::string> readResp0(int client_fd) {
    char buffer[MAX_BUFFER] = {0};
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        throw std::runtime_error(std::string("read failed: ") + std::strerror(errno));
    }
    std::string_view view(buffer, n);

    size_t crlf = view.find("\r\n");
    if (crlf == std::string_view::npos || view[0] != '*') {
        throw std::runtime_error("expected multi-bulk protocol");
    }

    size_t count = std::stoul(std::string(view.substr(1, crlf - 1)));
    size_t pos = crlf + 2;

    std::vector<std::string> tokens;
    tokens.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        if (pos >= view.size() || view[pos] != '$') {
            throw std::runtime_error("expected bulk string");
        }

        size_t end = view.find("\r\n", pos);          
        if (end == std::string_view::npos) {
            throw std::runtime_error("incomplete bulk length");
        }

        size_t len = std::stoul(std::string(view.substr(pos + 1, end - pos - 1)));
        pos = end + 2;                                

        if (pos + len + 2 > view.size()) {
            throw std::runtime_error("incomplete bulk payload");
        }
        if (view.substr(pos + len, 2) != "\r\n") {
            throw std::runtime_error("bulk string missing CRLF terminator");
        }

        tokens.emplace_back(view.substr(pos, len));
        pos += len + 2;                                    
    }

    return tokens;
}


void Server::handleConnection(int client_fd) {
    while (true){
        try {
            std::vector<std::string> tokens = readResp0(client_fd);
            auto response = this->db.execute(tokens);
            try{
                std::cout << "the fucking response type: " << response.data.index() << std::endl;
                writeResp(client_fd, response);
            } catch(const std::runtime_error& err) {
                std::cout << "Client error: " << err.what() << std::endl;
                close(client_fd);
                return;
            }
        } catch(const std::runtime_error& err) {
            std::cout << "Client error: " << err.what() << std::endl;
            close(client_fd);
            return;
        }
    };
}


int Server::run() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed\n";
        close(server_fd);
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed on port " << port << "\n";
        close(server_fd);
        return 1;
    }

    std::cout << "TCP Server is listening on port " << port << "...\n";

    if (listen(server_fd, 3) < 0) {
        close(server_fd);
        std::cerr << "Listen failed\n";
        return 1;
    }

    while(true) {
        socklen_t addrlen = sizeof(address);
        
        int client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::thread(&Server::handleConnection, this, client_socket).detach();
    }

    close(server_fd);

    return 0;
}
