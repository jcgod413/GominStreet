#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include "server.h"

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
	server_addr.sin_port = htons(19999);
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		printf("bind error\n");
		exit(1);
	}

	//listen
	if (listen(server_fd, 5) < 0){
		printf("listen error\n");
		exit(1);
	}

	client_addr_len = sizeof(client_addr);
	while(1){
		//accept
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_fd < 0){
			printf("accept error\n");
			continue;
		}

		//create thread
		thread_parameter.client_fd = client_fd;
		pthread_create(&thread_id, NULL, communication_thread, (void *)&thread_parameter);
	}
}

void *communication_thread(void *arg){
	int client_fd;
	char message[200];
	ssize_t read_size;
	string send_message;
	char send_buf[200];
	char *save_ptr;
	char *tokken;
	char *id, *password;

	client_fd = (int)((thread_param*)arg)->client_fd;
	read_size = 0;
	while ((read_size += read(client_fd, message, sizeof(message))) <= 0){
		if (read_size != sizeof(message)){
			continue;
		}
		read_size = 0;

		tokken = strtok_r(message, DELIM, &save_ptr);
		switch (atoi(tokken)){
		case CHK_ID: //order_num|ID
			int has_same_id;
			id = strtok_r(NULL, DELIM, &save_ptr);
			/*
			DB
			*/
			has_same_id = 1;
			send_message = "" + CHK_ID;
			send_message += DELIM;
			send_message += has_same_id;//Áßº¹ ¿©ºÎ
			strcpy(send_buf, send_message.c_str());
			write(client_fd, send_buf, sizeof(send_buf));
			break;
		case CREATE_ID://order_num|id|password
			int is_success;
			id = strtok_r(NULL, DELIM, &save_ptr);
			password = strtok_r(NULL, DELIM, &save_ptr);
			/*
			DB
			*/

			break;
		case LOGIN:
			break;
		case RELOAD:
			break;
		case ACCESS_ROOM:
			break;
		case CREATE_ROOM: //game thread create

			break;
		}
	}
}

void *game_thread(void *arg){
	
}