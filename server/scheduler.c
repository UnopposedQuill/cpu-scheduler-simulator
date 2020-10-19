//
// Created by esteban on 18/10/20.
//

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

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
    //While it is allowed to keep working
    while (_schedulerInfo->working){
        printf("Selecting new process to execute\n");
        struct pcbNode * scheduledPcb = schedule(_schedulerInfo);
        if (scheduledPcb == NULL){//Didn't find any work to do, get into idle mode
            printf("No ready jobs in queue\n");
            _schedulerInfo->idleTicks++;
            sleep(1);
        }
        else{ //There's work I can do
            printf("Job selected: %d\n", scheduledPcb->node->pid);
            _schedulerInfo->currentProcess = scheduledPcb->node;//I design the found pcb as the one it's being worked
            if (    _schedulerInfo->_configuration->schedulerType != ASJF &&
                    _schedulerInfo->_configuration->schedulerType != AHPF &&
                    _schedulerInfo->_configuration->schedulerType != ROUNDROBIN){
                sleep(_schedulerInfo->currentProcess->burst);
                _schedulerInfo->currentProcess->progress = _schedulerInfo->currentProcess->burst;
                printf("Completed job: %d\n", _schedulerInfo->currentProcess->pid);
                removePcbPid(_schedulerInfo->readyList, scheduledPcb->node->pid);
                free(scheduledPcb);
                free(_schedulerInfo->currentProcess);
            }
            else {
                sleep(_schedulerInfo->_configuration->schedulerType == ROUNDROBIN ? _schedulerInfo->_configuration->quantum : 1);
                scheduledPcb->node->progress += _schedulerInfo->_configuration->schedulerType == ROUNDROBIN ? _schedulerInfo->_configuration->quantum : 1;
                if (scheduledPcb->node->progress == scheduledPcb->node->burst){
                    printf("Completed job: %d\n", _schedulerInfo->currentProcess->pid);
                    free(scheduledPcb);
                    free(_schedulerInfo->currentProcess);
                }
            }
        }
    }
    return NULL;
}

struct pcbNode * schedule(struct schedulerInfo * _schedulerInfo){

    /**
     * I'll employ a strategy pattern here, depending on the value of the scheduler type
     */
    switch (_schedulerInfo->_configuration->schedulerType) {
        case FIFO:{
            return _schedulerInfo->readyList->firstNode;
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