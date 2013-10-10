compile-server:
	gcc -Wall servidor.c -lpthread -o servidor

compile-cliente:
	gcc -Wall cliente.c -lpthread -o cliente
