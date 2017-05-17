#ifndef __MAINSERVER_CPP__
#define __MAINSERVER_CPP__

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "usermanager.h"
#include "roommanager.h"
#include "gamemanager.h"
#include "gamedata.h"
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

	// option
	int enable = true;
	if( setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0 )
		printf("setsockopt(SO_REUSEADDR) failed\n");

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

	/* test */
	printf("Server running ...\n");
	/* test */

	client_addr_len = sizeof(client_addr);
	while(1){
		//accept
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_fd < 0){
			printf("accept error\n");
			continue;
		}

		/* test */
		printf("User entered\n");
		/* test */

		//create thread
		thread_parameter.client_fd = client_fd;
		pthread_create(&thread_id, NULL, communication_thread, (void *)&thread_parameter);
	}

	exit(0);
}

bool validityCheck(Message *message)
{
	printf("----- Received Message -----\n");
	printf("Identifer : ");	for(int i=0; i<IDENTIFIER_SIZE; i++)	printf("%c", message->identifier[i]);
	printf("\nCategory : %d %d \n", message->category[Major], message->category[Minor]);
	printf("data : %s \n\n", message->data);

	//check whether identifier is GOMIN or not.
	for(int i=0; i<IDENTIFIER_SIZE; i++)	{
		if(message->identifier[i] != IDENTIFIER[i] )
			return false;
	}
	return true;
}

void sendResponse(int clientFD, Message *response)
{
	write(clientFD, (char*)response, PACKET_SIZE);

	/* test */
	printf("----- Respond Message  -----\n");
	printf("Identifer : ");	for(int i=0; i<IDENTIFIER_SIZE; i++)	printf("%c", response->identifier[i]);
	printf("\nCategory : %d %d \n", response->category[Major], response->category[Minor]);
	printf("data : %s \n\n", response->data);
	/* test */

}

void userManager(Message *message, Message *response)
{
	switch( message->category[Minor] )	{
		case User_Login: 	login(message, response);	break;
		case User_Signup: 	signup(message, response);	break;
		case User_Record: 	record(message, response);	break;
		case User_Win: 		win(message, response);		break;
		case User_Lose: 	lose(message, response);	break;
		default: printf("error : user category %d\n", message->category[Minor]);
	}
}

void roomManager(Message *message)
{
	switch( message->category[Minor] )	{
		case Room_Create: 		createRoom(message);	break;
		case Room_List: 		listRoom(message);		break;
		case Room_Enter: 		enterRoom(message);		break;
		case Room_Exit: 		exitRoom(message);		break;
		case Room_Alert_Enter: 	enterAlertRoom(message);break;
		case Room_Alert_Exit: 	exitAlertRoom(message);	break;
		case Room_Start: 		startRoom(message);		break;
		default: printf("error : room category %d\n", message->category[Minor]);
	}
}

void gameManager(Message *message)
{
	switch( message->category[Minor] )	{
		case Game_DiceRoll:		diceRoll(message); 	break;
		case Game_Turn: 		turn(message);		break;
		case Game_Move: 		move(message);		break;
		case Game_Buy: 			buy(message);		break;
		case Game_Pay: 			pay(message);		break;
		case Game_GoldKey: 		goldKey(message);	break;
		case Game_Isolation: 	isolation(message);	break;
		default: printf("error : manager category %d\n", message->category[Minor]);
	}
}

void *communication_thread(void *arg){
	int clientFD;
	ssize_t readSize = 0;
	bool isSuccess;
	Message message;

	clientFD = (int)((thread_param*)arg)->client_fd;

	/* test */
	printf("thread created. client fd : %d\n", clientFD);
	/* test */

	while( (readSize += read(clientFD, (char*)&message, PACKET_SIZE-readSize)) >= 0 ) 	{
		if( readSize < PACKET_SIZE )
			continue;
		readSize = 0;

		// message validity check
		if( validityCheck(&message) == false )	{
			printf("Error : Invalid message\n");
			continue;
		}

		//내가 받을땐 숫자로 받고, 보낼땐 문자로 보내기.

		Message response;
		strcpy(response.identifier, "GOMIN");
		response.category[Major] = message.category[Major];
		response.category[Minor] = message.category[Minor];

		printf("enum %d %d\n", Major_User, User_Login);
		switch( message.category[Major] )	{
			case Major_User: userManager(&message, &response);	break;
			case Major_Room: roomManager(&message);	break;
			case Major_Game: //방번호 파싱해서 sharedMemory messageQueue에 push
			//gameManager(&message);	break;
			default: printf("error : major category %d\n", message.category[Major]);
		}
		sendResponse(clientFD, &response);
	}
}

void *game_thread(void *arg){
	game_room game_room_info;

	((game_room *)arg)->roomID = pthread_self();
	game_room_info = *(game_room *)arg;
	sharedMemory.roomList.push_back(game_room_info);

	while(1) {//유저 수가 0
		//동기화 문제 해결(mutex)
		// 반복문을 돌아서 리스트에 들어있는 방 정보를 가져오기

		// 해당 방에 메시지 들어올때까지 블로킹(무한)

		// 요청 정보 파싱해서 해당 동작


	}
}

void createRoom(Message *message) {
	pthread_t game_thread_id;
	game_room game_room_info;
	userInfo user_info;
	char *user_idx, *FD, *save_ptr;

	user_idx = strtok_r(message->data, DELIM, &save_ptr);
	FD = strtok_r(NULL, DELIM, &save_ptr);

	user_info.number = atoi(user_info);
	user_info.FD = atoi(FD);

	game_room_info.status = WAIT;
	game_room_info.turn = 0;

	if(!game_room_info.userList.empty())
		game_room_info.userList.clear();
	game_room_info.userList.push_back(user_info);

	game_room_info.roomLeader = user_info.number;
	game_room_info.userCount = game_room_info.userList.size();

	while(!game_room_info.messageQueue.empty())
		game_room_info.messageQueue.pop();

  //create game thread
  pthread_create(&game_thread_id, NULL, game_thread, (void *)&game_room_info);
}

#endif
