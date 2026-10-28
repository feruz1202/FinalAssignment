// TODO: set, get, del APIs
#include <optional>
#include <vector>
#include "resp.hpp"

class Client {
    public:
        Client(char* addr, int port);
        ~Client();
        RespVal sendCmd(std::vector<RespBulkStr> cmd); 

        std::optional<std::string> get(std::string key);
        void set(std::string key, std::string val);
    protected:
        int socket_fd;
};
