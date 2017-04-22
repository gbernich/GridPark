//-----------------------------------------------------------------------------
// File:                recv.c
// Author:              Garrett Bernichon
// Function:            To receive data and write it to the necessary
//                      mySQL database table.
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

char queryString[K_QUERY_STRING_LENGTH] = {0};

int main(int argc, char *argv[])
{
    int sockfd = 0;
    int n = 0;
    char recvBuff[1024] = {0};
    struct sockaddr_in serv_addr; 
    FILE * json = NULL;

    void * db = NULL;
    int i;
    OPEN_SPOT_T * spots = NULL;
    PARKED_CAR_T * cars = NULL;
    SUSP_ACTIVITY_T * acts = NULL;
    int port_type = 0;

    //inserting data
    int num_entries = 0;
    int entry_type = 0;
    int j = 0;

    int args[7] = {0};


    // Check for command line inputs
    if(argc != 3)
    {
        printf("\n Usage: %s <server IP addr> <server port number> \n",argv[0]);
        return 1;
    }
    else
    {
        port_type = atoi(argv[2]) + K_PORT_OFFSET;
    }
    while (1)
    {

        // Create the socket
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Error : Could not create socket \n");
            return 1;
        }

        // Zero out serv_addr struct
        memset(&serv_addr, '0', sizeof(serv_addr)); 

        // Set up socket
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port_type); 

        // Convert IP address to binary
        if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
        {
            printf("\n inet_pton error occured\n");
            return 1;
        }

        // Open database
        db = (void *)OpenDB(K_DB);

        // Connect the socket to the network
        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\n Error : Connect Failed \n");
            return 1;
        }

        // Open Parking JSON file
        json = fopen("/var/www/html/garrett.json", "w");
        WriteOpenParkingJSONHeader(json);

        // Receive the packets until done, write to command line
        while ( (n = read(sockfd, recvBuff, sizeof(recvBuff))) > 0)
        {
            //clear table
            if (entry_type == K_PACKET_OPEN_PARKING)
            {
                ClearTable(db, K_TBL_OPEN_PARKING);
                WriteOpenParkingJSON(recvBuff)
            }
            else if (entry_type == K_PACKET_PARKED_CARS)
                ClearTable(db, K_TBL_PARKED_CARS);
            else
                ClearTable(db, K_TBL_SUSP_ACTIVITY);
	        printf("here3\n");

            //insert data
            num_entries = recvBuff[1];
            entry_type = recvBuff[0];

            j = 2;
            for (i = 0; i < num_entries; i++){
                if (entry_type == K_PACKET_OPEN_PARKING)
                {
                    GetSocketArgs(args, recvBuff, j, K_PACKET_OPEN_PARKING);
                    j = j + K_TBL_OPEN_PARKING_COLUMNS * sizeof(int);
                    FormatInsertForOpenParking(queryString, K_TBL_OPEN_PARKING,
                        args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
                    InsertEntry(db, queryString);

                    WriteOpenParkingJSONEntry(json, args);
                    if (i < num_entries - 1)
                        WriteOpenParkingJSONComma(json);
                }
                else if (entry_type == K_PACKET_PARKED_CARS)
                {
                    GetSocketArgs(args, recvBuff, j, K_PACKET_PARKED_CARS);
                    j = j + K_TBL_PARKED_CARS_COLUMNS * sizeof(int);
                    FormatInsertForParkedCars(queryString, K_TBL_PARKED_CARS,
                        args[0], args[1], args[2], args[3], args[4], args[5]);
                    InsertEntry(db, queryString);
                }
                else // K_PACKET_SUSP_ACTIVITY
                {
                    GetSocketArgs(args, recvBuff, j, K_PACKET_SUSP_ACTIVITY);
                    j = j + K_TBL_SUSP_ACTIVITY_COLUMNS * sizeof(int);
                    FormatInsertForSuspActivity(queryString, K_TBL_SUSP_ACTIVITY,
                        args[0], args[1], args[2]);
                    InsertEntry(db, queryString);
                }
            }
            /*recvBuff[n] = 0;
            if(fputs(recvBuff, stdout) == EOF)
            {
                printf("\n Error : Fputs error\n");
            }*/
        }

        // Error reading from socket
        if(n < 0)
        {
            printf("\n Read error \n");
        }

        // Close parking JSON file
        WriteOpenParkingJSONFooter(json);
        fclosef(json);

        // close database
        CloseDB(db);

        // close socket
        close(sockfd);

        // Sleep
        sleep(5);
    }

    //db = (void *)OpenDB(K_DB);
    //FormatInsertForOpenParking(queryString, K_TBL_OPEN_PARKING, 0, 1, 2, 3, 4, 5, 6);
    //InsertEntry(db, queryString);
    //ClearTable(db, K_TBL_OPEN_PARKING);
    //i = TableIsLocked(db, K_TBL_OPEN_PARKING);
    //printf("%d\n", i);

    //WaitForLock(db, K_TBL_OPEN_PARKING);
    //printf("Got the lock!\n");
    //UnlockTable(db, K_TBL_OPEN_PARKING);
    //printf("Released the lock!\n");

    /*
    spots = GetOpenSpots(db, K_TBL_OPEN_PARKING);
    PrintOpenSpots(spots);
    DeleteOpenSpots(spots);

    cars = GetParkedCars(db, K_TBL_PARKED_CARS);
    PrintParkedCars(cars);
    DeleteParkedCars(cars);

    acts = GetSuspActivity(db, K_TBL_SUSP_ACTIVITY);
    PrintSuspActivities(acts);
    DeleteSuspActivities(acts);
    */
    //CloseDB(db);

    return 0;
}
