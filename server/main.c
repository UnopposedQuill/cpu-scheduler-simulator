#include <stdio.h>

#include "configuration.h"

struct configuration {
    short isValid;
};

int main() {
    //Declare a new configuration
    struct configuration conf;
    configure(&conf);//Overwrite it with a configuration

    if (!conf.isValid){
        printf("Configuration invalid");
        return 1;
    }
    printf("Valid configuration, preparing sockets\n");
    return 0;
}
