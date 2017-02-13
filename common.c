//-----------------------------------------------------------------------------
// File:                common.c
// Author:              Garrett Bernichon
// Function:            Global structs
//-----------------------------------------------------------------------------

// OPEN_SPOT_T
OPEN_SPOT_T * CreateOpenSpot(int a, int b, int c, int d, int e, int f, int g)
{
    OPEN_SPOT_T * spot = NULL;
    spot = malloc(sizeof(OPEN_SPOT_T));
    spot -> spot_id = a
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
    return ptr;
}

void PrintOpenSpots(OPEN_SPOT_T * head)
{
    OPEN_SPOT_T * spot = head;

    do {
        printf("spot_id= %d, region= %d, dist= %d, corners= %d %d %d %d\n",
            spot->spot_id, spot->region, spot->distance, spot->corner0,
            spot->corner1, spot->corner2, spot->corner3);
        spot = spot -> next;
    } while(spot != NULL);
    printf("\n");
}

// PARKED_CAR_T



// SUSP_ACTIVITY_T



