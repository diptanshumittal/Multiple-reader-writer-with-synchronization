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


int flag = 0 ;
int readable = 0;
int writeable = 0;

void * tWrite(void * sockID){
	int cSoc = *((int *) sockID);
	while(1){
		char input[1024];
		scanf("%s",input);
		if(strcmp(input,"READ")==0)
			send(cSoc,input,1024,0);
		else if(strcmp(input,"DEQUEUE")==0)
			send(cSoc,input,1024,0);
		else
			printf("Wrong Command entered\n");
	}
}

int main(){
	int cSoc = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(2344);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if(connect(cSoc, (struct sockaddr*) &server, sizeof(server)) == -1) 
		return 0;
	pthread_t thread;
	printf("Connected as a READER\n");
	pthread_create(&thread, NULL, tWrite, (void *) &cSoc );
	char data[1024];
	int read ;
	while(1){
		if(flag==1)
			break;
		if(read = recv(cSoc,data,1024,0)>0)
			if(!(strcmp(data,"W") == 0) && !(strcmp(data,"DW")==0))
				printf("%s\n",data);
	}
	printf("outed\n");
}