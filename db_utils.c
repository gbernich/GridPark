//-----------------------------------------------------------------------------
// File:                db_utils.c
// Author:              Garrett Bernichon
// Function:            Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <mysql/mysql.h>
//#include <my_global.h>

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
        return 1;
    }

    if (mysql_real_connect(con, "localhost", "root", "1111light", 
      NULL, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }  

    if (mysql_query(con, "CREATE DATABASE testdb")) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    mysql_close(con);

    return 0;
}
