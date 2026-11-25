server:
	@g++ -std=c++17 -Wall -Wextra -Iinclude src/bin/server.cpp include/resp.cpp src/db.cpp src/tcp_server.cpp -o server

client:
	 @g++ -std=c++17 -Wall -Wextra -Iinclude include/client.cpp  example/cli-client.cpp include/resp.cpp -o client
