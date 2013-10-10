/******************************************
*
* IMPLEMENTAÇÃO DO CLINETE DE CHAT  
*
*******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define PORT 4000


int main(int argc, char *argv[]){

	int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	char buffer[256];	
	pthread_t thread;
    
	// VERIFICA QUANTIDADES DE PARAMETROS	
    if (argc < 2) {
		fprintf(stderr,"usage %s hostname\n", argv[0]);
		exit(0);
    }

	// ADQUIRI SERVIDOR
	server = gethostbyname(argv[1]);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

	// CRIA SOCKET
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("ERROR opening socket\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);

	// 	REQUISITA CONEXÃO
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
		exit(0);
	}



	return 0;
}
