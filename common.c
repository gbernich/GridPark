//-----------------------------------------------------------------------------
// File:                common.c
// Author:              Garrett Bernichon
// Function:            Global structs
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include "common.h"

// OPEN_SPOT_T
OPEN_SPOT_T * CreateOpenSpot(int a, int b, int c, int d, int e, int f, int g)
{
    OPEN_SPOT_T * spot = NULL;
    spot = malloc(sizeof(OPEN_SPOT_T));
    spot -> spot_id = a;
    spot -> region = b;
    spot -> distance = c;
    spot -> corner0 = d;
    spot -> corner1 = e;
    spot -> corner2 = f;
    spot -> corner3 = g;
    spot -> next = NULL;
    return spot;
}

void DeleteOpenSpot(OPEN_SPOT_T * spot)
{
    free(spot);
}

void DeleteOpenSpots(OPEN_SPOT_T * head)
{
    OPEN_SPOT_T * p = NULL;
    OPEN_SPOT_T * q = NULL;

    p = head;
    while(p != NULL)
    {
        q = p -> next;
        DeleteOpenSpot(p);
        p = q;
    }
}

OPEN_SPOT_T * InsertOpenSpot(OPEN_SPOT_T * head, int a, int b, int c, int d, int e, int f, int g)
{
    OPEN_SPOT_T * spot = NULL;
    spot = CreateOpenSpot(a, b, c, d, e, f, g);

    // List head is null, this spot is new head
    if (head == NULL)
    {
        return spot;
    }

    OPEN_SPOT_T * ptr = head;
    while(ptr -> next != NULL)
    {
        ptr = ptr -> next;
    }
    ptr -> next = spot;
    return head;
}

void PrintOpenSpots(OPEN_SPOT_T * head)
{
    OPEN_SPOT_T * spot = head;

    if (head == NULL)
    {
        return;
    }

    do {
        spot = spot -> next;
    } while(spot != NULL);
    printf("\n");
}

// PARKED_CAR_T
PARKED_CAR_T * CreateParkedCar(int a, int b, int c, int d, int e, int f)
{
    PARKED_CAR_T * car = NULL;
    car = malloc(sizeof(PARKED_CAR_T));
    car -> car_id = a;
    car -> susp_activity = b;
    car -> corner0 = c;
    car -> corner1 = d;
    car -> corner2 = e;
    car -> corner3 = f;
    car -> next = NULL;
    return car;
}

void DeleteParkedCar(PARKED_CAR_T * car)
{
    free(car);
}

void DeleteParkedCars(PARKED_CAR_T * head)
{
    PARKED_CAR_T * p = NULL;
    PARKED_CAR_T * q = NULL;
    
    p = head;
    while(p != NULL)
    {
        q = p -> next;
        DeleteParkedCar(p);
        p = q;
    }
}

PARKED_CAR_T * InsertParkedCar(PARKED_CAR_T * head, int a, int b, int c, int d, int e, int f)
{
    PARKED_CAR_T * car = NULL;
    car = CreateParkedCar(a, b, c, d, e, f);

    // List head is null, this spot is new head
    if (head == NULL)
    {
        return car;
    }

    PARKED_CAR_T * ptr = head;
    while(ptr -> next != NULL)
    {
        ptr = ptr -> next;
    }
    ptr -> next = car;
    return head;
}

void PrintParkedCars(PARKED_CAR_T * head)
{
    PARKED_CAR_T * car = head;

    if (head == NULL)
    {
        return;
    }

    do {
        car = car -> next;
    } while(car != NULL);
    printf("\n");
}


// SUSP_ACTIVITY_T
SUSP_ACTIVITY_T * CreateSuspActivity(int a, int b, int c)
{
    SUSP_ACTIVITY_T * act = NULL;
    act = malloc(sizeof(SUSP_ACTIVITY_T));
    act -> car_id = a;
    act -> time_of_detect = b;
    act -> length_of_activity = c;
    act -> next = NULL;
    return act;
}

void DeleteSuspActivity(SUSP_ACTIVITY_T * act)
{
    free(act);
}

void DeleteSuspActivities(SUSP_ACTIVITY_T * head)
{
    SUSP_ACTIVITY_T * p = NULL;
    SUSP_ACTIVITY_T * q = NULL;
    
    p = head;
    while(p != NULL)
    {
        q = p -> next;
        DeleteSuspActivity(p);
        p = q;
    }
}

SUSP_ACTIVITY_T * InsertSuspActivity(SUSP_ACTIVITY_T * head, int a, int b, int c)
{
    SUSP_ACTIVITY_T * act = NULL;
    act = CreateSuspActivity(a, b, c);

    // List head is null, this spot is new head
    if (head == NULL)
    {
        return act;
    }

    SUSP_ACTIVITY_T * ptr = head;
    while(ptr -> next != NULL)
    {
        ptr = ptr -> next;
    }
    ptr -> next = act;
    return head;
}

void PrintSuspActivities(SUSP_ACTIVITY_T * head)
{
    SUSP_ACTIVITY_T * act = head;

    if (head == NULL)
    {
        return;
    }

    do {
        act = act -> next;
    } while(act != NULL);
    printf("\n");
}

// Packet
void PackIntoPacket(char * buff, int offset, int size, int value)
{
    buff[offset++] = (char)(value & K_MASK_0);
    buff[offset++] = (char)((value & K_MASK_1) >> 8);
    buff[offset++] = (char)((value & K_MASK_2) >> 16);
    buff[offset++] = (char)((value & K_MASK_3) >> 24);
}

void GetSocketArgs(int * args, char * buff, int offset, int packet_type)
{
    int i = 0;
    int ind = 0;
    int max_ind = 0;

    if (packet_type == K_PACKET_OPEN_PARKING)
    {
        max_ind = 7;
    }
    else if (packet_type == K_PACKET_PARKED_CARS)
    {
        max_ind = 6;
    }
    else // K_PACKET_SUSP_ACTIVITY
    {
        max_ind = 3;
    }


    for(i = 0; i < max_ind; i++)
    {
       ind = offset + i*4;
       args[i] =  buff[ind] + (buff[ind+1] << 8) + (buff[ind+2] << 16) + (buff[ind+3] << 24);
    }
}

// JSON /////////////////
void WriteOpenParkingJSONHeader(FILE * f)
{
    fprintf(f, "{\n\t\"Parking_Data\": {\n");
    fprintf(f, "\t\t\"Spots\": [\n");
}
void WriteOpenParkingJSONEntry(FILE * f, int * args)
{
    fprintf(f, "\t\t\t{\n"); // open entry

    fprintf(f, "\t\t\t\"index\": \"%d\",\n", args[0]);
    fprintf(f, "\t\t\t\"x\": \"%d\",\n", args[1]);
    fprintf(f, "\t\t\t\"y\": \"%d\",\n", args[2]);
    fprintf(f, "\t\t\t\"c0\": \"%d\",\n", args[3]);
    fprintf(f, "\t\t\t\"c1\": \"%d\",\n", args[4]);
    fprintf(f, "\t\t\t\"c2\": \"%d\",\n", args[5]);
    fprintf(f, "\t\t\t\"c3\": \"%d\"\n", args[6]);

    fprintf(f, "\t\t\t}"); // close entry
}

void WriteOpenParkingJSONComma(FILE * f)
{
    fprintf(f, ",\n");
}

void WriteOpenParkingJSONFooter(FILE * f)
{
    fprintf(f, "\n\t\t]\n");
    fprintf(f, "\t}\n");
    fprintf(f, "}\n");
}
