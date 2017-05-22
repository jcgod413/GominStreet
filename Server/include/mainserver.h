#ifndef __MAINSERVER_H__
#define __MAINSERVER_H__

#include "protocol.h"
#define PORT 20001
#define MAX_ROOM 1024

struct thread_param{
	int client_fd;
};

bool validityCheck(Message *);
void sendResponse(int, Message *);
void userManager(Message *, Message *);
void roomManager(Message *, Message *, int);
void gameManager(Message *);
void *communication_thread(void *);
void *game_thread(void *);
void createRoom(Message *, Message *, int);

#endif
