//-----------------------------------------------------------------------------
// File:                common.h
// Author:              Garrett Bernichon
// Function:            Global structs
//-----------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

typedef struct spotnode
{
    int spot_id;
    int region;
    int distance;
    int corner0;
    int corner1;
    int corner2;
    int corner3;
    struct spotnode * next;
} OPEN_SPOT_T;

typedef struct carnode
{
    int car_id;
    int susp_activity;
    int distance;
    int corner0;
    int corner1;
    int corner2;
    int corner3;
    struct carnode * next;
} PARKED_CAR_T;

typedef struct suspnode
{
    int car_id;
    int time_of_detect;
    int length_of_activity;
    struct suspnode * next;
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
