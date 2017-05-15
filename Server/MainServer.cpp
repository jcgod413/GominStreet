#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include "mainserver.h"

using namespace std;
void *communication_thread(void *);

int main(void)
{
	struct sockaddr_in server_addr, client_addr;
	int server_fd, client_fd;
	socklen_t client_addr_len;
	thread_param thread_parameter;
	pthread_t thread_id;

	//create socket
	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_fd < 0){
		printf("socket error\n");
		exit(1);
	}

	//bind
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		printf("bind error\n");
		exit(1);
	}

	//listen
	if (listen(server_fd, 5) < 0){
		printf("listen error\n");
		exit(1);
	}
	printf("Server running ...\n");

	client_addr_len = sizeof(client_addr);
	while(1){
		//accept
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_fd < 0){
			printf("accept error\n");
			continue;
		}
		printf("User entered\n");

		//create thread
		thread_parameter.client_fd = client_fd;
		pthread_create(&thread_id, NULL, communication_thread, (void *)&thread_parameter);
	}
}

bool validityCheck(Message *message)
{		
	printf("Received Message :\n");
	printf("Identifer : ");	for(int i=0; i<IDENTIFIER_SIZE; i++)	printf("%c", message->identifier[i]);
	printf("\nCategory : %c %c \n", message->category[Major], message->category[Minor]);
	printf("data : %s \n\n", message->data);

	for(int i=0; i<IDENTIFIER_SIZE; i++)	{
		if(message->identifier[i] != IDENTIFIER[i] )
			return false;
	}
	return true;
}

void *communication_thread(void *arg){
	int clientFD;
	ssize_t readSize = 0;
	bool isSuccess;
	Message message;

	clientFD = (int)((thread_param*)arg)->client_fd;
	printf("thread created. client fd : %d\n", clientFD);

	while( (readSize += read(clientFD, (char*)&message, PACKET_SIZE-readSize)) >= 0 ) 	{
		if( readSize < PACKET_SIZE )
			continue;
		readSize = 0;

		// message validity check
		if( validityCheck(message) == false )	{
			printf("Error : Invalid message\n");
			continue;
		}
	}
}

void *game_thread(void *arg){
	
}