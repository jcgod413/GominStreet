#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include <list>
#include <queue>
#include <pthread.h>
#include "protocol.h"
using namespace std;

struct userInfo	{
	int number;		// user index
	int FD;			// file descriptor
};

struct game_room {
	pthread_t roomID;	// room id(thread num)
	int status;			// game status (wait, play)
	int turn;			// current turn user index

	list<userInfo> userList;		// user list
	queue<Message> messageQueue;	// message queue (row message)
};

struct shared_memory	{
	list<game_room> roomList;
} sharedMemory;

#endif