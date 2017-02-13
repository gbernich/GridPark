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

    void * db;
    int i;
    OPEN_SPOT_T * spots = NULL;
    PARKED_CAR_T * cars = NULL;
    SUSP_ACTIVITY_T * acts = NULL;

    // Check for command line inputs
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

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
    serv_addr.sin_port = htons(5000); 

    // Convert IP address to binary
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    // Connect the socket to the network
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    // Receive the packets until done, write to command line
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    // Error reading from socket
    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    db = (void *)OpenDB(K_DB);
    //FormatInsertForOpenParking(queryString, K_TBL_OPEN_PARKING, 0, 1, 2, 3, 4, 5, 6);
    //InsertEntry(db, queryString); 
    //ClearTable(db, K_TBL_OPEN_PARKING);
    //i = TableIsLocked(db, K_TBL_OPEN_PARKING);
    //printf("%d\n", i);

    //WaitForLock(db, K_TBL_OPEN_PARKING);
    //printf("Got the lock!\n");
    //UnlockTable(db, K_TBL_OPEN_PARKING);
    //printf("Released the lock!\n");

    spots = GetOpenSpots(db, K_TBL_OPEN_PARKING);
    PrintOpenSpots(spots);
    DeleteOpenSpots(spots);

    cars = GetParkedCars(db, K_TBL_PARKED_CARS);
    PrintParkedCars(cars);
    DeleteParkedCars(cars);

    acts = GetSuspActivity(db, K_TBL_SUSP_ACTIVITY);
    PrintSuspActivities(acts);
    DeleteSuspActivities(acts);

    CloseDB(db);

    return 0;
}
