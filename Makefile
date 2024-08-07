all: 
	g++ demo_server.cpp -o demo_server -lpthread
	g++ demo_client.cpp -o demo_client -lpthread
