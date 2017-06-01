#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

// #include "gamemanager.h"
#include "protocol.h"
#include "gamedata.h"

void listRoom(Message *, Message *, int);
void enterRoom(Message *, int);
void exitRoom(Message *, Message *);
void enterAlertRoom(game_room *, int);
void exitAlertRoom(game_room *, int);
void startRoom(Message *, Message *);
void userDisconnected(int, int);
#endif
