#include "resp.hpp"
#include <stdexcept>
#include <string>
#include <unistd.h>


void RespPayload::writeToClient(int fd) {
    // no op
}

RespErr::RespErr(const std::string& s) {
    this->buffer.push_back('-');
    this->buffer.insert(this->buffer.end(), s.begin(), s.end());
    this->buffer.push_back('\r');
    this->buffer.push_back('\n');
}

void RespErr::writeToClient(int fd) {
     size_t n = write(fd, this->buffer.data(), this->buffer.size());
     if(n <= 0) {
         throw std::runtime_error(std::string("could not write to client"));
     }
}


RespSimpleStr::RespSimpleStr(const std::string& s) {
    this->buffer.push_back('+');
    this->buffer.insert(this->buffer.end(), s.begin(), s.end());
    this->buffer.push_back('\r');
    this->buffer.push_back('\n');
}


void RespSimpleStr::writeToClient(int fd) {
     size_t n = write(fd, this->buffer.data(), this->buffer.size());
     if(n <= 0) {
         throw std::runtime_error(std::string("could not write to client"));
     }
}


RespMultiBulk::RespMultiBulk(const std::vector<std::string>& data) {
    this->buffer.push_back('*');
    std::string len = std::to_string(data.size());
    this->buffer.insert(this->buffer.end(), len.begin(), len.end());
    this->buffer.push_back('\r');
    this->buffer.push_back('\n');

    for(const std::string& s : data) {
        this->buffer.push_back('$');
        std::string len = std::to_string(s.length());
        this->buffer.insert(this->buffer.end(), len.begin(), len.end());

        this->buffer.push_back('\r');
        this->buffer.push_back('\n');

        this->buffer.insert(this->buffer.end(), s.begin(), s.end());

        this->buffer.push_back('\r');
        this->buffer.push_back('\n');
    }
}


void RespMultiBulk::writeToClient(int fd) {
     size_t n = write(fd, this->buffer.data(), this->buffer.size());
     if(n <= 0) {
         throw std::runtime_error(std::string("could not write to client"));
     }
}

RespBulk::RespBulk(const std::string& s) {
    std::string header = "$" + std::to_string(s.size()) + "\r\n";
    this->buffer.insert(this->buffer.end(), header.begin(), header.end());
    this->buffer.insert(this->buffer.end(), s.begin(), s.end());
    this->buffer.push_back('\r');
    this->buffer.push_back('\n');
}

void RespBulk::writeToClient(int fd) {
     size_t n = write(fd, this->buffer.data(), this->buffer.size());
     if(n <= 0) {
         throw std::runtime_error(std::string("could not write to client"));
     }
}

RespInt::RespInt(int val) {
    std::string val_str = std::to_string(val);
    buffer.push_back(':');
    buffer.insert(buffer.end(), val_str.begin(), val_str.end());
    this->buffer.push_back('\r');
    this->buffer.push_back('\n');
}

void RespInt::writeToClient(int fd) {
     size_t n = write(fd, this->buffer.data(), this->buffer.size());
     if(n <= 0) {
         throw std::runtime_error(std::string("could not write to client"));
     }
}
