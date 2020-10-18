#include <stdio.h>

#include "configuration.h"
#include "pcb.h"

int main() {

    //<editor-fold defaultstate=collapsed desc="Configuration">
    //Declare a new configuration
    struct configuration conf;
    configure(&conf);//Overwrite it with a configuration

    if (!conf.isValid){
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

    struct pcb * _pcb = createPcb(1, 2, 1);

    insertNewPcb(&readyList, _pcb);

    clearList(&readyList);
    //</editor-fold>

    printf("Data structures prepared, preparing threads\n");


    return 0;
}
