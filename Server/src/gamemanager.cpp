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

  if( current_user->rest_turn > 0 )  // ISOLATION, OUT 인 경우
    res = "0 " + to_string(current_user->rest_turn);
  else  {
    res = to_string(current_game->turn) + " " + to_string(dice_number);
    move(current_game, dice_number);
  }

  strcpy(response->data, res.c_str());
  sendAllUser(current_game, response);
}

// 한 턴을 증가시켜준 후 모든 유저에게 해당정보를 알림
void turn(game_room *current_game) {
  Message response;
  messageSetting(&response, Major_Game, Game_Turn);  

  // turn은 1~플레이어 수, 4명이면 1~4
  current_game->turn = (current_game->turn % current_game->userList.size());
  userInfo *current_user = findCurrentUser(current_game, current_game->turn);
  // 움직일 수 없는 유저를 통과시키기 위한 코드
  while( current_user->rest_turn == OUT ) {
    current_game->turn = (current_game->turn % current_game->userList.size());
    current_game->turn++;
    if( current_game->turn > current_game->userList.size() )
      current_game->turn = 1;
    current_user = findCurrentUser(current_game, current_game->turn);
  }
  printf("[Turn %d번방 %d번유저 user%d 차례\n", current_game->roomID, current_game->turn, current_user->number);

  strcpy(response.data, to_string(current_game->turn).c_str());
  sendAllUser(current_game, &response);

  current_game->turn++;
  if( current_game->turn > current_game->userList.size() )
    current_game->turn = 1;
}

//move_number = 주사위 숫자 or 황금열쇠 이동 숫자
void move(game_room *current_game, int move_number) {
  Message response;
  messageSetting(&response, Major_Game, Game_Move);

  string res = to_string(current_game->turn) + " " + to_string(move_number);
  strcpy(response.data, res.c_str());
  sendAllUser(current_game, &response);
}

void buy(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int restaurant_number = atoi(strtok_r(NULL, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int current_turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, current_turn);

  // 돈이 없는 경우
  if(current_user->money < current_game->restaurant_info[restaurant_number].money) {
    strcpy(response->data, "0");
    sendAllUser(current_game, response);
    turn(current_game);
    return;
  }

  // 점포의 개수가 3개 이상이거나, 다른 주인인 경우
  if(current_game->restaurant_info[restaurant_number].storeCount >= 3
    || (current_game->restaurant_info[restaurant_number].owner > 0 
    && current_game->restaurant_info[restaurant_number].owner != current_turn)) {
    strcpy(response->data, "0");
    sendAllUser(current_game, response);
    turn(current_game);
    return;
  }

  current_game->restaurant_info[restaurant_number].owner = current_turn;
  current_game->restaurant_info[restaurant_number].storeCount++;
  current_user->money -= current_game->restaurant_info[restaurant_number].money;
  strcpy(response->data, to_string(restaurant_number).c_str());
  sendAllUser(current_game, response);

  turn(current_game);
}

void pay(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int current_turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, current_turn);
  int target = atoi(strtok_r(NULL, DELIM, &save_ptr));
  userInfo *target_user;
  if( target == 0 ) {// system
    target_user = (userInfo *) malloc(sizeof(userInfo));
    target_user->number = 0;
  	target_user->FD = 0;
  	target_user->money = 0;
  	target_user->rest_turn = 0;
  }
  else  {
    target_user = findCurrentUser(current_game, target);
  }
  int money = atoi(strtok_r(NULL, DELIM, &save_ptr));
  string res;
  //돈이 부족할 경우
  //소유 음식점이 있을 경우 팔아버리고 알림   // iterator로 하나씩 돌면서 자기 보유 금액이 목표금액보다 크거나 같을때까지 땅을 판매
  if( current_user->money < money ) {
    // sell restaurant
    bool has_restaurant[RESTAURANT_NUM];
    memset(has_restaurant, 0, sizeof(has_restaurant));

    int cnt = findRestaurantOwner(current_game, current_user, has_restaurant);

    int restaurant_number = 0;

    while(current_user->money < money && cnt > 0) {
      for(int i = restaurant_number + 1; i < RESTAURANT_NUM; i++) {
        if(has_restaurant[i]) {
          restaurant_number = i;
          has_restaurant[restaurant_number] = false;
          cnt--;
          break;
        }
      }

      sellRestaurant(current_game, current_user, restaurant_number, true);
    }

    // 땅을 다 판매해도 목표금액보다 작은경우 파산처리.
    if( current_user->money < money ) {
      current_user->rest_turn = OUT;
      //turn이 target에게 지불해야 할 money가 없어서 파산하였습니다.
      res = "0 " + to_string(current_turn) + " " + to_string(target) + " " + to_string(money);
      strcpy(response->data, res.c_str());
      sendAllUser(current_game, response);
      turn(current_game);
      return;
    }
  }

  // 돈이 충분할 경우
  current_user->money -= money;
  target_user->money += money;
  res = "1 " + to_string(current_turn) + " " + to_string(target) + " " + to_string(money);
  strcpy(response->data, res.c_str());
  sendAllUser(current_game, response);

  if(target == 0) {
    free(target_user);
    target_user = NULL;
  }
  
  turn(current_game);
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

void sellRestaurant(game_room *current_game, userInfo *current_user, int restaurant_number, bool flag) {
  Message response;
  messageSetting(&response, Major_Game, Game_Sell_Restaurant);
  string res;
  if(restaurant_number == 0) {
    res = "0 " + to_string(current_game->roomID) + " " + to_string(current_game->turn);
    strcpy(response.data, res.c_str());
    sendAllUser(current_game, &response);
    return;
  }

  int owner = current_game->restaurant_info[restaurant_number].owner;
  res = "1 " + to_string(current_game->roomID) + " " + to_string(owner) + " " + to_string(restaurant_number);
  if(flag) {//판매인 경우 true, 강제 철거(세무조사)인 경우 false
    int money = current_game->restaurant_info[restaurant_number].money;
    int storeCount = current_game->restaurant_info[restaurant_number].storeCount;
    //owner가 소유한 restaurant_number가 판매되었습니다.
    res += " 1";
    current_user->money +=  money * storeCount;
  }
  else//owner가 소유한 restaurant_number가 철거되었습니다.
    res += " 0";
  strcpy(response.data, res.c_str());
  sendAllUser(current_game, &response);

  current_game->restaurant_info[restaurant_number].owner = 0;
  current_game->restaurant_info[restaurant_number].storeCount = 0;
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

  goldKeyManager(current_game, key_number);

  turn(current_game);
}

void goldKeyManager(game_room *current_game, int key_number) {
  Message message, response;
  string data;
  int current_turn, cnt, restaurant_number;
  bool has_restaurant[RESTAURANT_NUM];
  userInfo *current_user;
  switch (key_number) {
    case 1:// 폐점
      // 건물 하나 찾아서 없애버리기
      current_turn = current_game->turn;
      current_user = findCurrentUser(current_game, current_turn);
      memset(has_restaurant, 0, sizeof(has_restaurant));
      cnt = findRestaurantOwner(current_game, current_user, has_restaurant);
      restaurant_number = 0;
      if(cnt > 0)
        for(int i = 1; i < RESTAURANT_NUM; i++) {
          if(has_restaurant[i]) {
            restaurant_number = i;
            break;
          }
        }

      sellRestaurant(current_game, current_user, restaurant_number, false);
      break;
    case 2:// 앞으로 이동
      move(current_game, 3);
      break;
    case 3: // 뒤로 이동
      move(current_game, -3);
      break;
    case 4: // 고민사거리 발전기금 내기
      messageSetting(&message, Major_Game, Game_Pay);
      data = to_string(current_game->roomID) + " " + "0 500";
      strcpy(message.data, data.c_str());

      messageSetting(&response, Major_Game, Game_Pay);
      pay(&message, &response);
      break;
    case 5:// 착한식당 선정
      messageSetting(&message, Major_Game, Game_Pay);
      data = to_string(current_game->roomID) + " " + to_string(current_game->turn) + " 500";
      strcpy(message.data, data.c_str());

      messageSetting(&response, Major_Game, Game_Pay);
      pay(&message, &response);
      break;
  }
}

void messageSetting(Message *message, char c0, char c1) {
  strcpy(message->identifier, "GOMIN");
  sprintf(message->category, "%c%c", c0, c1);
}

void isolation(Message *message) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int current_turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, current_turn);

  current_user->rest_turn = ISOLATION;
  
  turn(current_game);
}

void salary(Message *message) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  game_room *current_game = findCurrentGame(roomID);
  int current_turn = current_game->turn;
  userInfo *current_user = findCurrentUser(current_game, current_turn);

  current_user->money += SALARY;
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
    if( it->rest_turn <= ISOLATION )  {
      write(it->FD, response, PACKET_SIZE);
    }
  }
}

#endif
