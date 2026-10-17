#include "db.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <memory>
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


unique_ptr<RespPayload> Database::execute(vector<string> cmd)
{
    if (cmd.empty())
    {
        return make_unique<RespErr>(string("empty command"));
    }

    transform(cmd[0].begin(), cmd[0].end(), cmd[0].begin(),
                   [](unsigned char c) { return toupper(c); });

    string name = cmd[0];

    // TODO: docs command
    if(name == "COMMAND") {
        return make_unique<RespSimpleStr>(string("OK"));
    }
    
    if (name == "SET") {
        if (cmd.size() != 3) {
            return make_unique<RespErr>(string("wrong number of arguments for 'SET'\n"));
        }
        set(cmd[1], cmd[2]);
        return make_unique<RespSimpleStr>(string("OK"));
    }
    else if (name == "GET") {
        if (cmd.size() != 2) {
            return make_unique<RespErr>("wrong number of arguments for 'GET'\n");
        }
        auto result = get(cmd[1]);
        if (result.has_value()) {
            return make_unique<RespBulk>(*result);
        } else {
            // TODO: return real (nil)
            return make_unique<RespSimpleStr>(string("(nil)"));
        }
    }
    else if (name == "DEL") {
        if (cmd.size() != 2) {
            return make_unique<RespErr>("wrong number of arguments for 'DEL'\n");
        }
        int res = del(cmd[1]);
        return make_unique<RespInt>(res);
    }
    return make_unique<RespErr>("unknown command");
}
