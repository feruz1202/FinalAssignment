build:
	@g++ -std=c++17 -Wall -Wextra -Iinclude src/playground_main.cpp include/resp.cpp src/db.cpp src/tcp_server.cpp -o test
