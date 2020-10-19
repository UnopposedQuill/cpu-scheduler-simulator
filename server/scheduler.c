//
// Created by esteban on 18/10/20.
//

#include <stdio.h>
#include <unistd.h>

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
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;
    struct pcbNode * removedNode = removePcbPid(_schedulerInfo->readyList, 10);
    printf("Removed node pid: %d\n", removedNode->node->pid);
    sleep(_schedulerInfo->_configuration.schedulerType == FIFO ? 10 : 5);
    return NULL;
}

struct pcbNode * schedule(struct schedulerInfo _schedulerInfo){

    /**
     * I'll employ a strategy pattern here, depending on the value of the scheduler type
     */
    switch (_schedulerInfo._configuration.schedulerType) {
        case FIFO:{
            return _schedulerInfo.readyList->firstNode;
        }
        case SJF:{
            printf("SJF Not implemented yet\n");
        }
        case HPF:{
            printf("HJF Not implemented yet\n");
        }
        case ROUNDROBIN:{
            printf("RoundRobin Not implemented yet\n");
        }
        case ASJF:{
            printf("Appropiative SJF Not implemented yet\n");
        }
        case AHPF:{
            printf("Appropiative HPF Not implemented yet\n");
            return NULL;
        }
    }
    return NULL;
}