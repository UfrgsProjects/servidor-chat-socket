/******************************************
*
* IMPLEMENTAÇÃO DO SERVIDOR DE CHAT
*
*******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "type.h"

#define MESSAGE_SIZE 256
#define MAXCONNECT 5

// THREAD SERVIDOR - TODA LOGICA AQUI
void *serverWork(void * arg){
	char buffer[MESSAGE_SIZE];
	int n;
	int newsockfd = *(int *) arg;	// Pode ser id do usuario
	
	while(1){
		bzero(buffer, 256);

		/* read from the socket */
		n = read(newsockfd, buffer, 256);
	
		printf("Receive Message: %s", buffer);

	   // LOGICA DO QUE O SERVIDOR DEVE FAZER 
			// IMPLEMENTAR METODOS
		if(strstr(buffer, "--nickname ") != 0){
				
		}else if(strstr(buffer, "--chat ") != 0){

		}else if(strstr(buffer, "--join ") != 0){

		}else if(strstr(buffer, "--leave ") != 0){

		}else if(strstr(buffer, "--close ") != 0){

		}else if(strstr(buffer, "--exit ") != 0){

		}else{

		}
	}
}


int main(){
		
	int sockfd, newsockfd, n;
	socklen_t clilen;
	char buffer[MESSAGE_SIZE];
	struct sockaddr_in serv_addr, cli_addr;
	pthread_t thread;
	//pthread_mutex_init(&m, NULL);

	// CRIA SOCKET
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("ERROR opening socket\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);

	// ASSOCIA ENDEREÇO/PORTA
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("ERROR on binding port\n");
		exit(1);
	}

	// DISPONIBILIZA SERVIDOR PARA ACEITAR CONEXÕES, 
	// DISPONIBILIZA MAXCONNECT CONEXÕES PENDENTES
	listen(sockfd, MAXCONNECT);

	clilen = sizeof(struct sockaddr_in);
	

	// MANTEM SERVIDOR SEMPRE PRONTO PARA UMA CONEXÃO 
	while(1) {
		printf("Waiting for connections...\n");

		// FUNÇÃO ACCEPT SEGURA ATE QUE EXISTA UMA CONEXÃO, PRESSO DENTRO DO ACCEPT
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1)
			printf("ERROR on accept\n");

		printf("Socket id => %d\n", newsockfd);
		
		// CRIA UMA CONEXÃO PARA CADA SERVIDOR - SERVIDOR CONCORRENTE
		pthread_create(&thread, NULL, serverWork, &newsockfd); 
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}
