//
// Created by esteban on 19/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"

void configure(struct configuration * conf){
    conf->isValid = 0;//I'll flag it as not valid until I find it otherwise

    //I'll try with the default name
    FILE * confFile = fopen("config.ini", "r");
    if (confFile == NULL){//There is no config file
        return;
    }

    char port[6], clientMode[2], minimumBurst[16], maximumBurst[16], minimumIssuingTime[16], maximumIssuingTime[16];

    //This will take care of reading the config values and setting them into each variable
    fscanf(confFile,
           "[SETUP]\n"
           "ServerAddress=%s\n"
           "Port=%s\n"
           "ClientMode=%s\n", conf->serverAddress, port, clientMode);

    //Now I need to convert, and insert each variable to its corresponding place in the configuration variable
    conf->port = (int) strtol(port, NULL, 10);
    conf->clientMode = (int) strtol(clientMode, NULL, 10);

    //If the client mode is in automatic mode, I need to read additional values
    if (conf->clientMode == 1){
        fscanf(confFile, "MinimumBurst=%s\n"
                                "MaximumBurst=%s\n"
                                "MinimumIssuingTime=%s\n"
                                "MaximumIssuingTime=%s\n", minimumBurst, maximumBurst, minimumIssuingTime, maximumIssuingTime);

        conf->minimumBurst = (unsigned int) strtol(minimumBurst, NULL, 10);
        conf->maximumBurst = (unsigned int) strtol(maximumBurst, NULL, 10);
        conf->minimumIssuingTime = (unsigned int) strtol(minimumIssuingTime, NULL, 10);
        conf->maximumIssuingTime = (unsigned int) strtol(maximumIssuingTime, NULL, 10);
    }

    fclose(confFile);

    conf->isValid = 1;
}