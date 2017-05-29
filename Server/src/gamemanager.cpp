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
  int dice_number = rand() % 12 + 1;
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  userInfo current_user = findCurrentUser(current_game, current_game->turn);
  string res;

  if( current_user->rest_turn > 0 )  // ISOLATION, OUT 인 경우
    res = to_string(current_game->turn) + " " + to_string(current_user->rest_turn);
  else  {
    res = to_string(current_game->turn) + " " + to_string(dice_number);
    move(current_game, dice_number);
  }

  strcpy(response->data, res.c_str());
  sendAllUser(current_game, response);
}

// 한 턴을 증가시켜준 후 모든 유저에게 해당정보를 알림
void turn(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);

  // turn은 1~플레이어 수, 4명이면 1~4
  current_game->turn = ((current_game->turn + 1) % current_game->userList.size()) + 1;
  userInfo *current_user = findCurrentUser(current_game, current_game->turn);
  // 움직일 수 없는 유저를 통과시키기 위한 코드
  // 무인도에 걸리지 않은 유저는 그냥 통과
  while( current_user->rest_turn == OUT ) {  
    current_game->turn = ((current_game->turn + 1) % current_game->userList.size()) + 1;
    current_user = findCurrentUser(current_game, current_game->turn);
  }

  strcpy(response->data, to_string(current_game->turn).c_str());
  sendAllUser(current_game, response);
}

void move(game_room *current_game, int move_number) {
  Message response;
  strcpy(response.identifier, "GOMIN");
  response.category[Major] = Major_Room;
  response.category[Minor] = Room_Alert_Exit;
  
  string res = to_string(current_game->turn) + " " + to_string(move_number);
  strcpy(response.data, res.c_str());
  sendAllUser(current_game, response);
}

void buy(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int restaurant_number = atoi(strtok_r(NULL, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, turn);

  // 돈이 없는 경우
  if(current_user->money < current_game->restaurant_info[restaurant_number].money) {
    strcpy(response->data, "0");
    sendAllUser(current_game, response);
    return;
  }

  // 점포의 개수가 3개 이상이거나, 다른 주인인경우
  if(current_game->restaurant_info[restaurant_number].storeCount >= 3
    || (current_game->restaurant_info[restaurant_number].owner > 0 && current_game->restaurant_info[restaurant_number].owner != turn)) {  
    strcpy(response->data, "0");
    sendAllUser(current_game, response);
    return;
  }

  current_game->restaurant_info[restaurant_number].owner = turn;
  current_game->restaurant_info[restaurant_number].storeCount++;
  current_user->money -= current_game->restaurant_info[restaurant_number].money;
  strcpy(response->data, to_string(restaurant_number).c_str());
  sendAllUser(current_game, response);
}

void pay(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, turn);
  int target = atoi(strtok_r(NULL, DELIM, &save_ptr));
  userInfo *target_user;
  if( target == 0 ) {// system 
    // shared memory에 시스템 유저 가리키기(무한 돈을 가지고 있는)
  } 
  else  {
    target_user = findCurrentUser(current_game, target);  
  }
  int money = atoi(strtok_r(NULL, DELIM, &save_ptr));

  //돈이 부족할 경우
  //소유 음식점이 있을 경우 팔아버리고 알림   // iterator로 하나씩 돌면서 자기 보유 금액이 목표금액보다 크거나 같을때까지 땅을 판매
  if( current_user->money < money ) {
    // sell restaurant

    // 땅을 다 판매해도 목표금액보다 작은경우 파산처리. 
    if( current_user->money < money ) {
      // 파산
      // (rest_turn = 9)
      return;
    }
  }
  
  // 돈이 충분할 경우
  current_user->money -= money;
  target_user->money += money;
  string res = "1 " + to_string(turn) + " " + to_string(target) + " " + to_string(money);
  strcpy(response->data, res.c_str());
}

void goldKey(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);

  srand(time(NULL));
  int key_number = rand() % gold_key_num + 1;
  string key_value = to_string(key_number);

  strcpy(response->data, key_value.c_str());
  sendAllUser(current_game, response);

  goldKeyManager(key_number);
}

void goldKeyManager(int key_number) {
  // 1. 폐점
  // 2. 앞 이동 
  // 3. 뒤 이동 
  // 4. 고민사거리 발전기금 내기
  // 5. 착한식당 선정
  switch (key_number) {
    case 1: 
      // 건물 하나 찾아서 없애버리기  (건물팔기 함수 재사용)
      break;
    case 2:
      // move 호출
      break;
    case 3:
      // move 호출
      break;
    case 4:
      // pay 호출
      break;
    case 5:
      // pay 호출
      break;
  }
}

void isolation(Message *message) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, turn);
  
  current_user->rest_turn = ISOLATION;
}

void salary(Message *message) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, turn);

  current_user->money += SALARY;
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
  int turnCnt = 0;
  //pthread_mutex_lock(&mutex_lock);
  for(list<userInfo>::iterator it = current_game->userList.begin(); it != current_game->userList.end(); ++it) {
    turnCnt++;
    if(turnCnt == turn) {
      current_user = &*it;
      break;
    }
  }
  //pthread_mutex_unlock(&mutex_lock);
  return current_user;
}

void sendAllUser(game_room *current_game, Message *response) {
  for(list<userInfo>::iterator it = current_game->userList.begin(); it != current_game->userList.end(); ++it) {
    if( it->rest_turn <= ISOLATION )  {
      write(it->FD, response, PACKET_SIZE);
    }
  }
}

#endif
