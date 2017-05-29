#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "protocol.h"
#include "gamedata.h"

const int gold_key_num = 5;


void diceRoll(Message *, Message *);
void turn(Message *, Message *);
void move(Message *, int);
void buy(Message *, Message *);
void pay(Message *, Message *);
void goldKey(Message *, Message *);
void goldKeyManager(int);
void isolation(Message *);
void salary(Message *);
game_room *findCurrentGame(int roomID);
userInfo *findCurrentUser(game_room *, int userID);
void sendAllUser(game_room *, Message *);
#endif
