#ifndef __MAINSERVER_CPP__
#define __MAINSERVER_CPP__

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <list>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "usermanager.h"
#include "roommanager.h"
#include "gamemanager.h"
#include "gamedata.h"
#include "mainserver.h"
#include "protocol.h"
using namespace std;

shared_memory sharedMemory;
bool room_number[MAX_ROOM];
pthread_mutex_t mutex_lock;

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
	memset(room_number, 0, sizeof(room_number));
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
	printf("\nCategory : %d %d \n", message->category[Major]-'0', message->category[Minor]-'0');
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
	printf("\t\t\t----- Respond Message  -----\n");
	printf("\t\t\tIdentifer : ");	for(int i=0; i<IDENTIFIER_SIZE; i++)	printf("%c", response->identifier[i]);
	printf("\n\t\t\tCategory : %d %d \n", response->category[Major]-'0', response->category[Minor]-'0');
	printf("\t\t\tdata : %s \n\n", response->data);
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

void roomManager(Message *message, Message *response, int clientFD)
{
	switch( message->category[Minor] )	{
		case Room_Create: 		createRoom(message, response, clientFD);	break;
		case Room_List: 		listRoom(message, response, clientFD);		break;
		case Room_Enter: 		enterRoom(message, response, clientFD);	break;
		case Room_Exit: 		exitRoom(message, response);			break;
		case Room_Start: 		startRoom(message, response);			break;
		default: printf("error : room category %d\n", message->category[Minor]);
	}
}

void gameManager(Message *message, Message *response)
{
	switch( message->category[Minor] )	{
		case Game_DiceRoll:		diceRoll(message, response); 	break;
		case Game_Turn: 		turn(message, response);		break;
		case Game_Buy: 			buy(message, response);		break;
		case Game_Pay: 			pay(message, response);		break;
		case Game_GoldKey: 		goldKey(message, response);	break;
		case Game_Isolation: 	isolation(message);			break;
		case Game_Salary:		salary(message);			break;
		default: printf("error : manager category %d\n", message->category[Minor]);
	}
}

void *communication_thread(void *arg){
	int clientFD;
	ssize_t readSize = 0;
	Message message;

	clientFD = (int)((thread_param*)arg)->client_fd;

	printf("thread created. client fd : %d\n", clientFD);

	while( (readSize += read(clientFD, (char*)&message, PACKET_SIZE-readSize)) >= 0 ) 	{
		if( readSize < PACKET_SIZE )
			continue;
		readSize = 0;

		//헤더가 GOMIN인지 체크
		if( validityCheck(&message) == false )	{
			printf("Error : Invalid message\n");
			continue;
		}

		// 내가 받을땐 숫자로 받고, 보낼땐 문자로 보내기.

		Message response;
		strcpy(response.identifier, "GOMIN");
		response.category[Major] = message.category[Major];
		response.category[Minor] = message.category[Minor];

		// Major_Game에서 필요
		char *save_ptr = NULL, *roomID_str = NULL;
		int roomID = 0;
		bool isFound = false;

		switch( message.category[Major] )	{
			case Major_User:
				userManager(&message, &response);
				sendResponse(clientFD, &response);
				break;

			case Major_Room:
				roomManager(&message, &response, clientFD);
				sendResponse(clientFD, &response);
				break;

			case Major_Game:
				// 방번호를 추출함
			 	roomID_str = strtok_r(message.data, DELIM, &save_ptr);
				roomID = atoi(roomID_str);
				isFound = false;

				// 방번호를 찾을 때까지 무한반복
				while(!isFound)
					for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
						if(it->roomID == roomID) {//방번호를 찾았을 때
							it->messageQueue.push(message);//공유자원의 큐에 메시지를 넘겨줌
							isFound = true;
							break;
						}
				break;

			default:
				printf("error : major category %d\n", message.category[Major]);
				break;
		}
	}
}

void deleteRoom(int roomID)
{
	printf("room size : %d\n", sharedMemory.roomList.size());
	for (list<game_room>::iterator itri = sharedMemory.roomList.begin(); itri != sharedMemory.roomList.end(); )	{
		if( itri->roomID == roomID )
			itri = sharedMemory.roomList.erase(itri);
		else
			itri++;
	}
}

void *game_thread(void *arg) {
	printf("game thread start\n");
	pthread_mutex_init(&mutex_lock, NULL);

	game_room *current_game = &sharedMemory.roomList.back();

	while(1) {
		//모두 나가면 게임 스레드 종료
		if( current_game->userCount == 0 )	{
			deleteRoom(current_game->roomID);
			pthread_exit((void *)0);
		}

		// 해당 방에 메시지 들어올때까지 블로킹(무한)
		while( current_game->messageQueue.empty() ) {
			//모두 나가면 게임 스레드 종료
			if( current_game->userCount == 0 )	{
				deleteRoom(current_game->roomID);
				// pthread_mutex_unlock(&mutex_lock);
				pthread_exit((void *)0);
			}
		}

		pthread_mutex_lock(&mutex_lock);
		Message message = current_game->messageQueue.front();
		current_game->messageQueue.pop();
		pthread_mutex_unlock(&mutex_lock);

		Message response;
		strcpy(response.identifier, "GOMIN");
		response.category[Major] = message.category[Major];
		response.category[Minor] = message.category[Minor];

		gameManager(&message, &response);
	}
}

void createRoom(Message *message, Message *response, int clientFD) {
	pthread_t game_thread_id;
	game_room game_room_info;
	userInfo user_info;
	char *save_ptr;

	user_info.number = atoi(strtok_r(message->data, DELIM, &save_ptr));
	user_info.FD = clientFD;
	user_info.money = 0;
	user_info.rest_turn = 0;
	string title = strtok_r(NULL, DELIM, &save_ptr);

	for(int n = 1; n < MAX_ROOM; n++)	{
		if(!room_number[n]) {
				game_room_info.roomID = n;
				room_number[n] = true;
				break;
		}
	}

	// initialize game room info
	game_room_info.status = WAIT;
	game_room_info.turn = 0;
	game_room_info.userList.clear();
	game_room_info.userList.push_back(user_info);
	game_room_info.roomLeader = user_info.number;
	game_room_info.userCount = game_room_info.userList.size();
	strcpy(game_room_info.title, title.c_str());

	// initialize queue
	while(!game_room_info.messageQueue.empty())
		game_room_info.messageQueue.pop();

	pthread_mutex_init(&mutex_lock, NULL);
	pthread_mutex_lock(&mutex_lock);
	// make room in shared memory
	sharedMemory.roomList.push_back(game_room_info);
	pthread_mutex_unlock(&mutex_lock);

	printf("room created. room size : %d\n", sharedMemory.roomList.size());

	// create game thread
	pthread_create(&game_thread_id, NULL, game_thread, (void *)&game_room_info);

	// make response contains room id
	strcpy(response->data, to_string(game_room_info.roomID).c_str());
}

#endif
