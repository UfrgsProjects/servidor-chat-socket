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

#define MAXCONNECT 5

// VARIAVEIS GLOBAIS
USER* lobby; // LISTA DE USUARIOS DO SISTEMA
ROOM* room;  // LISTA DE CHATS DO SISTEMA

// FUNÇÕES PARA GERENCIAR SALAS DE CHAT
void create_room(char* name, int id_usuario){
	printf("Enter Create ROOM \n");		
	// CRIA SALA	
	ROOM* r = (ROOM*) malloc ( sizeof(ROOM) );
	strcpy(r->name, name);	
	r->id = id_usuario;
	r->list =  (USER*) malloc( sizeof(USER) );
	
	// ADICIONA NA LISTA DE SALAS
	ROOM* aux = room;
	while(aux->prox != NULL)
		aux = aux->prox;
	aux->prox = r;
	r->prox = NULL;

	printf("END Create ROOM\n");
}

// VERIFICA SE USUARIO CADASTRADO
int user_exist_in_lobby(int id){
	int exist = 0;
	USER* aux = lobby;

	while(aux->prox != NULL){
		if(aux->id == id){
			exist = 1; 		
			break;
		}		
		aux = aux->prox;
	}
	return exist;			
}


void change_name_user(char* name, int id){
	printf("--- Mundando nome usuario ---\n");	
	USER* aux = lobby;
	
	while(aux->prox != NULL){
		if(aux->id == id){
			strcpy(aux->name, name);	
			break;
		}		
		aux = aux->prox;
	}
	
	char message[MESSAGE_SIZE];
	sprintf(message,"%s: %s\n", "Nome Alterado", name);
	write(id, message, strlen(message));
}

void add_user_to_lobby(char* name, int id){
	printf("Adding user: %s %d\n",name, id);		

	USER* new_user = (USER*) malloc( sizeof(USER) );			
	strcpy(new_user->name, name);	
	new_user->id = id;
	
	// ADICIONA NA LISTA DE SALAS
	USER* aux = lobby;
	
	while(aux->prox != NULL)
		aux = aux->prox;
	aux->prox = new_user;
	new_user->prox = NULL;
}

/*
void show_rooms(){
	ROOM* aux = room;
	
	while(aux->prox != NULL){
			
	}
	
}
*/

// THREAD SERVIDOR - TODA LOGICA AQUI
void *serverWork(void * arg){
	char buffer[MESSAGE_SIZE];
	int n;
	int newsockfd = *(int *) arg;	// Pode ser id do usuario
	
	while(1){
		bzero(buffer, 256);

		/* read from the socket */
		n = read(newsockfd, buffer, 256);
	
		if (n <= 0) {
			close(n);			
			break;
		}

		printf("Receive Message: %s", buffer);

	   // LOGICA DO QUE O SERVIDOR DEVE FAZER 
			// IMPLEMENTAR METODOS
		
		if(strstr(buffer, "--firstacess") != 0){
			  char name[MESSAGE_SIZE]; 	
			  sscanf(buffer, "--firstacess %s", name);
			  add_user_to_lobby(name, newsockfd);
		}else if(strstr(buffer, "--nickname") != 0){
              char name[MESSAGE_SIZE];
			  sscanf(buffer, "--nickname %s", name);	
			  change_name_user(name, newsockfd);
		}else if(strstr(buffer, "--chat") != 0){
				
		}else if(strstr(buffer, "--join") != 0){

		}else if(strstr(buffer, "--leave") != 0){

		}else if(strstr(buffer, "--close") != 0){

		}else if(strstr(buffer, "--exit") != 0){

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
		
	lobby = (USER*) malloc( sizeof(USER) );		
	room = (ROOM*) malloc( sizeof(ROOM) );	
	
	lobby->prox = NULL;	
	room->prox = NULL;
	
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
