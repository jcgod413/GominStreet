#include "gamemanager.h"



void common_message(Message *message, char *user[], char *player, char *save_ptr) {
  user[0] = strtok_r(message->data, DELIM, &save_ptr);
  for(int i = 1; i < 4; i++)
    user[i] = strtok_r(NULL, DELIM, &save_ptr);
  player = strtok_r(NULL, DELIM, &save_ptr);

}

void diceRoll(Message *message)
{
  char *user[4], *player;
  char *save_ptr;
  common_message(message, user, player, save_ptr);

  /*

  // Login Success
  if( strcmp(ID, tempID) + strcmp(PW, tempPW) == 0 )  {
      strcpy(response->data, "1");//응답으로 유저 인덱스
  }
  else    {   // Fail
      strcpy(response->data, "0");
  }
  */
}

void turn(Message *message)
{

}

void move(Message *message)
{

}

void buy(Message *message)
{

}

void pay(Message *message)
{

}

void goldKey(Message *message)
{

}

void isolation(Message *message)
{

}
