#pragma once
#include <string>
#include <vector>
#define MAX_RESP_BUFFER 4096



class RespPayload {
    public:
        virtual void writeToClient(int fd);
        virtual ~RespPayload() = default;
};

class RespMultiBulk : public RespPayload {
    std::vector<char> buffer;
    public:
        RespMultiBulk(const std::vector<std::string>& data);
        void writeToClient(int fd) override;
};

class RespBulk : public RespPayload {
    std::vector<char> buffer;
    public:
        RespBulk(const std::string& s);   
        void writeToClient(int fd) override;
};

class RespSimpleStr : public RespPayload {
    std::vector<char> buffer;
    public:
        RespSimpleStr(const std::string& s);
        void writeToClient(int fd) override;
};

class RespErr : public RespPayload {
    std::vector<char> buffer;
    public: 
        RespErr(const std::string& s);
        void writeToClient(int fd) override;
};

class RespInt: public RespPayload {
    std::vector<char> buffer; 
    public:
        RespInt(int val);
        void writeToClient(int fd) override;
};
