//
// Created by esteban on 18/10/20.
//

#include <stdio.h>

#include "scheduler.h"
#include "pcb.h"

void * jobSchedulerWork(void * arguments){
    printf("Inserting test jobs\n");
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;
    insertNewPcb(_schedulerInfo->readyList, createPcb(10, 2, 1, _schedulerInfo->tick));
    return NULL;
}

void * cpuSchedulerWork(void * arguments){
    printf("Removing test jobs\n");
    struct pcbList * readyList = (struct pcbList *) arguments;
    struct pcbNode * removedNode = removePcbPid(readyList, 10);
    printf("Removed node pid: %d\n", removedNode->node->pid);
    return NULL;
}

struct pcbNode * schedule(struct schedulerInfo _schedulerInfo){

    return NULL;
}