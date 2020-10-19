//
// Created by esteban on 17/10/20.
//
#include <stdlib.h>
#include "pcb.h"

/**
 * This function will take care of creating and asigning the data to a new PCB
 * This *WILL* block until it's capable of requesting memory in order to create the new PCB
 * @param pid The assigned PID
 * @param burst The burst for the process
 * @param priority The priority for the process
 * @return A pcb structure representing the information for the process
 */
struct pcb * createPcb(unsigned int pid, unsigned int burst, unsigned int priority, unsigned int tick){
    struct pcb * target;
    do {
        target = malloc(sizeof(struct pcb));
    } while (target == NULL);
    target->pid = pid;
    target->burst = burst;
    target->priority = priority;
    target->progress = 0;
    target->tickOfEntry = tick;
    return target;
}

void destroyPcb(struct pcb * _pcb){
    free(_pcb);
}

/**\
 * This function will take care of inserting a non-null pcb in a non-null pcb list
 * This *WILL* block until it's capable of requesting memory in order to add the new pcb to the list
 * @param _pcbList The list to which the pcb will be added
 * @param _pcb The pcb to add
 * @return A pointer to the added wrapper containing the pcb
 */
struct pcbNode * insertNewPcb(struct pcbList * _pcbList, struct pcb * _pcb){
    if (_pcb == NULL || _pcbList == NULL) return NULL;//No pcb to insert, or no list to insert

    //Wrap the pcb in a node
    struct pcbNode * wrapper;
    do {//Will block until it can create a wrapper
        wrapper = malloc(sizeof(struct pcbNode));
    } while (wrapper == NULL);

    //Assign the data inside the wrapper
    wrapper->node = _pcb;
    wrapper->next = NULL;
    _pcbList->len++;

    //If there are no elements, just place it there
    if (_pcbList->firstNode == NULL){
        _pcbList->firstNode = wrapper;
        wrapper->previous = NULL;
    }
    else{
        //I need to go through all the list, until the last element
        struct pcbNode * ptr = _pcbList->firstNode;
        while (ptr->next != NULL) ptr = ptr->next;

        //Then hook the current wrapper to the last element
        ptr->next = wrapper;
        wrapper->previous = ptr;
    }

    return wrapper;
}

struct pcbNode * removePcbPid(struct pcbList * _pcbList, int pid){
    if (_pcbList == NULL) return NULL;//No list to remove from

    struct pcbNode * ptr = _pcbList->firstNode;
    while (ptr != NULL){
        //If the current pointer matches pid, return it
        if (ptr->node->pid == pid){
            if (ptr->previous != NULL){
                ptr->previous->next = ptr->next;
            }
            if (ptr->next != NULL){
                ptr->next->previous = ptr->previous;
            }
            _pcbList->len--;
            if (_pcbList->firstNode == ptr){
                _pcbList->firstNode = NULL;
            }
            return ptr;
        }

        //Keep looking
        ptr = ptr->next;
    }
    //Didn't find it
    return NULL;
}

int clearList(struct pcbList * _pcbList){
    int pcbCleared = 0;
    if (_pcbList != NULL){
        struct pcbNode * clearNode, * ptr = _pcbList->firstNode;
        while (ptr != NULL){
            destroyPcb(ptr->node);
            clearNode = ptr;
            ptr = ptr->next;
            free(clearNode);
            pcbCleared++;
        }
        _pcbList->len = 0;
        _pcbList->firstNode = NULL;
    }
    return pcbCleared;

}
