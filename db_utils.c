//-----------------------------------------------------------------------------
// File: db_utils.c
// Author: Garrett Bernichon
// Function: Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

#include "db_utils.h"
#include "common.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

void WaitForLockForWrite(MYSQL * conn, char * table)
{
    while(TableIsLocked(conn, table))
    {
        usleep(K_LOCK_SLEEP_MICROSECONDS);
    }
    LockTableForWrite(conn, table);
}

int LockTableForRead(MYSQL * conn, char * table)
{
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "LOCK TABLES %s READ", table);

    if (mysql_query(conn, query)) 
    {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}

int LockTableForWrite(MYSQL * conn, char * table)
{
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "LOCK TABLES %s WRITE", table);

    if (mysql_query(conn, query)) 
    {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}

int UnlockTable(MYSQL * conn, char * table)
{
    char query[K_QUERY_STRING_LENGTH] = {0};
    sprintf(query, "UNLOCK TABLES");

    if (mysql_query(conn, query))
    {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    return 0;
}

int TableIsLocked(MYSQL * conn, char * table)
{
    int i = 0;
    MYSQL_ROW row = {0};
    int num_fields = 0;
    int result_int = 0;
    MYSQL_RES * result = NULL;
    char query[K_QUERY_STRING_LENGTH] = {0};
    char result_char[8] = {0};

    sprintf(query, "SHOW OPEN TABLES LIKE \"%s\"", table);
    printf("here1\n");

    if (mysql_query(conn, query)) 
    {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    printf("here2\n");

    result = mysql_store_result(conn);
    if (result == NULL) 
    {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    printf("here3\n");

    num_fields = mysql_num_fields(result);

    while ((row = mysql_fetch_row(result))) 
    { 
        /*for(i = 0; i < num_fields; i++) 
        { 
          printf("%s %d ", row[i] ? row[K_LOCK_INDEX] : "NULL", i); 
        } 
        printf("\n");*/ 
        sprintf(result_char, "%s", row[K_LOCK_INDEX]);
        break;
    }

    if(!strcmp(result_char, (const char*)"0"))
        result_int = 0;
    else
        result_int = 1;

    mysql_free_result(result);
    printf("here4 result %d\n", result_int);

    return result_int;
}
////////////////////////////////////////////////////////////////////////////////

// Read Tables /////////////////////////////////////////////////////////////////
OPEN_SPOT_T * GetOpenSpots(MYSQL * conn, char * table)
{
    OPEN_SPOT_T * head = NULL;
    int i;
    MYSQL_ROW row;
    int num_fields;
    MYSQL_RES * result;
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "SELECT * FROM %s", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    result = mysql_store_result(conn);
    if (result == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    num_fields = mysql_num_fields(result);

    while ((row = mysql_fetch_row(result))) 
    { 
        head = InsertOpenSpot(head, atoi(row[0]), atoi(row[1]), atoi(row[2]), atoi(row[3]),
                                atoi(row[4]), atoi(row[5]), atoi(row[6]));
    }

    mysql_free_result(result);

    return head;
}

PARKED_CAR_T * GetParkedCars(MYSQL * conn, char * table)
{
    PARKED_CAR_T * head = NULL;
    int i;
    MYSQL_ROW row;
    int num_fields;
    MYSQL_RES * result;
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "SELECT * FROM %s", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    result = mysql_store_result(conn);
    if (result == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    num_fields = mysql_num_fields(result);

    while ((row = mysql_fetch_row(result))) 
    { 
        head = InsertParkedCar(head, atoi(row[0]), atoi(row[1]), atoi(row[2]), atoi(row[3]),
                                atoi(row[4]), atoi(row[5]));
    }

    mysql_free_result(result);

    return head;
}

SUSP_ACTIVITY_T * GetSuspActivity(MYSQL * conn, char * table)
{
    SUSP_ACTIVITY_T * head = NULL;
    int i;
    MYSQL_ROW row;
    int num_fields;
    MYSQL_RES * result;
    char query[K_QUERY_STRING_LENGTH];
    sprintf(query, "SELECT * FROM %s", table);

    if (mysql_query(conn, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    result = mysql_store_result(conn);
    if (result == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    num_fields = mysql_num_fields(result);

    while ((row = mysql_fetch_row(result))) 
    { 
        head = InsertSuspActivity(head, atoi(row[0]), atoi(row[1]), atoi(row[2]));
    }

    mysql_free_result(result);

    return head;
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

#ifdef __cplusplus
};
#endif
