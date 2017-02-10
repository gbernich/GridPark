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

#include "network.h"

int main(int argc, char *argv[])
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025] = {0};
    time_t ticks; 

    // Open socket to listen on
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0 , sizeof(serv_addr));

    // Set up parameters for the connection
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    // Listen for up to 10 connections
    listen(listenfd, 10); 

    while(1)
    {
        // Accept request to connect
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        // Create a packet to send
        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));

        // Send packet over socket
        write(connfd, sendBuff, strlen(sendBuff)); 

        // Close connection
        close(connfd);

        // Sleep for some time
        sleep(1);
     }
}
