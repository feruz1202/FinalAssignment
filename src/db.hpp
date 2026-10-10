#ifndef DB_HPP
#define DB_HPP

#include <unordered_map>
#include <string>
#include <optional>
using namespace std;
class Database {
public:
    void set(string key, string value);
    optional<string> get(string key);
    bool del(string key);

private:
    unordered_map<string, string> store;
};

#endif
