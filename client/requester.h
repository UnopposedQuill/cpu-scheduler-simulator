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
    struct configuration * _configuration;

    /**
     * Line as read from the file, it is the server's job to parse it for simplicity
     */
    char * line;

    /**
     * Place to store the pthread
     */
    pthread_t _pthread;

    /**
     * It will be set to 1 once the thread has completed its work
     */
    short isDone;

    /**
     * This will be used to id the different threads
     */
    unsigned int threadId;
};

/**
 * I need a structure to keep the requesterData hooked together
 */
struct requesterDataNode{
    struct requesterData * node;

    struct requesterDataNode * next, * previous;
};

struct requesterDataList {
    struct requesterDataNode *firstNode;
    unsigned int len;
};

void * requesterJob(void * arguments);
struct requesterDataNode * insertNewThread(struct requesterDataList * _requesterDataList, struct requesterData * _requesterData);
int deleteDone(struct requesterDataList * _requesterDataList);
short removeThreadId(struct requesterDataList * _requesterDataList, unsigned int threadId);
int clearList(struct requesterDataList * _requesterDataList);
void destroyRequesterData(struct requesterData * _requesterData);

#endif //CLIENT_REQUESTER_H
