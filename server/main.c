
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "configuration.h"
#include "pcb.h"

void * jobSchedulerWork(void * arguments){
    printf("Inserting test jobs");
    struct pcbList * readyList = (struct pcbList *) arguments;
    insertNewPcb(readyList, createPcb(10, 2, 1));
    return NULL;
}

void * cpuSchedulerWork(void * arguments){
    printf("Removing test jobs");
    struct pcbList * readyList = (struct pcbList *) arguments;
    removePcbPid(readyList, 10);
    return NULL;
}

int main() {

    //<editor-fold defaultstate=collapsed desc="Configuration">
    //Declare a new configuration
    struct configuration conf;
    configure(&conf);//Overwrite it with a configuration

    if (!conf.isValid) {
        printf("Configuration invalid");
        return 1;
    }

    //</editor-fold>

    printf("Valid configuration, preparing data structures\n");

    //<editor-fold defaultstate=collapsed desc="Data Structures">
    //A new ready list, initialized on 0s
    struct pcbList readyList;
    readyList.firstNode = NULL;
    readyList.len = 0;

    //Current process that's being worked on by the CPU
    struct pcb current;

    struct pcb *_pcb = createPcb(1, 2, 1);

    insertNewPcb(&readyList, _pcb);

    clearList(&readyList);
    //</editor-fold>

    printf("Data structures prepared, preparing threads\n");

    // <editor-fold defaultstate=collapsed desc="Threads Setup">
    pthread_t threads[2];
    int result = pthread_create(&threads[0], NULL, jobSchedulerWork, &readyList);
    if (result)
        return 1;

    result = pthread_create(&threads[1], NULL, cpuSchedulerWork, &readyList);
    if (result)
        return 1;

    result = pthread_join(threads[0], NULL);
    if (!result) {
        if (readyList.len > 0) {
            printf("Pid in list: %d", readyList.firstNode->node->pid);
        }
        return 0;
    }
    return 1;
}
