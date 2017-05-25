#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "protocol.h"
#include "gamedata.h"

void diceRoll(Message *, Message *);
void turn(Message *, Message *);
void move(Message *, game_room *, int);
void buy(Message *);
void pay(Message *);
void goldKey(Message *);
void isolation(Message *);

#endif
