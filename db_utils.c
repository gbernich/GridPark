//-----------------------------------------------------------------------------
// File:                db_utils.c
// Author:              Garrett Bernichon
// Function:            Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "db_utils.h"
#include "constants.h"

// MYSQL admin /////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

// Table insertion /////////////////////////////////////////////////////////////
void FormatInsertForOpenParking(char * s, char * table, int spot_id,
        int region, int distance, int corner0, int corner1, int corner2,
        int corner3)
{
    sprintf(s, "INSERT INTO %s VALUES(%d,%d,%d,%d,%d,%d,%d)", table, spot_id,
        region, distance, corner0, corner1, corner2, corner3);
}

void FormatInsertForParkedCars(char * s, char * table, int car_id,
        int susp_activity, int corner0, int corner1, int corner2, int corner3)
{
    sprintf(s, "INSERT INTO %s VALUES(%d,%d,%d,%d,%d,%d)", table, car_id,
        susp_activity, corner0, corner1, corner2, corner3);
}

void FormatInsertForSuspActivity(char * s, char * table, int car_id,
        int time_of_detect, int length_of_activity)
{
    sprintf(s, "INSERT INTO %s VALUES(%d,%d,%d)", table, car_id,
        time_of_detect, length_of_activity);
}

int InsertEntry(MYSQL * conn, char * query)
{
    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////

// Table clearing //////////////////////////////////////////////////////////////
int ClearTable(MYSQL * conn, char * table)
{
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "TRUNCATE %s", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////

// Table locking //////////////////////////////////////////////////////////////
void WaitForLock(MYSQL * conn, char * table)
{
    while(TableIsLocked(conn, table))
    {
        usleep(K_LOCK_SLEEP_MICROSECONDS);
    }
    LockTableForRead(conn, table);
}

int LockTableForRead(MYSQL * conn, char * table)
{
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "LOCK TABLES %s READ", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}

int UnlockTable(MYSQL * conn, char * table)
{
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "UNLOCK TABLES", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}

int TableIsLocked(MYSQL * conn, char * table)
{
    MYSQL_ROW row;
    int num_fields;
    MYSQL_RES * result;
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "SHOW OPEN TABLES LIKE \"%s\"", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) 
    {
        finish_with_error(conn);
    }

    num_fields = mysql_num_fields(result);    

    while ((row = mysql_fetch_row(result))) 
    { 
        for(int i = 0; i < num_fields; i++) 
        { 
            printf("%s ", row[i] ? row[i] : "NULL"); 
        } 
        printf("\n"); 
    }

    return row[2];
}

////////////////////////////////////////////////////////////////////////////////

// Experimenting ///////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
