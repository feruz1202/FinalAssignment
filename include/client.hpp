// TODO: set, get, del APIs
class Client {
    public:
        Client(char* addr, int port);
        ~Client();
    protected:
        int socket_fd;
};


