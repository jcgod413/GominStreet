#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include <iostream>
#include <list>
#include <queue>
#include <pthread.h>
#include <sys/types.h>
#include "protocol.h"
using namespace std;

#define MAX_USER 4

enum game_status {WAIT, PLAY};

struct userInfo	{
	int number;		// user index
	int FD;			// file descriptor
};



struct game_room {
	int roomID;	// room id(thread num)
	int status;			// game status (wait, play)
	char title[40];		// room title
	int turn;			// current turn user index
	int userCount; //Number of users
	int roomLeader; //방장
	list<userInfo> userList;		// user list
	queue<Message> messageQueue;	// message queue (raw message)
};

struct shared_memory	{
	list<game_room> roomList;
};

#endif
