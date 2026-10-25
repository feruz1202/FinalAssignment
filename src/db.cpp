#include "db.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <variant>
#include <vector>
#include "../include/resp.hpp"
using namespace std;
void Database::set(string key, string value)
{
    store.insert_or_assign(key, value);
}

optional<string> Database::get(string key)
{
    auto it = store.find(key);
    if (it == store.end())
    {
        return nullopt;
    }
    return it->second;
}

bool Database::del(string key)
{
    return store.erase(key) > 0;
}


RespVal Database::execute(vector<string> cmd)
{
    if (cmd.empty())
    {
        return RespVal(RespErrVal("empty command"));
    }

    transform(cmd[0].begin(), cmd[0].end(), cmd[0].begin(),
                   [](unsigned char c) { return toupper(c); });

    string name = cmd[0];

    // TODO: docs command
    if(name == "COMMAND") {
        return RespVal("OK");
    }
    
    if (name == "SET") {
        if (cmd.size() != 3) {
            return RespVal(RespErrVal("wrong number of arguments for 'SET'"));
        }
        set(cmd[1], cmd[2]);
        return RespVal("OK");
    }
    else if (name == "GET") {
        if (cmd.size() != 2) {
            return RespVal(RespErrVal("wrong number of arguments for 'GET'"));
        }
        auto result = get(cmd[1]);
        if (result.has_value()) {
            return RespVal(RespBulkStr(*result));
        } else {
            return RespVal(std::monostate{});
        }
    }
    else if (name == "DEL") {
        if (cmd.size() != 2) {
            return RespVal(RespErrVal("wrong number of arguments for 'DEL'"));
        }
        int res = del(cmd[1]);
        return RespVal(res);
    }
    return RespVal(RespErrVal("unknown command"));
}
