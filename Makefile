all: clean server client
server:
	g++ server.cpp common.cpp serverchannel.cpp -o server
client:
	g++ client.cpp common.cpp clientchannel.cpp -o client

server-O3:
	g++ -O3 server.cpp common.cpp serverchannel.cpp -o server
client-O3:
	g++ -O3 client.cpp common.cpp clientchannel.cpp -o client
clean:
	rm -f server client *.o
