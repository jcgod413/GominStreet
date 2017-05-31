#ifndef __ROOMMANAGER_CPP__
#define __ROOMMANAGER_CPP__

#include "mainserver.h"
#include "roommanager.h"
#include "gamedata.h"
#include "protocol.h"
#include "gamemanager.h"
#include <iostream>
#include <string.h>
#include <list>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

extern shared_memory sharedMemory;
extern bool room_number[MAX_ROOM];
const int init_money = 1000;

void listRoom(Message *message, Message *response, int clientFD) {
    string room = to_string(sharedMemory.roomList.size());
    strcpy(response->data, room.c_str());
    write(clientFD, response, PACKET_SIZE);
    printf("room size : %s\n", response->data);

    list<game_room>::iterator it;
    for (it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it){
        memset(response->data, 0, sizeof(response->data));
        room.clear();
        room = to_string(it->roomID) + " " + to_string(it->status) + " " + to_string(it->userCount) + " " + it->title;
        strcpy(response->data, room.c_str());
        write(clientFD, response, PACKET_SIZE);
        printf("room info : %s\n", response->data);
    }

    memset(response->data, 0, sizeof(response->data));
}

void enterRoom(Message *message, Message *response, int clientFD) {
  char *save_ptr;
  char *roomID_str = strtok_r(message->data, DELIM, &save_ptr);
  char *user_idx = strtok_r(NULL, DELIM, &save_ptr);
  int roomID = atoi(roomID_str);
  game_room *current_game = NULL;
  userInfo user_info;

  user_info.number = atoi(user_idx);
  user_info.FD = clientFD;
  user_info.money = 0;
	user_info.rest_turn = 0;

  // find game room in shared memory
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }

  // if the room is full, send response 0
  if( current_game->userCount >= MAX_USER ) {
    strcpy(response->data, "0");
    return;
  }

  current_game->userCount++;
  current_game->userList.push_back(user_info);

  // success response
  strcpy(response->data, "1");

  enterAlertRoom(current_game, user_info.number);
}

void exitRoom(Message *message, Message *response) {
  char *save_ptr;

  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));

  int exit_user = atoi(strtok_r(NULL, DELIM, &save_ptr));

  game_room *current_game = NULL;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }

    current_game->userCount--;

    bool is_roomLeader = false;
    if(current_game->roomLeader == exit_user)
      is_roomLeader = true;
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      if(it2->number == exit_user) {
        current_game->userList.erase(it2);
        break;
      }

    if(is_roomLeader)
      current_game->roomLeader = current_game->userList.begin()->number;

    exitAlertRoom(current_game, exit_user);
}

void enterAlertRoom(game_room *current_game, int userID) {
  Message response;
  strcpy(response.identifier, "GOMIN");
  response.category[Major] = Major_Room;
  response.category[Minor] = Room_Alert_Enter;

  strcpy(response.data, to_string(userID).c_str());
  printf("<%d번 방에 user%d이 입장>\n", current_game->roomID, userID);
  for(int i = 0; i < current_game->userCount; i++) {
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)  {
      write(it2->FD, (char *)&response, PACKET_SIZE);
    }
  }
}

void exitAlertRoom(game_room *current_game, int userID) {
  Message response;
  strcpy(response.identifier, "GOMIN");
  response.category[Major] = Major_Room;
  response.category[Minor] = Room_Alert_Exit;

  strcpy(response.data, to_string(userID).c_str());
  printf("<%d번 방에 user%d이 퇴장>\n", current_game->roomID, userID);
  for(int i = 0; i < current_game->userCount; i++) {
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)  {
      write(it2->FD, (char *)&response, PACKET_SIZE);
    }
  }
}

int cost[31] = {0, 200, 180, 170, 200, 200, 200, 200,
                200, 200, 200, 200, 200, 200, 200, 200,
                200, 200, 200, 200, 200, 200, 200, 200,
                200, 200, 200, 200, 200, 200, 200};

void startRoom(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));

  game_room *current_game = NULL;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }

  // 인원수 체크, 2인 미만시 플레이 불가
  if(current_game->userCount < 2) {
    strcpy(response->data, 0);
    return;
  }

  // game_room 상태 변경
	current_game->status = PLAY;
  current_game->turn = 0;

  for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2) {
    it2->money = init_money;
    it2->rest_turn = 0;
  }

  //음식점 초기화
  for(int i = 1; i <= 30; i++) {
    current_game->restaurant_info[i].money = cost[i];
    current_game->restaurant_info[i].owner = 0;
  	current_game->restaurant_info[i].storeCount = 0;
  }

  // 모든 유저에게 게임 시작을 알림
  strcpy(response->data, "1");
  for(int i = 0; i < current_game->userCount; i++)
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      write(it2->FD, (char *)response, PACKET_SIZE);

  turn(current_game);
}

#endif
