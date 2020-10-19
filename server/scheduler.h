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
    struct pcbList * readyList;
    struct configuration * _configuration;
};

enum schedulerType{FIFO, SJF, HPF, ROUNDROBIN, ASJF, AHPF};


struct pcbNode * schedule(struct schedulerInfo * _schedulerInfo);
void * jobSchedulerWork(void * arguments);
void * cpuSchedulerWork(void * arguments);

#endif //SERVER_SCHEDULER_H
