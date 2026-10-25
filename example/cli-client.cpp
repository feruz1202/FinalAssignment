#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "../include/client.hpp"
#include "../include/resp.hpp"
#include <vector>
#define INPUT_SIZE 1024


std::vector<std::string> split(const std::string& str, 
        const std::string& delimiters,
        bool keepEmpty = false) {
    std::vector<std::string> tokens;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;

    while ((pos = str.find_first_of(delimiters, prev)) != std::string::npos) {
        if (keepEmpty || pos > prev) {
            tokens.push_back(str.substr(prev, pos - prev));
        }
        prev = pos + 1;
    }

    if (prev < str.length()) {
        tokens.push_back(str.substr(prev));
    } else if (keepEmpty && prev == str.length()) {
        tokens.push_back("");
    }

    return tokens;
}

std::vector<RespBulkStr> map_tokens(std::vector<std::string> tokens) {
    std::vector<RespBulkStr> result;
    for(std::string s: tokens) {
        result.push_back(RespBulkStr(s));
    }
    return result;
}


std::string format_resp_val(RespVal resp) {
    std::string formatted;

    switch (resp.data.index()) {
        case 0: { // std::string
            // Just return the string directly (or wrap in quotes if preferred)
            formatted = std::get<std::string>(resp.data);
            break;
        }
        case 1: { // int
            formatted = std::to_string(std::get<int>(resp.data));
            break;
        }
        case 2: { // std::vector<RespBulkStr>
            const auto& vec = std::get<std::vector<RespBulkStr>>(resp.data);
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                oss << vec[i].data; 
                if (i + 1 < vec.size()) oss << ", ";
            }
            oss << "]";
            formatted = oss.str();
            break;
        }
        case 3: { // RespBulkStr
            std::ostringstream oss;
            RespBulkStr data = std::get<3>(resp.data);
            oss << data.data; // Replace with .value if needed
            formatted = oss.str();
            break;
        }
        case 4: { // RespErrVal
            std::ostringstream oss;
            RespErrVal data = std::get<RespErrVal>(resp.data);
            oss << "(error) " << data.message; 
            formatted = oss.str();
            break;
        }
        default:
            formatted = "(unknown type)";
            break;
    }

    return formatted;
}

int main (int argc, char *argv[]) {
    char* host = "0.0.0.0";
    Client client = Client(host, 6379);

    while(true) {
        std::cout << "127.0.0.1> ";
        std::string input; 
        getline(std::cin, input);
        std::vector<RespBulkStr> tokens = map_tokens(split(input, " "));
        RespVal response = client.sendCmd(tokens);
        std::cout << format_resp_val(response.data) << std::endl;
    }
    return 0;
}
