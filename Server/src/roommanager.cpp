#ifndef __ROOMMANAGER_CPP__
#define __ROOMMANAGER_CPP__

#include "mainserver.h"
#include "roommanager.h"
#include <iostream>
#include <string.h>
#include <list>
using namespace std;

extern shared_memory sharedMemory;

void listRoom(Message *message, Message *response)
{
    string room = to_string(sharedMemory.roomList.size()) + " ";

    list<game_room>::iterator it;
    for (it = sharedMemory.roomList.begin(); it != sharedMemory.roomList.end(); ++it){
         room += (" " + to_string(it->roomID) + " " + to_string(it->status) + " " + to_string(it->userCount));
    }

    strcpy(response->data, room.c_str());
}

void enterRoom(Message *message, Message *response)
{
    // user id는 message를 통해 알 수 있는데, user fd를 알아야함
}

void exitRoom(Message *message, Message *response)
{

}

void enterAlertRoom(Message *message, Message *response)
{

}

void exitAlertRoom(Message *message, Message *response)
{

}

void startRoom(Message *message, Message *response)
{

}

#endif
