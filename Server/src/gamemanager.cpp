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
  //pthread_mutex_lock(&mutex_lock);
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }
  //pthread_mutex_unlock(&mutex_lock);

  //식중독 고려

  move(response, current_game, dice_number);
}

void turn(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game;

  //pthread_mutex_lock(&mutex_lock);
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }
  //pthread_mutex_unlock(&mutex_lock);

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

  string next_turn = to_string(current_game->turn);

    strcpy(response->data, next_turn.c_str());
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      write(it2->FD, response, PACKET_SIZE);
    //식중독 고려
}

void move(Message *response, game_room *current_game, int move_number) {
  //turn에 해당하는 클라이언트 말을 move_number 만큼 이동하라고 모든 클라이언트에게 알려주기
  string move_info = to_string(current_game->turn);
  move_info = move_info + " " + to_string(move_number);

  strcpy(response->data, move_info.c_str());
  for(list<userInfo>::iterator it = current_game->userList.begin(); it != current_game->userList.end(); ++it)
    write(it->FD, response, PACKET_SIZE);
}

void buy(Message *message) {
  //현재 남은 금액 체크
  //돈이 부족할 경우 -> 0
  //돈이 충분할 경우
  //호점수
}

void pay(Message *message)
{
  //돈이 부족할 경우
    //소유 음식점이 있을 경우 가격에 맞는 가장 싼거부터 팔아버리고 알림
    //소유 음식점도 없이 없는 경우 파산=> 패배
  //돈이 충분할 경우
}

void goldKey(Message *message)
{
  //황금열쇠 랜덤으로 결정하여 알려주기
  //rand()
}

void isolation(Message *message)
{
}

//돈을 증가시키는 프로토콜 => 황금열쇠
//게임 종료 프로토콜 => 파산 안 당한 1명만 남았을 때

#endif
