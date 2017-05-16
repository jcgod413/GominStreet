#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include <string.h>
#include <list>
#include <queue>
#include <pthread.h>
#include "protocol.h"

using namespace std;

void diceRoll(Message *);
void turn(Message *);
void move(Message *);
void buy(Message *);
void pay(Message *);
void goldKey(Message *);
void isolation(Message *);

#endif