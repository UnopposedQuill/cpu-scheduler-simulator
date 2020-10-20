//
// Created by esteban on 20/10/20.
//

#ifndef CLIENT_REQUESTER_H
#define CLIENT_REQUESTER_H

#include "configuration.h"

struct requesterData{
    /**
     * Configuration in which the requester thread will act upon
     */
    struct configuration _configuration;

    /**
     * Line as read from the file, it is the server's job to parse it for simplicity
     */
    char * line;
};

void * requesterJob(void * arguments);

#endif //CLIENT_REQUESTER_H
