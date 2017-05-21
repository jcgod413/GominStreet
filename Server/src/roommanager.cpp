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
using namespace std;

extern shared_memory sharedMemory;
extern bool room_number[MAX_ROOM];
extern pthread_mutex_t mutex_lock;

void listRoom(Message *message, Message *response) {
    string room = to_string(sharedMemory.roomList.size()) + " ";

    list<game_room>::iterator it;
    for (it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it){
         room += (" " + to_string(it->roomID) + " " + to_string(it->status) + " " + to_string(it->userCount));
    }

    strcpy(response->data, room.c_str());
}

void enterRoom(Message *message, Message *response) {
  char *save_ptr;
  char *roomID_str = strtok_r(message->data, DELIM, &save_ptr);
  char *user_idx = strtok_r(NULL, DELIM, &save_ptr);
	char *FD = strtok_r(NULL, DELIM, &save_ptr);
  int roomID = atoi(roomID_str);
  userInfo user_info;

  user_info.number = atoi(user_idx);
  user_info.FD = atoi(FD);

  game_room *current_game;
  for (list<game_room>::iterator it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it)
    if(it->roomID == roomID) {
      current_game = &*it;
      break;
    }

    if(current_game->userCount >= 4) {
      strcpy(response->data, "0");
      return;
    }

    current_game->userCount++;
  	current_game->userList.push_back(user_info);
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

    for(list<userInfo>::iterator it2 = current_game->userList.begin(); it2 != current_game->userList.end(); ++it2)
      if(it2->number == exit_user) {
        current_game->userList.erase(it2);
        return;
      }

}

void enterAlertRoom(Message *message, Message *response) {
  /*
  char *save_ptr;
  int roomID = atoi(strtok_r(message->data, DELIM, &save_ptr));
  int exit_user = atoi(strtok_r(NULL, DELIM, &save_ptr));
  */
}

void exitAlertRoom(Message *message, Message *response) {

}

void startRoom(Message *message, Message *response) {

}

#endif
