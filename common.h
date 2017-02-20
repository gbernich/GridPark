//-----------------------------------------------------------------------------
// File:                common.h
// Author:              Garrett Bernichon
// Function:            Global structs
//-----------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#define K_PACKET_OPEN_PARKING   0
#define K_PACKET_PARKED_CARS    1
#define K_PACKET_SUSP_ACTIVITY  2

#define K_PORT_OFFSET           5000
#define K_PORT_OPEN_PARKING     5000
#define K_PORT_PARKED_CARS      5001
#define K_PORT_SUSP_ACTIVITY    5002

#define K_MASK_0 0x000000FF // LSB
#define K_MASK_1 0x0000FF00 // LSB
#define K_MASK_2 0x00FF0000 // LSB
#define K_MASK_3 0xFF000000 // LSB


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
PARKED_CAR_T * CreateParkedCar(int a, int b, int c, int d, int e, int f);
void DeleteParkedCar(PARKED_CAR_T * car);
void DeleteParkedCars(PARKED_CAR_T * head);
PARKED_CAR_T * InsertParkedCar(PARKED_CAR_T * head, int a, int b, int c, int d, int e, int f);
void PrintParkedCars(PARKED_CAR_T * head);

// SUSP_ACTIVITY_T
SUSP_ACTIVITY_T * CreateSuspActivity(int a, int b, int c);
void DeleteSuspActivity(SUSP_ACTIVITY_T * act);
void DeleteSuspActivities(SUSP_ACTIVITY_T * head);
SUSP_ACTIVITY_T * InsertSuspActivity(SUSP_ACTIVITY_T * head, int a, int b, int c);
void PrintSuspActivities(SUSP_ACTIVITY_T * head);

// Packet
void PackIntoPacket(char * buff, int offset, int size, int value);

#endif
