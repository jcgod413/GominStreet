#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "protocol.h"

//void common_message(Message *, char *[], char *, char *);
void diceRoll(Message *, Message *response);
void turn(Message *);
void move(Message *);
void buy(Message *);
void pay(Message *);
void goldKey(Message *);
void isolation(Message *);

#endif
