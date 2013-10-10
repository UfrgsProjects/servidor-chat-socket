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
pthread_mutex_t mutex;

void send_message_to_user(char* messageToSend, int id){

	char message[MESSAGE_SIZE];
	sprintf(message,"%s\n",messageToSend);
	write(id, message, MESSAGE_SIZE);

}

// CRIA SALAS DE CHAT
void create_room(char* name, int id_usuario){
	printf("Enter Create ROOM \n");		
	
	pthread_mutex_lock(&mutex);		
		// CRIA SALA	
		ROOM* r = (ROOM*) malloc ( sizeof(ROOM) );
		strcpy(r->name, name);	
		r->id = id_usuario;
		r->list =  (USER*) malloc( sizeof(USER) );
	
		// ADICIONA NA LISTA DE SALAS
		ROOM* aux = room;
		ROOM* aux2 = room;
		int happen = 0;
			
		while(aux != NULL){
			aux2 = aux;			
			aux = aux->prox;
						
			if(! strcmp(aux2->name, name) ){
				send_message_to_user("Chat já existe",id_usuario);
				happen = 1;
			}
		}
		if(happen == 0){
			aux2->prox = r;
			r->prox = NULL;	
		}		
	pthread_mutex_unlock(&mutex);

	send_message_to_user("Sala de Chat Criada", id_usuario);
	printf("END Create ROOM\n");
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
	
	send_message_to_user("Nome Alterado", id);
}

// ADICIONA USUARIO NO SISTEMA
void add_user_to_lobby(char* name, int id){
	printf("Adding user: %s %d\n",name, id);		

	pthread_mutex_lock(&mutex);	
		USER* new_user = (USER*) malloc( sizeof(USER) );			
		strcpy(new_user->name, name);	
		new_user->id = id;
		new_user->inRoom = 0;

		// ADICIONA NA LISTA DE SALAS
		USER* aux = lobby;
	
		while(aux->prox != NULL)
			aux = aux->prox;
		aux->prox = new_user;
		new_user->prox = NULL;
	pthread_mutex_unlock(&mutex);	
}

void show_rooms(int id){
	ROOM* aux = room;
	
	send_message_to_user("\n ---- Chats Disponíveis ---- ",id);
	while(aux != NULL){
		send_message_to_user(aux->name,id);
		aux = aux->prox;				
	}
	send_message_to_user("",id);		
}

// REMOVE USUARIO DO SERVIDOR E DO CHAT
void remove_user(int id){
	printf("ELIMINANDO USUARIO DE TUDO");	
	pthread_mutex_lock(&mutex);	
	
		USER* aux = lobby;
		USER* aux2 = lobby;
	
		while(aux != NULL){
			if(aux->id == id){
				aux2->prox = aux->prox;
				free(aux);
				break;				
			}
			aux2 = aux;
			aux = aux->prox;
		}

		ROOM* raux = room;
		while(raux != NULL){
			aux = raux->list;
			aux2 = raux->list;
			while(aux != NULL){
				if(aux->id == id){
					aux2->prox = aux->prox;
					free(aux);
					break;				
				}
				aux2 = aux;
				aux = aux->prox;
			}
			raux = raux->prox;
		}
		
	printf("ENDING REMOVE\n");
	pthread_mutex_unlock(&mutex);	
}

void leave_from_chat(char* name, int id){
	pthread_mutex_lock(&mutex);	
		
		USER* aux;
		USER* aux2;
		int found = 0;
		USER* look = lobby;

		while(look != NULL){
			if(look->id == id)
				break;
			look = look->prox;
		}	
		
		ROOM* raux = room;
		while(raux != NULL){
			if(! strcmp(raux->name, name)){			
				aux = raux->list;
				aux2 = raux->list;
				while(aux != NULL){
					if(aux->id == id){
						aux2->prox = aux->prox;
						free(aux);
						look->inRoom = 0;
						found  = 1;
						break;				
					}
					aux2 = aux;
					aux = aux->prox;
				}
			 }
		   if(found == 1)
				break;			
		   raux = raux->prox;
		}
		if(raux == NULL){
			send_message_to_user("Acho que vc digitou errado, digite certo", id);		
		}else{
			send_message_to_user("Saiu da chat", id);
		}
		

	pthread_mutex_unlock(&mutex);	
}

void add_user_in_chat(char* name, int id){
	pthread_mutex_lock(&mutex);		
		printf("Add User in Chat\n");		
		ROOM* aux = room;
		ROOM* aux2 = room;

		USER* look = lobby;
		while(look != NULL){
			if(look->id == id)
				break;
			look = look->prox;
		}	
		
		if(look->inRoom == 0){

			while(aux != NULL){
				aux2 = aux;
				
				if(! strcmp(aux->name, name)){
					USER* temp = aux2->list;
					while(temp->prox != NULL)
						temp = temp->prox;
					USER* u = (USER*) malloc( sizeof(USER) );
					strcpy(u->name,name);
					u->id = id;
					u->prox = NULL;
					u->inRoom = 1;		
					temp->prox = u;
					send_message_to_user("Cadastrado no Chat", id);	
					look->inRoom = 1;	
					break;
				}
				aux = aux->prox;
			}
           if(aux == NULL){
			 send_message_to_user("Acho eu não vi essa sala por aqui, Digite certo", id);	 		
		  }		
			
		}else{
			send_message_to_user("Saia de um Chat para entrar em outro", id);	
		}	
		printf("END ADD User in Chat\n");	
	pthread_mutex_unlock(&mutex);	
}



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
			  char name[MESSAGE_SIZE];
			  sscanf(buffer, "--chat %s", name);	
			  create_room(name, newsockfd);	

		}else if(strstr(buffer, "--join") != 0){
			  char name[MESSAGE_SIZE];
			  sscanf(buffer, "--join %s", name);	
			  add_user_in_chat(name, newsockfd);	

		}else if(strstr(buffer, "--leave") != 0){
			  char name[MESSAGE_SIZE];
			  sscanf(buffer, "--leave %s", name);	
			  leave_from_chat(name, newsockfd);

		}else if(strstr(buffer, "--close") != 0){
               send_message_to_user("Get out", newsockfd);
			   remove_user(newsockfd);			
			   close(newsockfd);		

		}else if(strstr(buffer, "--list") != 0) {
			  show_rooms(newsockfd);	
		
		}else{
			// Mandar mensagens para todos usuarios que 	
		}
	}
}


int main(){
		
	int sockfd, newsockfd, n;
	socklen_t clilen;
	char buffer[MESSAGE_SIZE];
	struct sockaddr_in serv_addr, cli_addr;
	pthread_t thread;
	pthread_mutex_init(&mutex, NULL);	

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
