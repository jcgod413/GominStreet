#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "protocol.h"
#include "gamedata.h"

const int gold_key_num = 5;


void diceRoll(Message *, Message *);
void turn(game_room *);
void nextTurn(game_room *);
void move(game_room *, int);
void buy(Message *, Message *);
void pay(Message *, Message *);
int findRestaurantOwner(game_room *, userInfo *, bool *);
void sellRestaurant(game_room *, userInfo *, int, bool);
void goldKey(Message *, Message *);
void goldKeyManager(game_room *, int);
void messageSetting(Message *, char, char);
void isolation(Message *);
void salary(Message *);
game_room *findCurrentGame(int roomID);
userInfo *findCurrentUser(game_room *, int userID);
void sendAllUser(game_room *, Message *);
#endif
