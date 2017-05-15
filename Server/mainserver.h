#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <queue>
#include "protocol.h"

#define PORT	20000

typedef struct game_room{
	struct page_num *next;
	struct page_num *prev;
	int player1;
	int player2;
	int player3;
	int player4;
	int status;
	pthread_t room_id;
	//queue
	char name[30];
} game_room;

struct game_room *play_list = NULL;
struct game_room *wait_list = NULL;

typedef struct thread_param{
	int client_fd;
}thread_param;
