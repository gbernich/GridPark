//-----------------------------------------------------------------------------
// File:                db_utils.c
// Author:              Garrett Bernichon
// Function:            Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------

#include <mysql.h>

int CreateNewSpot(int spot_id, int region, int distance, int * corners)
{
    return 0;
}

int TestDB()
{
    MYSQL *con = mysql_init(NULL);

    if (con == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "root", "root_pswd", 
      NULL, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }  

    if (mysql_query(con, "CREATE DATABASE testdb")) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    mysql_close(con);
    exit(0);

    return 0;
}