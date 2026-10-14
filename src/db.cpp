#include "db.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>
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

void Database::execute(vector<string> cmd)
{
    if (cmd.empty())
    {
        cout << "(error) empty command\n";
        return;
    }

    // 2. Make the command word uppercase, so "set" and "SET" both work
    std::transform(cmd[0].begin(), cmd[0].end(), cmd[0].begin(),
                   [](unsigned char c) { return std::toupper(c); });

    string name = cmd[0];
    
     // 3. Route to the right method, checking argument count BEFORE using cmd[i]
    if (name == "SET") {
        if (cmd.size() != 3) {
            std::cout << "(error) wrong number of arguments for 'SET'\n";
            return;
        }
        set(cmd[1], cmd[2]);
        std::cout << "OK\n";
    }
    else if (name == "GET") {
        if (cmd.size() != 2) {
            std::cout << "(error) wrong number of arguments for 'GET'\n";
            return;
        }
        auto result = get(cmd[1]);
        if (result.has_value()) {
            std::cout << *result << "\n";
        } else {
            std::cout << "(nil)\n";
        }
    }
    else if (name == "DEL") {
        if (cmd.size() != 2) {
            std::cout << "(error) wrong number of arguments for 'DEL'\n";
            return;
        }
        std::cout << "(integer) " << (del(cmd[1]) ? 1 : 0) << "\n";
    }
    else {
        std::cout << "(error) unknown command '" << name << "'\n";
    }
}