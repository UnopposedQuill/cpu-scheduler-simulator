//
// Created by esteban on 20/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "requester.h"

void * requesterJob(void * arguments){

    struct requesterData * _requesterData = (struct requesterData*)arguments;

    // <editor-fold defaultstate=collapsed desc="Socket preparation">
    //First I need a handler for the socket I'll use to communicate with the server
    int socket_handler, valread;

    //Now the address to the server
    struct sockaddr_in serv_addr;

    //A buffer for the communication
    char buffer[256];memset(buffer, 0, 256);//Create and clear the buffer

    //First I need to create the socket in the handler
    if((socket_handler = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation error");
        return NULL;
    }

    //I need to clear some values I don't need
    memset(&serv_addr, 0, sizeof(serv_addr));

    //Time to add the values I do need, first the protocol: ipv4
    serv_addr.sin_family = AF_INET;
    //Now the port
    serv_addr.sin_port = htons(_requesterData->_configuration.port);

    //Now I convert the address into it's binary network form
    if(inet_pton(AF_INET, _requesterData->_configuration.serverAddress, &serv_addr.sin_addr) <= 0){
        perror("Address invalid or not supported");
        return NULL;
    }

    //Now I try connecting to the server
    if(connect(socket_handler, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
        perror("Couldn't connect to server");
        return NULL;
    }
    // </editor-fold>

    /*
    //Try to send it to the server
    if ((valread = send(socket_handler, buffer, valread, 0)) < 0){
        perror("Couldn't write data to server");
        free(line);
        close(socket_handler);
        return 1;
    }
    printf("Process: %s", buffer);
    //Clear the buffer before reading again
    memset(buffer, 0, 256);
    //Now I'll output the Pid the server assigned to it
    if ((valread = recv(socket_handler, buffer, 256, 0)) < 0){
        perror("Couldn't receive back the pid assigned by the server");
        free(line);
        close(socket_handler);
        return 1;
    }
    printf(".\t Pid assigned by server: %s\n", buffer);

    */
    return NULL;
}