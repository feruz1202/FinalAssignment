#include "resp.hpp"
#include <string>
#include <unistd.h>
#include <variant>
#include <vector>

RespVal parseInt(std::string_view s) {
    // :[value]\r\n
    int crlf_pos = s.find("\r\n");
    if(crlf_pos == std::string_view::npos) {
        throw std::runtime_error(std::string("crlf not found on intiger"));
    }
    return  RespVal(std::string(s.substr(1, crlf_pos)));
}

RespVal parseErr(std::string_view s) {
    int crlf_pos = s.find("\r\n");
    if(crlf_pos == std::string_view::npos) {
        throw std::runtime_error(std::string("crlf not found on err type"));
    }
    return  RespVal(std::string(s.substr(1, crlf_pos)));
}


RespVal parseSimpleStr(std::string_view s) {
    size_t pos = 0;
    size_t end = s.find("\r\n", pos);
    
    if (end == std::string_view::npos) {
        throw std::runtime_error("incomplete simple string");
    }

    if (s.empty() || s[0] != '+') {
        throw std::runtime_error("invalid simple string prefix");
    }

    std::string content(s.substr(pos + 1, end - pos - 1));

    return RespVal(content);
}

RespVal parseBulkStr(std::string_view s) {
    int pos = 0;
    size_t end = s.find("\r\n", pos);          
    if (end == std::string_view::npos) {
        throw std::runtime_error("incomplete bulk length");
    }

    int len = std::stoul(std::string(s.substr(pos + 1, end - pos - 1)));
    if(len < 0) {
        // $-1\r\n null bulk
        return RespVal(std::monostate{});
    }
    pos = end + 2;                                

    if (pos + len + 2 > s.size()) {
        throw std::runtime_error("incomplete bulk payload");
    }
    
    if (s.substr(pos + len, 2) != "\r\n") {
        throw std::runtime_error("bulk string missing CRLF terminator");
    }
    std::string str = std::string(s.substr(pos, len));
    return RespVal(RespBulkStr(str));
}

RespVal parseResp(std::string_view s) {
    char t = s[0];
    switch(t) {
        case '+': 
            return parseSimpleStr(s);
            break;
        case '-': 
            return parseErr(s);
            break;
        case '$': 
            return parseBulkStr(s);
            break;
        case ':':
            return parseInt(s);
        default:
            throw std::runtime_error("unknown RESP type");
        // TODO: this is a bit complicated, so later.
        // case '*': break;
    }
}

RespVal readResp(int fd) {
    char buffer[MAX_RESP_BUFFER];
    size_t n = read(fd, buffer, sizeof(buffer));
    if (n <= 0) {
        throw std::runtime_error(std::string("read failed: ") + std::strerror(errno));
    }
    std::string_view view(buffer, n);
    return parseResp(view);
}




RespVal::RespVal(RespVal::ValueType v) {
    this->data = v;
}


RespErrVal::RespErrVal(std::string msg) {
    this->message = msg;
}

RespBulkStr::RespBulkStr(std::string data) {
    this->data = data;
}

void serializeRespErr(std::vector<char>& buffer, const RespErrVal& s) {
    buffer.push_back('-');
    buffer.insert(buffer.end(), s.message.begin(), s.message.end());
    buffer.push_back('\r');
    buffer.push_back('\n');
}

void serializeSimpleStr(std::vector<char>& buffer, const std::string& s) {
    buffer.push_back('+');
    buffer.insert(buffer.end(), s.begin(), s.end());
    buffer.push_back('\r');
    buffer.push_back('\n');
}

void serializeBulkStr(std::vector<char>& buffer, const RespBulkStr& s) {
    std::string header = "$" + std::to_string(s.data.size()) + "\r\n";
    buffer.insert(buffer.end(), header.begin(), header.end());
    buffer.insert(buffer.end(), s.data.begin(), s.data.end());
    buffer.push_back('\r');
    buffer.push_back('\n');
}

void serializeRespMultiBulk(std::vector<char>& buffer, const std::vector<RespBulkStr>& data) {
    buffer.push_back('*');
    std::string len = std::to_string(data.size());
    buffer.insert(buffer.end(), len.begin(), len.end());
    buffer.push_back('\r');
    buffer.push_back('\n');

    for(const RespBulkStr& s : data) {
        serializeBulkStr(buffer, s);
    }
}


void serializeInt(std::vector<char>& buffer, int val) {
    std::string val_str = std::to_string(val);
    buffer.push_back(':');
    buffer.insert(buffer.end(), val_str.begin(), val_str.end());
    buffer.push_back('\r');
    buffer.push_back('\n');
}


int writeResp(int fd, RespVal val) {
    std::vector<char> buffer = {};
    switch (val.data.index()) {
        // string
        case 0: 
            {
                std::string copy = std::get<0>(val.data);
                serializeSimpleStr(buffer, copy);
                break;
            }
        // int  
        case 1: 
            {
                int data = std::get<1>(val.data);
                serializeInt(buffer, data);
                break;
            }
        // vec<string>
        case 2: 
            {
                std::vector<RespBulkStr> multiBulk = std::get<2>(val.data); 
                serializeRespMultiBulk(buffer, multiBulk);
                break;
            }
        // RespBulkStr
        case 3: 
            {
                RespBulkStr bulk = std::get<3>(val.data);
                serializeBulkStr(buffer, bulk);
                break;
            }
        // RespErrVal 
        case 4:
            {
                RespErrVal err = std::get<4>(val.data);
                serializeRespErr(buffer, err);
                break;
            }
        // (nil)
        case 5:
            {
                buffer.push_back('$');
                buffer.push_back('-');
                buffer.push_back('1');
                buffer.push_back('\r');
                buffer.push_back('\n');
            }
    }

    return write(fd, buffer.data(), buffer.size());
}
