#include <iostream>
#include <ostream>
#include "../include/client.hpp"
#include "../include/resp.hpp"
int main() {
    char* host = "0.0.0.0";
    Client client = Client(host, 6379);

    std::cout << 
        "Setting key and value " 
        << "key: name, " 
        << "name: bobur" 
        << std::endl;
    try {
        client.set("name", "bobur");
        auto val = client.get("name");
        if(val.has_value()) {
            std::cout << "the value: " << *val << std::endl;
        }
    } catch(RespErrVal& err) {
        std::cout << "an error from the server: " << err.message << std::endl;
        return 1;
    }
}
