#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>


char q[100][1024];
int qsindex=0;
int qeindex=0 ;
int counter = 0;
struct cDetails {
	int index;
	int sockID;
	struct sockaddr_in cAddr;
	int len;
};
struct Node{
	struct cDetails Client;
	struct Node* next;
};
struct Node *clients = NULL;
pthread_t thread[1024];
pthread_mutex_t lock; 

int writing = 0 ;


void printer(){
	for(int i=qsindex ; i<qeindex ; i++)
		printf("%d : %s\n", i-qsindex , q[i] );
}

void * clientThread(void * tclient){
	struct cDetails* ClientDetail = (struct cDetails*) tclient;
	int index = ClientDetail->index, cSoc = ClientDetail->sockID;
	printf("Client number %d connected.\n",index + 1);
	while(1){
		char data[1024];
		int read = -2;
		if(read = recv(cSoc,data,1024,0)>-1){
			printf("here  %s\n",data);
			//char output[1024];
			if(strcmp(data,"WRITE")==0){
				pthread_mutex_lock(&lock);
				writing = 1 ;
				struct Node* temp = clients ;
				for(int i = 0 ; i < counter ; i ++){
					if(temp->Client.index != index){
						send(temp->Client.sockID,"W",1024,0);
					
					}
					if(temp->next!=NULL)
						temp = temp->next;
					else
						break;
				}

				read = recv(cSoc,data,1024,0);
				strcpy(q[qeindex],data);
				qeindex++;
				 
				temp = clients ;
				for(int i = 0 ; i < counter ; i ++){
					if(temp->Client.index != index){
						send(temp->Client.sockID,"DW",1024,0);
					
					}
					if(temp->next!=NULL)
						temp = temp->next;
					else
						break;
				}
				pthread_mutex_unlock(&lock);
				writing = 0;
				printer();

			}
			if(strcmp(data,"READ")==0){
				//while(writing==1);
				for(int i=qsindex ; i<qeindex ; i++)
					send(cSoc,q[i],1024,0);
			}
			if(strcmp(data,"DEQUEUE")==0){
				pthread_mutex_lock(&lock);
				qsindex++;
				pthread_mutex_unlock(&lock);
			}
			continue;
		}
	}
	printf("Server outed\n");
	return NULL;
}


int main(){
	int sSoc = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(2344);
	server.sin_addr.s_addr = htons(INADDR_ANY);
	if(bind(sSoc,(struct sockaddr *) &server , sizeof(server)) == -1) 
		return 0;
	if(listen(sSoc,1024) == -1) 
		return 0;
	printf("Server started\n");
	struct Node *temp = NULL;
	clients = (struct Node*)malloc(sizeof(struct Node)) ;
	struct cDetails Client ;
	temp = clients ;
	int clientCounter = counter ;
	while(1){
		Client.sockID = accept(sSoc, (struct sockaddr*) &Client.cAddr, &Client.len);
		Client.index = clientCounter;
		pthread_create(&thread[clientCounter], NULL, clientThread, (void *) &Client);
		temp->Client = Client ;
		temp->next = (struct Node*)malloc(sizeof(struct Node)) ;
		temp = temp->next;
		counter ++;
		clientCounter ++ ;
	}
}
