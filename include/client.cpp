#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define SA struct sockaddr
#include "client.hpp"


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

Client::~Client() {
    std::cout << "bye bye...\n";
    close(this->socket_fd);
}
