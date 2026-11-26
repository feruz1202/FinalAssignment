#include <iostream>
#include <optional>
#include <string>
#include "../include/client.hpp"
#include "../include/resp.hpp"
int main() {
    char host[] = "127.0.0.1";
    Client client = Client(host, 6379);
    const char* key = "name";
    const char* value = "tohir";

    std::cout << 
        "Setting key and value " 
        << "key: " << key << ", " 
        << "value: " << value 
        << std::endl;
    try {
        client.set(key, value);
        auto val = client.get(key);
        if(val.has_value()) {
            std::cout << "the value: " << val.value() << std::endl;
        }
    } catch(RespErrVal& err) {
        std::cout << "an error from the server: " << err.message << std::endl;
        return 1;
    }
}
