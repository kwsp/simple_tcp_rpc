all: 
	g++ -Wall -Werror -std=c++14 demo_server.cpp -o demo_server -lpthread
	g++ -Wall -Werror -std=c++11 demo_client.cpp -o demo_client -lpthread
