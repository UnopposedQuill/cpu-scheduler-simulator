//
// Created by esteban on 14/10/20.
//
#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"
#include "configuration.h"

void configure(struct configuration * conf){
    conf->isValid = 0;//I'll flag it as not valid until I find it otherwise

    //I'll try with the default name
    FILE * confFile = fopen("config.ini", "r");
    if (confFile == NULL){//There is no config file
        return;
    }

    char port[6], maximumPendingConnections[5], schedulerType[2];

    //This will take care of reading the config values and setting them into each variable
    fscanf(confFile,
           "[SETUP]\n"
           "Port=%s\n"
           "MaxWaitingConnections=%s\n"
           "SchedulerType=%s\n", port, maximumPendingConnections, schedulerType);

    //Now I need to convert, and insert each variable to its corresponding place in the configuration variable
    conf->port = (int) strtol(port, NULL, 10);
    conf->maximumPendingConnections = (int) strtol(maximumPendingConnections, NULL, 10);
    conf->schedulerType = (int) strtol(schedulerType, NULL, 10);

    //If schedulerType is RR, then I need to also receive the quantum size
    if (conf->schedulerType == ROUNDROBIN){
        char quantum [10];
        fscanf(confFile,
               "Quantum=%s\n", quantum);

        conf->quantum = (int) strtol(quantum, NULL, 10);
    }

    fclose(confFile);

    conf->isValid = 1;
}
