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
#include "type.h"

//#define MESSAGE_SIZE 256


// THREAD QUE SOMENTE IMPRIME DADOS ENVIADOS DO SERVIDOR
void *clienteWork(void * arg) {
	char buffer[MESSAGE_SIZE];
	int n;
	int sockfd = *(int *) arg;

	/* read from the socket */
	while(1){
		bzero(buffer, MESSAGE_SIZE);
		n = read(sockfd, buffer, MESSAGE_SIZE);
		if (n <= 0) {
			printf("Disconnected from server\n");
			close(sockfd);
			pthread_exit(NULL);
		}

		printf("%s",buffer);
	}
}

void options(){
	system("clear");	
	printf("\t\t COMANDOS \n\n");	
	printf("Mudar NickName:\t--nickname [name]\n");
	printf("Criar Chat:    \t--chat [name]\n");
	printf("Entrar Chat:   \t--join [chat_name]\n");
	printf("Listar Chats:  \t--list\n");	
	printf("Sair Chat:     \t--leave [chat_name]\n");
	printf("Fechar Chat:   \t--close [chat_name]\n");
	printf("_____________________________________________________________\n\n");
	
			
}


void getUserName(char* userName){
	// INCIALIZA NOME DO USUARIO
	system("clear");
	printf("\t\t START SERVIDOR-CHAT-SOCKET\n\n");
	printf("Enter NickName: ");
	//scanf(" %MESSAGE_SIZE[^\n]",userName);		
	fgets(userName, MESSAGE_SIZE, stdin);		
	printf("Your name is: %s\n", userName);	
	if (userName[MESSAGE_SIZE - 1] == '\n')
	    userName[MESSAGE_SIZE - 1] = '\0';

}



int main(int argc, char *argv[]){

	int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	char buffer[MESSAGE_SIZE];
	char userName[MESSAGE_SIZE];	
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
	}else{
	
		// CRIA THREAD DO USUARIO	
		pthread_create(&thread, NULL, clienteWork, &sockfd);
		
		// Ao inicializar servidor manda dados do usuario para servidor
		getUserName(userName);
		strcpy (buffer,"--firstacess ");
		strcat (buffer, userName);
		write(sockfd, buffer, strlen(buffer));
		options();
		
		// ESCREVER MENSAGENS PARA SERVIDOR		
		while(1) {
					
			bzero(buffer, MESSAGE_SIZE);
			fgets(buffer, MESSAGE_SIZE, stdin);
					

			/* write in the socket */
			n = write(sockfd, buffer, strlen(buffer));

			if(strstr(buffer, "--close") != 0){
					sleep(2);			
					break;
			}

			if (n < 0) {
				printf("ERROR writing to socket\n");
				exit(1);
			}
		}
		close(sockfd);	
	}
	return 0;
}
