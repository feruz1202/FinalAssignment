#ifndef DB_HPP
#define DB_HPP
#include "../include/resp.hpp"
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>
class Database {
    public:
        void set(std::string key, std::string value);
        std::optional<std::string> get(std::string key);
        bool del(std::string key);
        std::unique_ptr<RespPayload> execute(std::vector<std::string> cmd);

    private:
        std::unordered_map<std::string, std::string> store;
};

#endif
