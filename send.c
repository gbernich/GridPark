//-----------------------------------------------------------------------------
// File:                send.c
// Author:              Garrett Bernichon
// Function:            To send data when it is available so the recv process
//                      can update its mySQL database table.
// Command Line Args:   
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h> 

#include "db_utils.h"
#include "common.h"

int main(int argc, char *argv[])
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025] = {0};
    char recvBuff[1025] = {0};
    int read_size;

    // Database
    void * db = NULL;
    char packet_type = 0;
    int port_type = 0;
    OPEN_SPOT_T * spots = NULL;
    OPEN_SPOT_T * s = NULL;
    PARKED_CAR_T * cars = NULL;
    PARKED_CAR_T * c = NULL;
    SUSP_ACTIVITY_T * acts = NULL;
    SUSP_ACTIVITY_T * a = NULL;
    int j = 0;
    int count = 0;

    // check for input
    if (argc != 2)
    {
        printf("Usage: ./send <packet_type>\n");
        return 0;
    }
    else
    {
        packet_type = (char)atoi(argv[1]);
        port_type = packet_type + K_PORT_OFFSET;
    }

    // Open socket to listen on
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0 , sizeof(serv_addr));

    // Set up parameters for the connection
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port_type); 
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    // Listen for up to 10 connections
    listen(listenfd, 10); 

    while(1)
    {
        // Accept request to connect
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        // Connect to database
        db = (void *)OpenDB(K_DB);

        // Read from database
        j = 2;
        count = 0;
        if (packet_type == K_PACKET_OPEN_PARKING)
        {
            // get open spots from DB
            spots = GetOpenSpots(db, K_TBL_OPEN_PARKING);
            s = spots;
            //PrintOpenSpots(spots);
            do
            {   // pack into buffer
                count++;
                PackIntoPacket(sendBuff, j, sizeof(int), s->spot_id);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), s->region);   j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), s->distance); j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), s->corner0);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), s->corner1);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), s->corner2);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), s->corner3);  j += sizeof(int);
                s = s -> next;
            } while (s != NULL);

            // free memory
            DeleteOpenSpots(spots);
        }
        else if (packet_type == K_PACKET_PARKED_CARS)
        {
            // get parked car info
            cars = GetParkedCars(db, K_TBL_PARKED_CARS);
            c = cars;
            do
            {   // pack into buffer
                count++;
                PackIntoPacket(sendBuff, j, sizeof(int), c->car_id);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), c->susp_activity);   j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), c->corner0);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), c->corner1);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), c->corner2);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), c->corner3);  j += sizeof(int);
                c = c -> next;
            } while (c != NULL);

            // free memory
            DeleteParkedCars(cars);
        }
        else // K_PACKET_SUSP_ACTIVITY
        {
            // get suspicious activity
            acts = GetSuspActivity(db, K_TBL_SUSP_ACTIVITY);
            a = acts;
            do
            {   // pack into buffer
                count++;
                PackIntoPacket(sendBuff, j, sizeof(int), a->car_id);  j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), a->time_of_detect);   j += sizeof(int);
                PackIntoPacket(sendBuff, j, sizeof(int), a->length_of_activity);  j += sizeof(int);
                a = a -> next;
            } while (a != NULL);

            // free memory
            DeleteSuspActivities(acts);
        }

        // Create a packet to send
        sendBuff[0] = (char)packet_type;
        sendBuff[1] = (char)count;

        // close database
        CloseDB(db);

        // Send packet over socket
        write(connfd, sendBuff, j);

        // Close connection
        close(connfd);

        // Sleep for some time
        sleep(1);
    }


    return 0;
}
