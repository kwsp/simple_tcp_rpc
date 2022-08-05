all: 
	g++ tcp_server.cpp -o tcp_server -lpthread
	g++ tcp_client.cpp -o tcp_client -lpthread
