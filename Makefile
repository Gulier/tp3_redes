all: clienteFTP servidorFTP

client.o:	client.c
	gcc -c client.c tp_socket.c -lm

server.o:	server.c
	gcc -c server.c tp_socket.c -lm

clienteFTP:	client.o
	gcc client.o tp_socket.o -o clienteFTP -lm

servidorFTP:	server.o
	gcc server.o tp_socket.o -o servidorFTP -lm

clean:
	-rm all -f client.o server.o tp_socket.o -lm
