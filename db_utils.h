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

int CreateTestEntry(MYSQL * conn, int id, int num);

int CreateNewSpot(int spot_id, int region, int distance, int * corners);

int TestDB();
