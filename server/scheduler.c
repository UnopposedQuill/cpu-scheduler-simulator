//
// Created by esteban on 18/10/20.
//

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

#include "scheduler.h"
#include "pcb.h"

void * clockTickerWork(void * arguments){
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;
    while (_schedulerInfo->working){
        sleep(1);
        _schedulerInfo->tick++;
    }
    return NULL;
}

void * jobSchedulerWork(void * arguments){
    printf("Job scheduler initialized\n");
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;
    insertNewPcb(_schedulerInfo->readyList, createPcb(10, 2, 1, _schedulerInfo->tick));
    sleep(1);
    insertNewPcb(_schedulerInfo->readyList, createPcb(12, 2, 1, _schedulerInfo->tick));
    return NULL;
}

void * cpuSchedulerWork(void * arguments){
    printf("Cpu Scheduler initialized\n");
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
                _schedulerInfo->currentProcess->tickOfCompletion = _schedulerInfo->tick;
                printf("Completed job: %d\n", _schedulerInfo->currentProcess->pid);
                removePcbPid(_schedulerInfo->readyList, scheduledPcb->node->pid);
                free(scheduledPcb);//Discard the wrapper
                insertNewPcb(_schedulerInfo->doneList, _schedulerInfo->currentProcess);//I completed it, add it to done
            }
            else {
                unsigned int amountOfWork = 1;
                if (_schedulerInfo->_configuration->schedulerType == ROUNDROBIN){
                    amountOfWork = _schedulerInfo->_configuration->quantum < scheduledPcb->node->burst - scheduledPcb->node->progress ?
                            _schedulerInfo->_configuration->quantum : scheduledPcb->node->burst - scheduledPcb->node->progress;
                }
                sleep(amountOfWork);
                scheduledPcb->node->progress += amountOfWork;
                if (scheduledPcb->node->progress == scheduledPcb->node->burst){
                    _schedulerInfo->currentProcess->tickOfCompletion = _schedulerInfo->tick;
                    printf("Completed job: %d\n", _schedulerInfo->currentProcess->pid);
                    free(scheduledPcb);//Discard the wrapper
                    insertNewPcb(_schedulerInfo->doneList, _schedulerInfo->currentProcess);//I completed it, add it to done
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
        case FIFO:{//I place the last received one in last place, so I just pick the first one
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

void showStatistics(struct schedulerInfo * _schedulerInfo){
    printf("\n\n----------------------------------------\nStatistics: ");
    switch (_schedulerInfo->_configuration->schedulerType) {
        case FIFO:{
            printf("FIFO");
            break;
        }
        case SJF:{
            printf("SJF");
            break;
        }
        case HPF:{
            printf("HPF");
            break;
        }
        case ROUNDROBIN:{
            printf("Round Robin");
            break;
        }
        case ASJF:{
            printf("Appropiative SJF");
            break;
        }
        case AHPF:{
            printf("Appropiative HPF");
            break;
        }
    }
    printf(" Scheduler\nIdle Time: %d\nProcesses completed: %d\nProcesses still ready: %d\n\nDetailed per-process statistics:\n", _schedulerInfo->idleTicks, _schedulerInfo->doneList->len, _schedulerInfo->readyList->len);

    struct pcbNode * ptr = _schedulerInfo->doneList->firstNode;
    double averageTurnAroundTime = 0, averageWaitingTime = 0;

    while (ptr != NULL && ptr->node != NULL){//No wrapper should have it's node NULL, but I prefer to prevent errors
        unsigned int turnaroundTime = ptr->node->tickOfCompletion - ptr->node->tickOfEntry;
        printf("Process pid: %d, burst: %d, tick of entry: %d, tick of completion: %d, turn-around time: %d, waiting time: %d\n",
               ptr->node->pid, ptr->node->burst, ptr->node->tickOfEntry, ptr->node->tickOfCompletion, turnaroundTime, turnaroundTime - ptr->node->burst);

        averageTurnAroundTime += turnaroundTime;
        averageWaitingTime += (turnaroundTime - ptr->node->burst);
        ptr = ptr->next;
    }

    printf("\nAverage Turn-around time: %f, average waiting time: %f", averageTurnAroundTime / _schedulerInfo->doneList->len, averageWaitingTime / _schedulerInfo->doneList->len);
}
