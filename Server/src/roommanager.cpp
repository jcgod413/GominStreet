#ifndef __ROOMMANAGER_CPP__
#define __ROOMMANAGER_CPP__

#include "mainserver.h"
#include "roommanager.h"
#include "gamedata.h"
#include "protocol.h"
#include <iostream>
#include <string.h>
#include <list>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

extern shared_memory sharedMemory;
extern bool room_number[MAX_ROOM];
extern pthread_mutex_t mutex_lock;

void listRoom(Message *message, Message *response) {
    string room = to_string(sharedMemory.roomList.size());

    list<game_room>::iterator it;
    for (it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it){
         room += (" " + to_string(it->roomID) + " " + to_string(it->status) + " " + to_string(it->userCount));
    }

    strcpy(response->data, room.c_str());
}

void enterRoom(Message *message, Message *response, int clientFD) {
  char *save_ptr;
  char *roomID_str = strtok_r(message->data, DELIM, &save_ptr);
  char *user_idx = strtok_r(NULL, DELIM, &save_ptr);
  int roomID = atoi(roomID_str);
  game_room *current_game;
  userInfo user_info;

  user_info.number = atoi(user_idx);
  user_info.FD = clientFD;

  // find game room in shared memory
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it) {
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  }

  // if the room is full, send response 0
  if(current_game->userCount >= 4) {
    strcpy(response->data, "0");
    return;
  }

  current_game->userCount++;
  current_game->userList.push_back(user_info);
  
  // success response
  strcpy(response->data, "1");
}

void exitRoom(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int exit_user = atoi(strtok_r(NULL, DELIM, &save_ptr));

  game_room *current_game;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }

    current_game->userCount--;

    //모두 나가면 게임 스레드 종료
		if(current_game->userCount == 0) {
			room_number[current_game->roomID] = false;
			pthread_mutex_unlock(&mutex_lock);
			pthread_exit((void *)0);
      return;
		}
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
}

void enterAlertRoom(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  char *enter_user_str = strtok_r(NULL, DELIM, &save_ptr);
  int enter_user = atoi(enter_user_str);
  int user[4];
  user[0] = atoi(strtok_r(NULL, DELIM, &save_ptr));

  for(int i = 1; i < 4; i++)
    user[i] = atoi(strtok_r(NULL, DELIM, &save_ptr));

  game_room *current_game;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  strcpy(response->data, enter_user_str);
  for(int i = 0; i < current_game->userCount; i++) {
    if(user[i] == enter_user)
      continue;
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      if(it2->number == user[i])
        write(it2->FD, (char *)response, PACKET_SIZE);
  }
}

void exitAlertRoom(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  char *exit_user_str = strtok_r(NULL, DELIM, &save_ptr);
  int exit_user = atoi(exit_user_str);
  int user[4];
  user[0] = atoi(strtok_r(NULL, DELIM, &save_ptr));

  for(int i = 1; i < 4; i++)
    user[i] = atoi(strtok_r(NULL, DELIM, &save_ptr));
  char *roomLeader = strtok_r(NULL, DELIM, &save_ptr);

  game_room *current_game;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }
  strcpy(response->data, exit_user_str);
  strcat(response->data, " ");
  strcat(response->data, roomLeader);

  for(int i = 0; i < current_game->userCount; i++) {
    if(user[i] == exit_user)
      continue;
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      if(it2->number == user[i])
        write(it2->FD, (char *)response, PACKET_SIZE);
  }
}

void startRoom(Message *message, Message *response) {
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int user[4];
  user[0] = atoi(strtok_r(NULL, DELIM, &save_ptr));
  for(int i = 1; i < 4; i++)
    user[i] = atoi(strtok_r(NULL, DELIM, &save_ptr));

  game_room *current_game;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }

  //인원수 체크
  if(current_game->userCount < 2) {
    strcpy(response->data, 0);
    return;
  }

  //game_room 상태 변경
	int status = PLAY;
	int turn = user[0];

  //모든 유저에게 게임 시작을 알림
  strcpy(response->data, "1");
  for(int i = 0; i < current_game->userCount; i++)
    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      if(it2->number == user[i])
        write(it2->FD, (char *)response, PACKET_SIZE);
}

#endif
