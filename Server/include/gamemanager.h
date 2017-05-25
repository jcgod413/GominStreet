#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "protocol.h"

void diceRoll(Message *, Message *);
void turn(Message *, Message *);
void move(Message *);
void buy(Message *);
void pay(Message *);
void goldKey(Message *);
void isolation(Message *);

#endif
