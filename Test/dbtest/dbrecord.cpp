// comiple  :  g++ dbtest.cpp -l sqlite3

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

int win, lose;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i=0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL") ;
   }

   win = atoi(argv[3]);
   lose = atoi(argv[4]);

   printf("win : %d   lose : %d\n", win, lose);
   return 0;
}

void win()
{
    
}

void lose()
{

}

int main(int argc, char* argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open("./gomin.db", &db);

    if( rc ) {
      fprintf(stderr, "Can't open database: %s\n" , sqlite3_errmsg(db));
      return -1;
   }
   else{
      fprintf(stderr, "Opened database successfully\n");
   }

   sql = "SELECT * FROM USER";
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
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