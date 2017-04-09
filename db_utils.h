//-----------------------------------------------------------------------------
// File:                db_util.h
// Author:              Garrett Bernichon
// Function:            Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------

#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <mysql/mysql.h>
#include "common.h"

// To make these functions usable in C++
#ifdef __cplusplus
extern "C" {
#endif

// Admin
#define K_MYSQL_USER    "root"
#define K_MYSQL_PASS    "1111light"

// Database
#define K_DB    "GridPark"

// Table names
#define K_TBL_OPEN_PARKING   "open_parking"
#define K_TBL_PARKED_CARS    "parked_cars"
#define K_TBL_SUSP_ACTIVITY  "susp_activity"

// Table columns
#define K_TBL_OPEN_PARKING_COLUMNS   7
#define K_TBL_PARKED_CARS_COLUMNS    6
#define K_TBL_SUSP_ACTIVITY_COLUMNS  3

// Global Strings
#define K_QUERY_STRING_LENGTH   128

// Experimenting
#define K_TEST_DB "testdb"


#define K_LOCK_INDEX                2
#define K_LOCK_SLEEP_MICROSECONDS   10000 // 10 milliseconds



// MYSQL admin
MYSQL * OpenDB();
void CloseDB(MYSQL * conn);

// Table insertion
void FormatInsertForOpenParking(char * s, char * table, int spot_id,
        int region, int distance, int corner0, int corner1, int corner2,
        int corner3);

void FormatInsertForParkedCars(char * s, char * table, int car_id,
        int susp_activity, int corner0, int corner1, int corner2, int corner3);

void FormatInsertForSuspActivity(char * s, char * table, int car_id,
        int time_of_detect, int length_of_activity);

int InsertEntry(MYSQL * conn, char * query);

// Table clearing
int ClearTable(MYSQL * conn, char * table);

// Table locking
void WaitForLock(MYSQL * conn, char * table);
int UnlockTable(MYSQL * conn, char * table);
int TableIsLocked(MYSQL * conn, char * table);
int LockTableForRead(MYSQL * conn, char * table);

// Table reading
OPEN_SPOT_T * GetOpenSpots(MYSQL * conn, char * table);
PARKED_CAR_T * GetParkedCars(MYSQL * conn, char * table);
SUSP_ACTIVITY_T * GetSuspActivity(MYSQL * conn, char * table);

// Experimenting
int CreateTestEntry(MYSQL * conn, int id, int num);
int CreateNewSpot(int spot_id, int region, int distance, int * corners);
int TestDB();

// Clos extern
#ifdef __cplusplus
};
#endif

#endif
