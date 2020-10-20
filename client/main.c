#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <malloc.h>

#include "configuration.h"
#include "requester.h"

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
    int valread;

    char buffer[256];

    //If the client is on auto or manual mode
    if (_configuration.clientMode){//1 for automatic mode
        printf("Automatic Mode not implemented yet\n");
    } else{
        //These I use to get the line from the file
        FILE * inputFile = fopen("processes.txt", "r");
        char * line = NULL;
        size_t len = 256;

        //These I use to create the threads dinamically
        int threadCount = 1;//I need to have these in order to increase the thread pool size
        pthread_t * threads = NULL;
        struct requesterData * requesterData = NULL;

        //While there is file to read, keep reading
        while ((valread = getline(&line, &len, inputFile)) != -1) {
            //Got the line, I'll copy it into the buffer

            line[valread - 1] = 0;//Add a null to the end of the line

            threads = realloc(threads, threadCount++);


            //Create the job scheduler, check for errors
            int result = pthread_create(&threads[0], NULL, requesterJob, requesterData);
            if (result)
                return 1;
        }
    }

    return 0;
}
