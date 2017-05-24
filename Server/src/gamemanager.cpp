#ifndef __GAMEMANAGER_CPP__
#define __GAMEMANAGER_CPP__

#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>
#include <queue>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "gamemanager.h"
#include "gamedata.h"
#include "protocol.h"

using namespace std;

extern shared_memory sharedMemory;
extern pthread_mutex_t mutex_lock;

/*
void common_message(Message *message, char *roomID_str, char *user[], char *turn, char *save_ptr) {
  roomID_str = strtok_r(message->data, DELIM, &save_ptr);
  for(int i = 0; i < 4; i++)
    user[i] = strtok_r(NULL, DELIM, &save_ptr);
  turn = strtok_r(NULL, DELIM, &save_ptr);
}
*/

void diceRoll(Message *message, Message *response) {
  srand(time(NULL));
  int dice_number = rand() % 6 + 1;
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));

  game_room *current_game;
  pthread_mutex_lock(&mutex_lock);
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }
  pthread_mutex_unlock(&mutex_lock);

  string room = to_string(current_game->turn);
  room = room + " " + to_string(dice_number);

  strcpy(response->data, room.c_str());
  for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
    write(it2->FD, response, PACKET_SIZE);
  //식중독 고려
}

void turn(Message *message) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game;
  pthread_mutex_lock(&mutex_lock);
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }
  pthread_mutex_unlock(&mutex_lock);

  int nth = 0;
  bool current_turn = false;
  for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2) {
    if(!current_turn)
      nth++;
    else {
      current_game->turn = it2->number;
      break;
    }

    if(current_game->turn == it2->number)
      current_turn = true;
  }

  if(nth == current_game->userCount)
    current_game->turn = current_game->userList.begin()->number;

    //식중독 고려
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
