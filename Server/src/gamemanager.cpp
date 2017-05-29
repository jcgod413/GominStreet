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

//식중독을 고려해야 하는 함수 : diceRoll, turn, move, isolation
//mutex

void diceRoll(Message *message, Message *response) {
  srand(time(NULL));
  int dice_number = rand() % 6 + 1;
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  
  //식중독 고려

  move(response, current_game, dice_number);
}

// 한 턴을 증가시켜준 후 모든 유저에게 해당정보를 알림
void turn(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);

  // turn은 1~플레이어 수, 4명이면 1~4
  current_game->turn = ((current_game->turn + 1) % current_game->userList.size()) + 1;
  userInfo *current_user = findCurrentUser(current_game, current_game->turn);
  while( current_user->rest_turn > 0 && current_user->rest_turn <= ISOLATION ) {
    current_user->rest_turn--;
    current_game->turn = ((current_game->turn + 1) % current_game->userList.size()) + 1;
    current_user = findCurrentUser(current_game, current_game->turn);
  }

  strcpy(response->data, to_string(current_game->turn).c_str());
  sendAllUser(current_game, response);
}

void move(Message *response, game_room *current_game, int move_number) {
  //turn에 해당하는 클라이언트 말을 move_number 만큼 이동하라고 모든 클라이언트에게 알려주기
  string move_info = to_string(current_game->turn);
  move_info = move_info + " " + to_string(move_number);

  strcpy(response->data, move_info.c_str());
  for(list<userInfo>::iterator it = current_game->userList.begin(); it != current_game->userList.end(); ++it)
    write(it->FD, response, PACKET_SIZE);
}

void buy(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int restaurant_number = atoi(strtok_r(NULL, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);

  int turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, turn);
  

  if(current_user->money < current_game->restaurant_info[restaurant_number].money) {
    strcpy(response->data, "0");
    write(current_user->FD, response, PACKET_SIZE);
    return;
  }

  if(current_game->restaurant_info[restaurant_number].storeCount >= 3
    || current_game->restaurant_info[restaurant_number].owner != current_user->number) {
    strcpy(response->data, "0");
    write(current_user->FD, response, PACKET_SIZE);
    return;
  }

  if(current_game->restaurant_info[restaurant_number].storeCount == 0)
    current_game->restaurant_info[restaurant_number].owner = current_user->number;
  current_game->restaurant_info[restaurant_number].storeCount++;
  current_user->money -= current_game->restaurant_info[restaurant_number].money;

  strcpy(response->data, "1");
  sendAllUser(current_game, response);
}


void pay(Message *message) {
  //돈이 부족할 경우
    //소유 음식점이 있을 경우 랜덤으로 팔아버리고 알림
    //소유 음식점도 없이 없는 경우 파산=> 패배
  //돈이 충분할 경우
}

void goldKey(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);

  srand(time(NULL));
  int key_number = rand() % gold_key_num + 1;
  string key_value = to_string(key_number);

  strcpy(response->data, key_value.c_str());
  for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
    write(it2->FD, response, PACKET_SIZE);

  goldKeyManager(key_number);
}

void goldKeyManager(int key_number) {
  switch (key_number) {
    case 1:
      break;
  }
}

void isolation(Message *message) {

}

//돈을 증가시키는 프로토콜 => 황금열쇠
//게임 종료 프로토콜 => 파산 안 당한 1명만 남았을 때

game_room *findCurrentGame(int roomID)  {
  game_room *current_game;
  //pthread_mutex_lock(&mutex_lock);
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }
  //pthread_mutex_unlock(&mutex_lock);
  return current_game;
}

userInfo *findCurrentUser(game_room *current_game, int turn)  {
  userInfo *current_user;
  //pthread_mutex_lock(&mutex_lock);
  for(list<userInfo>::iterator it = current_game->userList.begin(); it != current_game->userList.end(); ++it) {
    if(it->number == turn) {
      current_user = &*it;
      break;
    }
  }
  //pthread_mutex_unlock(&mutex_lock);
  return current_user;
}

void sendAllUser(game_room *current_game, Message *response) {
  for(list<userInfo>::iterator it = current_game->userList.begin(); it != current_game->userList.end(); ++it)
    write(it->FD, response, PACKET_SIZE);
}

#endif
