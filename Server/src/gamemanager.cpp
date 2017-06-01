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

//게임 종료 프로토콜 => 파산 안 당한 1명만 남았을 때
// mutex 추가

void diceRoll(Message *message, Message *response) {
  srand(time(NULL));
  //주사위 번호 1 ~ 12
  int dice_number = rand() % 12 + 1;
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  userInfo *current_user = findCurrentUser(current_game, current_game->turn);
  string res;

  if( current_user->rest_turn > 0 ) { // ISOLATION, OUT 인 경우
    res = "0 " + to_string(current_user->rest_turn);
    strcpy(response->data, res.c_str());
    sendAllUser(current_game, response);
  }
  else  {
    res = to_string(current_game->turn) + " " + to_string(dice_number);
    strcpy(response->data, res.c_str());
    sendAllUser(current_game, response);
    move(current_game, dice_number);
  }
}

// 한 턴을 증가시켜준 후 모든 유저에게 해당정보를 알림
void turn(game_room *current_game) {
  Message response;
  messageSetting(&response, Major_Game, Game_Turn);

  printf("[Turn] %d번방 %d번유저 차례\n", current_game->roomID, current_game->turn);

  strcpy(response.data, to_string(current_game->turn).c_str());
  sendAllUser(current_game, &response);
}

void nextTurn(game_room *current_game)  {
  current_game->turn++;
  if( current_game->turn > (int) current_game->userList.size() )
    current_game->turn = 1;

  // turn은 1~플레이어 수, 4명이면 1~4
  userInfo *current_user = findCurrentUser(current_game, current_game->turn);
  // 움직일 수 없는 유저를 통과시키기 위한 코드
  while( current_user->rest_turn == OUT ) {
    current_game->turn = (current_game->turn % current_game->userList.size());
    current_user = findCurrentUser(current_game, current_game->turn);
  }

  turn(current_game);
}

//move_number = 주사위 숫자 or 황금열쇠 이동 숫자
void move(game_room *current_game, int move_number) {
  Message response;
  messageSetting(&response, Major_Game, Game_Move);

  string res = to_string(current_game->turn) + " " + to_string(move_number);
  strcpy(response.data, res.c_str());
  sendAllUser(current_game, &response);

  userInfo *current_user = findCurrentUser(current_game, current_game->turn);
  current_user->position += move_number;
  if( current_user->position >= RESTAURANT_NUM )  {
    current_user->position -= RESTAURANT_NUM;
    salary(current_game, current_user);
  }
}

void buy_check(game_room *current_game, userInfo *current_user) {
  Message response;
  messageSetting(&response, Major_Game, Game_Buy_Check);
  int user_pos = current_user->position;

  //출발점인 경우
  if(user_pos == 0)
    return;

  //호점 수가 3개인 경우
  if(current_game->restaurant_info[user_pos].storeCount >= 3)
    return;

  // 돈이 없는 경우
  if(current_user->money < current_game->restaurant_info[user_pos].money)
    return;

  memset(response.data, 0, sizeof(response.data));
  write(current_user->FD, &response, PACKET_SIZE);
}

void buy(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int buy_flag = atoi(strtok_r(NULL, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int current_turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, current_turn);
  int user_pos = current_user->position;
  if(buy_flag == 0) {
    nextTurn(current_game);
    return;
  }

  current_game->restaurant_info[user_pos].owner = current_turn;
  current_game->restaurant_info[user_pos].storeCount++;
  current_user->money -= current_game->restaurant_info[user_pos].money;
  strcpy(response->data, "1");
  sendAllUser(current_game, response);

  nextTurn(current_game);
}

void pay(game_room *current_game, userInfo *give_user, userInfo *target_user, bool flag) {

  Message response;
  messageSetting(&response, Major_Game, Game_Pay);

  int pay_money;
  if(flag) {
    int pos = give_user->position;
    pay_money = give_user->restaurant_info[give_user_pos].money / 10;
  }
  else
    pay_money = GOLDKEY_MONEY;

  if( give_user->money < pay_money ) {
    sellRestaurant(current_game, give_user, money, true);

    // 땅을 다 판매해도 목표금액보다 작은경우 파산처리.
    if( give_user->money < money ) {
      give_user->rest_turn = OUT;
      // turn이 target에게 지불해야 할 money가 없어서 파산하였습니다.
      res = "0 " + to_string(current_game->turn) + " " + to_string(target) + " " + to_string(money);
      strcpy(response.data, res.c_str());
      sendAllUser(current_game, &response);
      return;
    }
  }

  give_user->money -= money;
  target_user->money += money;

  res = "1 " + to_string(current_game->turn) + " " + to_string(target) + " " + to_string(money);
  strcpy(response.data, res.c_str());
  sendAllUser(current_game, &response);

  string res;
  //돈이 부족할 경우
  //소유 음식점이 있을 경우 팔아버리고 알림   // iterator로 하나씩 돌면서 자기 보유 금액이 목표금액보다 크거나 같을때까지 땅을 판매
  if( give_user->money < money ) {
    sellRestaurant(current_game, give_user, money, true);

    // 땅을 다 판매해도 목표금액보다 작은경우 파산처리.
    if( give_user->money < money ) {
      give_user->rest_turn = OUT;
      // turn이 target에게 지불해야 할 money가 없어서 파산하였습니다.
      res = "0 " + to_string(current_game->turn) + " " + to_string(target) + " " + to_string(money);
      strcpy(response.data, res.c_str());
      sendAllUser(current_game, &response);
      return;
    }
  }

  // 돈이 충분할 경우
  give_user->money -= money;
  target_user->money += money;
  res = "1 " + to_string(current_game->turn) + " " + to_string(target) + " " + to_string(money);
  strcpy(response.data, res.c_str());
  sendAllUser(current_game, &response);

}

int findRestaurantOwner(game_room *current_game, userInfo *current_user, bool *has_restaurant) {
  int cnt = 0;
  for(int i = 0; i < RESTAURANT_NUM; i++) {
    if(current_user->number == current_game->restaurant_info[i].owner) {
      has_restaurant[i] = true;
      cnt++;
    }
  }

  return cnt;
}

void sellRestaurant(game_room *current_game, userInfo *current_user, int money, bool flag) {
  Message response;
  messageSetting(&response, Major_Game, Game_Sell_Restaurant);

  string res;
  bool has_restaurant[RESTAURANT_NUM];
  int restaurant_number = 0;
  int cnt = findRestaurantOwner(current_game, current_user, has_restaurant);
  memset(has_restaurant, 0, sizeof(has_restaurant));
  if(flag) {//다른 유저의 음식점을 방문했는데 돈이 부족해서 본인의 건물을 판매하는 경우
    res = "1 " + to_string(current_game->roomID) + " " + to_string(current_game->turn) + " 1";
    while(current_user->money < money && cnt > 0)
      for(int i = restaurant_number + 1; i < RESTAURANT_NUM; i++)
        if(has_restaurant[i]) {
          restaurant_number = i;
          has_restaurant[restaurant_number] = false;
          cnt--;

          int money = current_game->restaurant_info[restaurant_number].money;
          int storeCount = current_game->restaurant_info[restaurant_number].storeCount;

          current_user->money +=  money * storeCount;

          current_game->restaurant_info[restaurant_number].owner = 0;
          current_game->restaurant_info[restaurant_number].storeCount = 0;

          memset(response.data, 0, sizeof(response.data));
          res += " " + to_string(restaurant_number);
          strcpy(response.data, res.c_str());
          sendAllUser(current_game, &response);
          break;
        }
  }
  else {//황금열쇠에서 세무조사로 인해 철거하는 경우
    if(cnt > 0)
      for(int i = 1; i < RESTAURANT_NUM; i++) {
        if(has_restaurant[i]) {
          restaurant_number = i;
          break;
        }
      }

      if(restaurant_number == 0) {
          res = "0 " + to_string(current_game->roomID) + " " + to_string(current_game->turn);
          strcpy(response.data, res.c_str());
          sendAllUser(current_game, &response);
          return;
      }

      res = "1 " + to_string(current_game->roomID) + " " + to_string(current_game->turn) + " 0";
      current_game->restaurant_info[restaurant_number].owner = 0;
      current_game->restaurant_info[restaurant_number].storeCount = 0;

      memset(response.data, 0, sizeof(response.data));
      res += " " + to_string(restaurant_number);
      strcpy(response.data, res.c_str());
      sendAllUser(current_game, &response);
  }
}

void goldKey(game_room *current_game, userInfo *current_user) {
  Message response;
  messageSetting(&response, Major_Game, Game_GoldKey);
  srand(time(NULL));
  int key_number = rand() % gold_key_num + 1;
  string res = to_string(key_number);
  strcpy(response.data, res.c_str());

  sendAllUser(current_game, &response);
  goldKeyManager(current_game, current_user, key_number);
}

userInfo * system_user_init() {
  userInfo * system_user = (userInfo *) malloc(sizeof(userInfo));
  system_user->number = 0;
  system_user->FD = 0;
  system_user->money = 1000000;
  system_user->rest_turn = 0;

  return system_user;
}

void goldKeyManager(game_room *current_game, userInfo *current_user, int key_number) {
  userInfo *system_user = NULL;

  switch (key_number) {
    case 1:// 폐점
      // 건물 하나 찾아서 없애버리기
      sellRestaurant(current_game, current_user, 0, false);
      break;
    case 2:// 앞으로 이동
      move(current_game, 3);
      break;
    case 3: // 한 바퀴 돌기
      move(current_game, RESTAURANT_NUM);
      break;
    case 4: // 고민사거리 발전기금 내기
      system_user = system_user_init();
      pay(current_game, current_user, system_user, 0);
      free(system_user);
      system_user = NULL;
      break;
    case 5:// 착한식당 선정
      system_user = system_user_init();
      pay(current_game, system_user, current_user, 0);
      free(system_user);
      system_user = NULL;
      break;
  }
}

void messageSetting(Message *message, char c0, char c1) {
  strcpy(message->identifier, "GOMIN");
  sprintf(message->category, "%c%c", c0, c1);
}

void isolation(game_room *current_game, userInfo *current_user) {
  Message response;
  messageSetting(&response, Major_Game, Game_Isolation);
  strcpy(response.data, to_string(current_game->turn).c_str());

  current_user->rest_turn = ISOLATION;

  sendAllUser(current_game, &response);
}

void salary(game_room *current_game, userInfo *current_user) {
  Message response;
  messageSetting(&response, Major_Game, Game_Salary);
  strcpy(response.data, to_string(current_game->turn).c_str());

  current_user->money += SALARY;

  sendAllUser(current_game, &response);
}

//돈을 증가시키는 프로토콜 => 황금열쇠
//게임 종료 프로토콜 => 파산 안 당한 1명만 남았을 때
game_room *findCurrentGame(int roomID)  {
  game_room *current_game = NULL;
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
  userInfo *current_user = NULL;
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
    // 파산하거나 나간 유저 제외하고 전송
    if( it->rest_turn <= ISOLATION )  {
      printf("\t\t\tSend : Major %c, Minor %c, FD : %d\n", response->category[Major], response->category[Minor], it->number);
      write(it->FD, response, PACKET_SIZE);
    }
  }
}

void visit(Message *message)  {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int current_turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, current_turn);
  int position = current_user->position;

  switch( position )  {
    case 0:
      nextTurn(current_game);
      break;
    case 10: case 19: case 30:
      isolation(current_game, current_user);
      nextTurn(current_game);
      break;
    case 2: case 7: case 12: case 25: case 32: case 37:
      goldKey(current_game, current_user);
      nextTurn(current_game);
      break;
    default:
      //현재 위치에 소유자가 없는 음식점이거나 본인 소유의 음식점일 경우
      if(current_game->restaurant_info[position].owner == 0
      || current_game->restaurant_info[position].owner == current_turn)
        buy_check(current_game, current_user);
      else {//다른 유저의 음식점을 방문할 경우
        int owner_turn = current_game->restaurant_info[position].owner;
        userInfo *owner = findCurrentUser(current_game, owner_turn);
        pay(current_game, current_user, owner, true);
      }
      break;
  }
}

#endif
