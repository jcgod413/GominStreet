#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "protocol.h"
#include "gamedata.h"

const int gold_key_num = 10;//개수 미정

void diceRoll(Message *, Message *);
void turn(Message *, Message *);
void move(Message *, game_room *, int);
void buy(Message *, Message *);
void pay(Message *);
void goldKey(Message *, Message *);
void goldKeyManager(int);
void isolation(Message *);

#endif
