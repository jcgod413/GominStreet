#ifndef __MAINSERVER_H__
#define __MAINSERVER_H__

#include "protocol.h"
#define PORT 20000
#define MAX_ROOM 80

struct thread_param{
	int client_fd;
};

bool validityCheck(Message *);
void sendResponse(int, Message *);
void userManager(Message *, Message *, int);
void roomManager(Message *, Message *, int);
void gameManager(Message *, Message *);
void *communication_thread(void *);
void *game_thread(void *);
void createRoom(Message *, Message *, int);
void deleteRoom(int);

#endif
