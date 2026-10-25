#pragma once
#include <string>
#include <vector>
#include <variant>
#define MAX_RESP_BUFFER 4096


class RespErrVal {
    public:
        RespErrVal(std::string msg);
        std::string message;
};

class RespBulkStr {
    public: 
        RespBulkStr(std::string data);
        std::string data;
};

class RespVal {
    public:
        using ValueType = std::variant<
            std::string,
            int,
            std::vector<RespBulkStr>,
            RespBulkStr,
            RespErrVal,
            std::monostate
            >;
        RespVal(ValueType v);
        ValueType data;
    protected:
};

int writeResp(int fd, RespVal val);

RespVal readResp(int fd);
