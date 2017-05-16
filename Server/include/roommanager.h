#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

#include <string.h>
#include "protocol.h"

void listRoom(Message *);
void enterRoom(Message *);
void exitRoom(Message *);
void enterAlertRoom(Message *);
void exitAlertRoom(Message *);
void startRoom(Message *);

#endif
