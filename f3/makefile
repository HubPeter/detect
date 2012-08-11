all:	server client
server:	server.c
	gcc -o server server.c -g -lpcap
client:	client.c
	gcc -o client client.c -g -lpcap
clean:	
	rm server client
