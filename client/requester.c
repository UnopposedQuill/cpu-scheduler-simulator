//
// Created by esteban on 20/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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
        _requesterData->isDone = 1;//Found an error, mark it as done
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
        _requesterData->isDone = 1;//Found an error, mark it as done
        return NULL;
    }

    //Now I try connecting to the server
    if(connect(socket_handler, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
        perror("Couldn't connect to server");
        _requesterData->isDone = 1;//Flag this thread as finished
        return NULL;
    }
    // </editor-fold>


    //Try to send it to the server
    if ((valread = send(socket_handler, _requesterData->line, strlen(_requesterData->line) + 1, 0)) <= 0){
        perror("Couldn't write data to server");
        close(socket_handler);
        _requesterData->isDone = 1;//Flag this thread as finished
        return NULL;
    }
    printf("Process: %s", buffer);
    //Clear the buffer before reading again
    memset(buffer, 0, 256);
    //Now I'll output the Pid the server assigned to it
    if ((valread = recv(socket_handler, buffer, 256, 0)) <= 0){
        perror("Couldn't receive back the pid assigned by the server");
        close(socket_handler);
        _requesterData->isDone = 1;//Flag this thread as finished
        return NULL;
    }
    printf(".\t Pid assigned by server: %s\n", buffer);

    _requesterData->isDone = 1;//It is done
    return NULL;
}

struct requesterDataNode * insertNewThread(struct requesterDataList * _requesterDataList, struct requesterData *  _requesterData){
    if (_requesterData == NULL || _requesterDataList == NULL) return NULL;//No pcb to insert, or no list to insert

    //Wrap the thread in a node
    struct requesterDataNode * wrapper;
    do {//Will block until it can create a wrapper
        wrapper = malloc(sizeof(struct requesterDataNode));
    } while (wrapper == NULL);

    //Assign the data inside the wrapper
    wrapper->node = _requesterData;
    wrapper->next = NULL;
    _requesterDataList->len++;

    //If there are no elements, just place it there
    if (_requesterDataList->firstNode == NULL){
        _requesterDataList->firstNode = wrapper;
        wrapper->previous = NULL;
    }
    else{
        //I need to go through all the list, until the last element
        struct requesterDataNode * ptr = _requesterDataList->firstNode;
        while (ptr->next != NULL) ptr = ptr->next;

        //Then hook the current wrapper to the last element
        ptr->next = wrapper;
        wrapper->previous = ptr;
    }

    return wrapper;
}

int deleteDone(struct requesterDataList * _requesterDataList){
    int threadsCleared = 0;
    //Can't remove nodes from empty list
    if (_requesterDataList != NULL){
        struct requesterDataNode * clearNode, * ptr = _requesterDataList->firstNode;
        //While there are more nodes to check
        while (ptr != NULL){
            //If the current node I have found is done
            if (ptr->node->isDone){

                //A simple removal similar to removeThreadId, I need to correct the rest of the list
                if (ptr->previous != NULL){
                    ptr->previous->next = ptr->next;
                }
                if (ptr->next != NULL){
                    ptr->next->previous = ptr->previous;
                }
                //If I'm sitting in the first node, I need to reassign it to the next one
                if (_requesterDataList->firstNode == ptr){
                    _requesterDataList->firstNode = ptr->next;
                }

                //Modify counters
                threadsCleared++;
                _requesterDataList->len--;

                //Delete the node's data
                destroyRequesterData(ptr->node);

                //Now delete the node, without tampering ptr
                clearNode = ptr;
                ptr = ptr->next;
                free(clearNode);
            } else {//I just need to skip to the next node
                ptr = ptr->next;
            }
        }
    }
    return threadsCleared;
}

short removeThreadId(struct requesterDataList * _requesterDataList, unsigned int threadId){
    if (_requesterDataList == NULL) return 0;//No list to remove from

    struct requesterDataNode * ptr = _requesterDataList->firstNode;
    while (ptr != NULL){
        //If the current pointer matches thread id, remove it
        if (ptr->node->threadId == threadId){
            if (ptr->previous != NULL){
                ptr->previous->next = ptr->next;
            }
            if (ptr->next != NULL){
                ptr->next->previous = ptr->previous;
            }
            _requesterDataList->len--;
            if (_requesterDataList->firstNode == ptr){
                _requesterDataList->firstNode = ptr->next;
            }
            free(ptr);//Deallocate it
            return 1;
        }

        //Keep looking
        ptr = ptr->next;
    }
    //Didn't find it
    return 0;
}

int clearList(struct requesterDataList * _requesterDataList){
    int threadsCleared = 0;
    if (_requesterDataList != NULL){
        struct requesterDataNode * clearNode, * ptr = _requesterDataList->firstNode;
        while (ptr != NULL){
            free(ptr->node);
            clearNode = ptr;
            ptr = ptr->next;
            free(clearNode);
            threadsCleared++;
        }
        _requesterDataList->len = 0;
        _requesterDataList->firstNode = NULL;
    }
    return threadsCleared;
}

/**
 * This function will take care of freeing all the memory in a requesterData structure
 * @param _requesterData The structure that isn't needed anymore, and will de free'd
 */
void destroyRequesterData(struct requesterData * _requesterData){
    free(_requesterData->line);//This was assigned dynamically
    free(_requesterData);
}
