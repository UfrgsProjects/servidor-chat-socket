/**
*
* DEFINIÇÕES DE ESTRUTURAS E CONSTANTES
*
*/
#include <stdio.h>
#include <stdlib.h>

#define PORT 4000
#define MESSAGE_SIZE 500

typedef struct user{

	char name[MESSAGE_SIZE];
	int  id;	
	int  inRoom;	
	struct user* prox;
	struct user* friends;	

} USER;

typedef struct room{

	char name[MESSAGE_SIZE];
	int  id;
	struct user* list;
	struct room* prox; 

}ROOM;
