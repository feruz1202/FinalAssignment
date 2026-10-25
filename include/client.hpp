// TODO: set, get, del APIs
#include <vector>
#include "resp.hpp"

class Client {
    public:
        Client(char* addr, int port);
        ~Client();
        RespVal sendCmd(std::vector<RespBulkStr> cmd); 

        void get(RespVal cmd);
        void set(RespVal cmd);
        void del(RespVal cmd);
    protected:
        int socket_fd;
};


