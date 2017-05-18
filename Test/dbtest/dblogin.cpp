// comiple  :  g++ dbtest.cpp -l sqlite3

#include <stdio.h>
#include <sqlite3.h>
#include <iostream>

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i=0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL") ;
   }
   printf("\n") ;
   return 0;
}

int main(int argc, char* argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *ID = "gomin";
    char *PW = "street";

    rc = sqlite3_open("./gomin.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n" , sqlite3_errmsg(db));
        return -1;
    }
    else{
        fprintf(stderr, "Opened database successfully\n");
    }

    string sql = "SELECT * FROM USER WHERE ID='" + string(ID) + "' AND PW='" + string(PW) + "'";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )    {
            fprintf(stderr, "SQL error: %s\n" , zErrMsg);
            sqlite3_free(zErrMsg) ;
    }
    else{
        fprintf(stdout, "Select operation executed successfully\n");
    }

    sqlite3_close(db);
    return 0;
}