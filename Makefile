all: clean server client
server:
	g++ server.cpp common.cpp serverchannel.cpp -o server
client:
	g++ client.cpp common.cpp clientchannel.cpp -o client
clean:
	rm -f server client *.o
