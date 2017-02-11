//-----------------------------------------------------------------------------
// File:                db_utils.c
// Author:              Garrett Bernichon
// Function:            Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include "db_utils.h"

MYSQL * OpenDB(char * dbName)
{
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    if (mysql_real_connect(conn, "localhost", K_MYSQL_USER, K_MYSQL_PASS, 
      dbName, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

void CloseDB(MYSQL * conn)
{
    mysql_close(conn);
}

int CreateTestEntry(MYSQL * conn, int id, int num)
{
    char query[80];
    sprintf(query, "INSERT INTO test VALUES(%d,%d)", id, num);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}

int CreateNewSpot(int spot_id, int region, int distance, int * corners)
{
    return 0;
}

int TestDB()
{
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }

    if (mysql_real_connect(conn, "localhost", K_MYSQL_USER, K_MYSQL_PASS, 
      NULL, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }  

    if (mysql_query(conn, "CREATE DATABASE testdb")) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    mysql_close(conn);

    return 0;
}
