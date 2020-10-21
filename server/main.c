
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
    struct pcbList readyList, doneList;
    readyList.firstNode = NULL;
    readyList.len = 0;

    doneList.firstNode = NULL;
    doneList.len = 0;

    //I'll create this so both schedulers can access it
    struct schedulerInfo _schedulerInfo;
    _schedulerInfo.readyList = &readyList;
    _schedulerInfo.doneList = &doneList;
    _schedulerInfo.lastPid = 0;
    _schedulerInfo.tick = 0;
    _schedulerInfo.idleTicks = 0;
    _schedulerInfo._configuration = &_configuration;

    //</editor-fold>

    printf("Data structures prepared, preparing threads\n");

    // <editor-fold defaultstate=collapsed desc="Threads Setup">
    pthread_t threads[3];

    //Create the job scheduler, check for errors
    int result = pthread_create(&threads[0], NULL, jobSchedulerWork, &_schedulerInfo);
    if (result)
        return 1;

    //Create the cpu scheduler, check for errors
    result = pthread_create(&threads[1], NULL, cpuSchedulerWork, &_schedulerInfo);
    if (result)
        return 1;

    //Create the tick increaser, check for errors
    result = pthread_create(&threads[2], NULL, clockTickerWork, &_schedulerInfo);
    if (result)
        return 1;

    //</editor-fold>

    printf("Threads prepared\n");

    // <editor-fold defaultstate=collapsed desc="Closing actions">
    int c;

    printf( "Program is working, press a key to finish\n");
    c = getchar();

    //Flag the threads to stop working
    _schedulerInfo.working = 0;

    //I'll try to join all the threads before exiting
    result = pthread_join(threads[1], NULL);
    if (!result) {
        //I joined them all without issues, I'll show statistics before leaving
        showStatistics(&_schedulerInfo);

        //Clear both lists
        clearList(_schedulerInfo.readyList);
        clearList(_schedulerInfo.doneList);
        return 0;//All is good
    }
    return 1;

    // </editor-fold>
}
