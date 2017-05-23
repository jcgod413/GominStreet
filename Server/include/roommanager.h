#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

// #include "gamemanager.h"
#include "protocol.h"
#include "gamedata.h"

void listRoom(Message *, Message *, int);
void enterRoom(Message *, Message *, int);
void exitRoom(Message *, Message *);
void enterAlertRoom(Message *, Message *);
void exitAlertRoom(Message *, Message *);
void startRoom(Message *, Message *);

#endif
