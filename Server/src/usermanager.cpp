#ifndef __USERMANAGER_CPP__
#define __USERMANAGER_CPP__

#include <string.h>
#include <list>
#include <queue>
#include <pthread.h>
#include <sqlite3.h>
#include <iostream>
//#include "protocol.h"

#include "usermanager.h"
using namespace std;

enum DB_USER  {
    USER_NUM,
    ID,
    PW,
    WIN,
    LOSE,
};

struct USER_INFO    {
    char num[20];
    char id[20];
    char pw[20];
    char win[20];
    char lose[20];
};
USER_INFO userInfo;
bool isUserUpdated = false;

sqlite3 *db;
char *zErrMsg = 0;
int rc;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
    strcpy(userInfo.num, argv[USER_NUM]);
    strcpy(userInfo.id, argv[ID]);
    strcpy(userInfo.pw, argv[PW]);
    strcpy(userInfo.win, argv[WIN]);
    strcpy(userInfo.lose, argv[LOSE]);
    
    isUserUpdated = true;

    return 0;
}

void initDB()
{
    rc = sqlite3_open("database/gomin.db", &db);
    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n" , sqlite3_errmsg(db));
    }
    else{
        fprintf(stderr, "Opened database successfully\n");
    }
}

bool query(string sql)
{
    isUserUpdated = false;
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if( rc == SQLITE_OK )    {
        return isUserUpdated;
    }
    else    {
        fprintf(stderr, "SQL error: %s\n" , zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
}

void login(Message *message, Message *response)
{
<<<<<<< Updated upstream
    char *ID, *PW;
    char *save_ptr;
=======
    if( rc == 0 )   {   initDB(); }

    char *save_ptr;
    char *ID = strtok_r(message->data, DELIM, &save_ptr);
    char *PW = strtok_r(NULL, DELIM, &save_ptr);

>>>>>>> Stashed changes

    string sql = "SELECT * FROM USER WHERE ID='" + string(ID) + "' AND PW='" + string(PW) + "'";

    // Login Success
    if( query(sql) )    {
        printf("logined user num : %s\n", userInfo.num);
        strcpy(response->data, userInfo.num);    //응답으로 유저 인덱스
    }
    else    {   // Fail
        strcpy(response->data, "0");
    }
}

void signup(Message *message, Message *response)
{
<<<<<<< Updated upstream
    char *ID, *PW;
    char *save_ptr;
=======
    if( rc == 0 )   {   initDB(); }

    char *save_ptr;
    char *ID = strtok_r(message->data, DELIM, &save_ptr);
    char *PW = strtok_r(NULL, DELIM, &save_ptr);
>>>>>>> Stashed changes

    string sql = "SELECT * FROM USER WHERE ID='" + string(ID) + "'";

    // ID Check on DB
    // make user on DB when ID is not created
    if( query(sql) )  {
        strcpy(response->data, "0");
    }
    else    {   // No User in DB
        sql = "INSERT INTO USER(ID,PW,WIN,LOSE) VALUES('" + string(ID) + "', '" + string(PW) + "', 0, 0)";
        query(sql);

        sql = "SELECT * FROM USER WHERE ID='" + string(ID) + "'";
        query(sql);

        printf("logined user num : %s\n", userInfo.num);
        strcpy(response->data, userInfo.num);    //응답으로 유저 인덱스
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

#endif
