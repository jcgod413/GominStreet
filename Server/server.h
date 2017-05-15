#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <queue>

#define DELIM 			"|"
#define CHK_ID 			1
#define CREATE_ID 		2
#define LOGIN 			3
#define RELOAD 			4
#define ACCESS_ROOM 	5
#define CREATE_ROOM 	6

typedef struct game_room{
	struct page_num *next;
	struct page_num *prev;
	int player1;
	int player2;
	int player3;
	int player4;
	int status;
	pthread_t room_id
	//queue
	char name[30];
} game_room;

struct game_room *play_list = NULL;
struct game_room *wait_list = NULL;

typedef struct thread_param{
	int client_fd;
}thread_param;
