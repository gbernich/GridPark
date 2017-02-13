//-----------------------------------------------------------------------------
// File:                common.h
// Author:              Garrett Bernichon
// Function:            Global structs
//-----------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

typedef struct
{
    int spot_id;
    int region;
    int distance;
    int corner0;
    int corner1;
    int corner2;
    int corner3;

} OPEN_SPOT_T;

typedef struct
{
    int car_id;
    int susp_activity;
    int distance;
    int corner0;
    int corner1;
    int corner2;
    int corner3;
    
} PARKED_CAR_T;

typedef struct
{
    int car_id;
    int time_of_detect;
    int length_of_activity;
    
} SUSP_ACTIVITY_T;


// OPEN_SPOT_T
OPEN_SPOT_T * CreateOpenSpot(int a, int b, int c, int d, int e, int f, int g);
void DeleteOpenSpot(OPEN_SPOT_T * spot);
void DeleteOpenSpots(OPEN_SPOT_T * head);
OPEN_SPOT_T * InsertOpenSpot(OPEN_SPOT_T * head, int a, int b, int c, int d, int e, int f, int g);
void PrintOpenSpots(OPEN_SPOT_T * head);

// PARKED_CAR_T




// SUSP_ACTIVITY_T




#endif
