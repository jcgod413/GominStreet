#ifndef __MAINSERVER_H__
#define __MAINSERVER_H__

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <queue>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "protocol.h"
#include "usermanager.h"
#include "roommanager.h"
#include "gamemanager.h"
#include "gamedata.h"

#define PORT 20001

using namespace std;

void createRoom(Message *);
bool validityCheck(Message *);
void sendResponse(int, Message *);
void userManager(Message *, Message *);
void roomManager(Message *);
void gameManager(Message *);
void *communication_thread(void *);
void *game_thread(void *);
void createRoom(Message *);

struct thread_param{
	int client_fd;
};


// extern shared_memory sharedMemory;

#endif
