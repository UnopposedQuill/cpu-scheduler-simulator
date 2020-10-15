#include <stdio.h>

#include "configuration.h"

struct configuration {
    short isValid;
};

int main() {
    //<editor-fold defaultstate=collapsed desc="Configuration">
    //Declare a new configuration
    struct configuration conf;
    configure(&conf);//Overwrite it with a configuration

    if (!conf.isValid){
        printf("Configuration invalid");
        return 1;
    }
    printf("Valid configuration, preparing sockets\n");

    //</editor-fold>

    //<editor-fold defaultstate=collapsed desc="Initialization">

    //</editor-fold>
    return 0;
}
