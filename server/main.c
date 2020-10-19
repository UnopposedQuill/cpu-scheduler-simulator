
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "configuration.h"
#include "pcb.h"
#include "scheduler.h"

int main() {

    //<editor-fold defaultstate=collapsed desc="Configuration">
    //Declare a new configuration
    struct configuration _configuration;
    configure(&_configuration);//Overwrite it with a configuration

    if (!_configuration.isValid) {
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

    struct pcb *_pcb = createPcb(1, 2, 1, 0);

    insertNewPcb(&readyList, _pcb);

    clearList(&readyList);

    //I'll create this so both schedulers can access it
    struct schedulerInfo _schedulerInfo;
    _schedulerInfo.readyList = &readyList;
    _schedulerInfo.tick = 0;
    _schedulerInfo._configuration = _configuration;

    //</editor-fold>

    printf("Data structures prepared, preparing threads\n");

    // <editor-fold defaultstate=collapsed desc="Threads Setup">
    pthread_t threads[2];
    int result = pthread_create(&threads[0], NULL, jobSchedulerWork, &_schedulerInfo);
    if (result)
        return 1;

    result = pthread_create(&threads[1], NULL, cpuSchedulerWork, &_schedulerInfo);
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
