#include "resp.hpp"
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <vector>
#define SA struct sockaddr
#define MAX_BUFFER 4096
#include "client.hpp"



// FIXME: change char* to std::string 
// but inet_addr() takes char*
Client::Client(char* addr, int port) {
    int sockfd;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "socket creation failed...\n";
        exit(0);
    }
    else {
        std::cout << "Socket successfully created..\n";
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(port);
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "connection with the server failed...\n" ;
        exit(0);
    }
    else {
        std::cout << "connected to the server successfully...\n" ;
        this->socket_fd = sockfd;
    }
}

RespVal Client::sendCmd(std::vector<RespBulkStr> cmd) {
    RespVal resp_cmd = RespVal(cmd); 
    int n = writeResp(this->socket_fd, resp_cmd);
    if (n <= 0) {
        throw std::runtime_error(std::string("write failed: ") + std::strerror(errno));
    };
    // FIXME: make it asynchrounous
    RespVal response = readResp(this->socket_fd);
    return response;
}

Client::~Client() {
    std::cout << "bye bye...\n";
    close(this->socket_fd);
}
