#include "usermanager.h"

char tempID[] = "gomin";
char tempPW[] = "street";

void login(Message *message, Message *response)
{
    char *ID, *PW;
    char *save_ptr;

    ID = strtok_r(message->data, DELIM, &save_ptr);
    PW = strtok_r(NULL, DELIM, &save_ptr);

    // Login Success
    if( strcmp(ID, tempID) + strcmp(PW, tempPW) == 0 )  {
        strcpy(response->data, "1");//응답으로 유저 인덱스
    }
    else    {   // Fail
        strcpy(response->data, "0");
    }
}

void signup(Message *message, Message *response)
{
    char *ID, *PW;
    char *save_ptr;

    ID = strtok_r(message->data, DELIM, &save_ptr);
    PW = strtok_r(NULL, DELIM, &save_ptr);

    // ID Check on DB
    // make user on DB when ID is not created

    if( strcmp(ID, tempID) == 0 )  {
        strcpy(response->data, "0");
    }
    else    {
        strcpy(response->data, "1");  // 응답으로 생성된 유저 인덱스
    }
}

void record(Message *message, Message *response)
{
    
}

void win(Message *message, Message *response)
{

}

void lose(Message *message, Message *response)
{

}
