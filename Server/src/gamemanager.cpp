#ifndef __GAMEMANAGER_CPP__
#define __GAMEMANAGER_CPP__

#include <stdlib.h>
#include <string.h>
#include <list>
#include <queue>
#include <pthread.h>
#include "gamemanager.h"
<<<<<<< HEAD
#include "gamedata.h"
=======
#include "game_data.h"
#include "protocol.h"
>>>>>>> origin/master
using namespace std;

extern shared_memory sharedMemory;

void common_message(Message *message, char *roomID_str, char *user[], char *turn, char *save_ptr) {
  roomID_str = strtok_r(message->data, DELIM, &save_ptr);
  for(int i = 0; i < 4; i++)
    user[i] = strtok_r(NULL, DELIM, &save_ptr);
  turn = strtok_r(NULL, DELIM, &save_ptr);
}

<<<<<<< HEAD
void diceRoll(Message *message)
{
  char *user[MAX_USER], *player;
=======
void diceRoll(Message *message) {
  //주사위 던진다
  char *roomID_str, user[4], *turn;
>>>>>>> origin/master
  char *save_ptr;
  char *diceNum_str;
  int dicNum;
  common_message(message, roomID_str, user, turn, save_ptr);
  diceNum_str = strtok_r(NULL, DELIM, &save_ptr);
  dicNum = atoi(diceNum_str);
}

void turn(Message *message) {
  char *roomID_str, user[4], *turn;
  char *save_ptr;
  int roomID;
  common_message(roomID_str, message, user, turn, save_ptr);
  roomID = atoi(roomID_str);
  game_room *current_game;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }

  	//current->turn =
  	int userCount; //Number of users
  	int roomLeader; //방장
  	list<userInfo> userList;		// user list
  	queue<Message> messageQueue;

}

void move(Message *message) {
  //주사위 기반으로 움직인다
  //황금열쇠에서 움직여야 할 때
}

void buy(Message *message)
{
  //돈 체크
  //
}

void pay(Message *message)
{
  //돈 없을 때
}

void goldKey(Message *message)
{
  //rand()
}

void isolation(Message *message)
{

}

#endif
