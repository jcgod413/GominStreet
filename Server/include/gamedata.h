#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <pthread.h>
#include <sys/types.h>
#include "protocol.h"
using namespace std;

#define MAX_USER 4

#define ISOLATION 2	// 2턴 쉬고 3번째부터 턴받기
#define OUT 9

#define SALARY 40

#define RESTAURANT_NUM 40

const int cycle_money = 50;	//한 바퀴 돌 때마다 제공되는 금액
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
	int rest_turn;	// 3이면 무인도, 9이면 파산 또는 나간 플레이어
	int position;
};

struct game_room {
	int roomID;	// room id(thread num)
	int status;			// game status (wait, play)
	char title[40];		// room title
	int turn;			// current turn user index
	int userCount; //Number of users
	int roomLeader; //방장
	list<userInfo> userList;		// user list
	restaurantInfo restaurant_info[RESTAURANT_NUM];	// index는 1부터
	queue<Message> messageQueue;	// message queue (raw message)
};

struct shared_memory	{
	list<game_room> roomList;
};

#endif
