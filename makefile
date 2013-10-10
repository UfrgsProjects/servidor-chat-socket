# COMPILE AND LINK SERVIDOR
compile-server:
	gcc -Wall servidor.c -lpthread -o servidor

# COMPILE AND LINK CLIENT
compile-cliente:
	gcc -Wall cliente.c -lpthread -o cliente

# COMPILE AND LINK SERVIDOR AND CLIENT
compile-all:
	gcc -Wall servidor.c -lpthread -o servidor
	gcc -Wall cliente.c -lpthread -o cliente 


