#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

// #include "gamemanager.h"
#include "protocol.h"
#include "gamedata.h"

void listRoom(Message *, int);
void enterRoom(Message *, int);
void exitRoom(Message *);
void enterAlertRoom(game_room *);
void exitAlertRoom(game_room *);
void startRoom(Message *, Message *);
void userDisconnected(int, int);
#endif
