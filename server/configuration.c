//
// Created by esteban on 14/10/20.
//
#include <stdio.h>

#include "configuration.h"

struct configuration{

    /**
     * The most important step, if > 0, this configuration will be valid
     */
    short isValid;

    /**
     * There are multiple types of schedulers, this will signal which one is being simulated
     */
    int schedulerType;

    /**
     * This is needed for the server, to decide how it will attend to requests
     */
    int maximumPendingConnections, port;
};

void configure(struct configuration * conf){
    conf->isValid = 0;//I'll flag it as not valid until I find it otherwise

    //I'll try with the default name
    FILE * confFile = fopen("config.ini", "r");
    if (confFile == NULL){//There is no config file
        return;
    }

    fscanf(confFile,
           "[SETUP]\n"
           "Port=%d\n"
           "MaxWaitingConnections=%d\n"
           "SchedulerType=%d\n", &conf->port, &conf->maximumPendingConnections, &conf->schedulerType);
    fclose(confFile);
    conf->isValid = 1;
}