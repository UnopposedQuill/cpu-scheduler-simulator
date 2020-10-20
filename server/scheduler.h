//
// Created by esteban on 18/10/20.
//

#include "configuration.h"
#include "pcb.h"

#ifndef SERVER_SCHEDULER_H
#define SERVER_SCHEDULER_H

struct schedulerInfo{
    unsigned int tick, idleTicks;
    int working;//This will be used as a flag as to when to stop
    struct pcb * currentProcess;
    struct pcbList * readyList, * doneList;
    struct configuration * _configuration;
};

enum schedulerType{FIFO, SJF, HPF, ROUNDROBIN, ASJF, AHPF};


struct pcbNode * schedule(struct schedulerInfo * _schedulerInfo);

/**
 * This function will take care of increasing the tick the CPU is running on
 * @param arguments A schedulerInfo struct with the tick in it
 * @return NULL
 */
void * clockTickerWork(void * arguments);

/**
 * This function will take care of receiving socket requests and adding their PCB to the ready list
 * @param arguments A schedulerInfo struct representing all the necessary information to keep this thread informed
 * @return NULL
 */
void * jobSchedulerWork(void * arguments);

/**
 * This function will take care of choosing which job will be run next, as well as increasing the counters of work
 * @param arguments A schedulerInfo struct with the information of the CPU
 * @return NULL
 */
void * cpuSchedulerWork(void * arguments);

/**
 * This function will take care of dumping all the data in the supplied CPU state in a human readable form
 * @param _schedulerInfo A schedulerInfo struct with the information this function should output
 */
void showStatistics(struct schedulerInfo * _schedulerInfo);

#endif //SERVER_SCHEDULER_H
