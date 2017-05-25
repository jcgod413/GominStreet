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

const int cycle_money = 50;//한 바퀴 돌 때마다 제공되는 금액
enum game_status {WAIT, PLAY};

struct restaurantInfo {
	int money;
	int owner;
	int storeCount;
};

struct userInfo	{
	int number;		// user index
	int FD;			// file descriptor
	int money;
	int rest_turn;
};

struct game_room {
	int roomID;	// room id(thread num)
	int status;			// game status (wait, play)
	char title[40];		// room title
	int turn;			// current turn user index
	int userCount; //Number of users
	int roomLeader; //방장
	list<userInfo> userList;		// user list
	restaurantInfo restaurant_info[30];
	queue<Message> messageQueue;	// message queue (raw message)
};

struct shared_memory	{
	list<game_room> roomList;
};

#endif
