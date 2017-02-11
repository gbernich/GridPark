//-----------------------------------------------------------------------------
// File:                db_util.h
// Author:              Garrett Bernichon
// Function:            Provide functions to interact with the mySQL databases.
//-----------------------------------------------------------------------------

#include <mysql/mysql.h>

#define K_MYSQL_USER    "root"
#define K_MYSQL_PASS    "1111light"

MYSQL * OpenDB();
void CloseDB(MYSQL * conn);


// Table insertion
char * FormatInsertForOpenParking(char * s, char * table, int spot_id,
        int region, int distance, int corner0, int corner1, int corner2,
        int corner3);

char * FormatInsertForParkedCars(char * s, char * table, int car_id,
        int susp_activity, int corner0, int corner1, int corner2, int corner3);

char * FormatInsertForSuspActivity((char * s, char * table, int car_id,
        int time_of_detect, int length_of_activity);

int InsertEntry(MYSQL * conn, char * query);

// Table clearing
int ClearTable(MYSQL * conn, char * table);





// Experimenting
int CreateTestEntry(MYSQL * conn, int id, int num);

int CreateNewSpot(int spot_id, int region, int distance, int * corners);

int TestDB();
