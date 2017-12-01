all: clienteFTP servidorFTP

client.o:	client.c
	gcc -c client.c tp_socket.c

server.o:	server.c
	gcc -c server.c tp_socket.c
	
clienteFTP:	client.o
	gcc client.o tp_socket.o -o clienteFTP
	
servidorFTP:	server.o
	gcc server.o tp_socket.o -o servidorFTP
	
clean:
	-rm all -f client.o server.o tp_socket.o
