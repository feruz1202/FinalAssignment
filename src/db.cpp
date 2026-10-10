#include "db.hpp"
#include <unordered_map>
#include <string>

void Database::set(string key, string value) {
    store.insert_or_assign(key, value);
}

optional<string> Database::get(string key) {
    auto it = store.find(key);
    if (it == store.end()) {
        return nullopt;
    }
    return it->second;
}

bool Database::del(string key) {
    return store.erase(key) > 0;
}