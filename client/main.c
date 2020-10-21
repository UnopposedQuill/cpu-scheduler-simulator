
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <malloc.h>
#include <unistd.h>

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
    srand(time(NULL));

    //These I use to create the threads dinamically
    unsigned int threadCount = 0;//I need to have these in order to increase the thread pool size
    struct requesterData * _requesterData = NULL;

    //I need a new list for the threads
    struct requesterDataList _requesterDataList;
    _requesterDataList.firstNode = NULL;
    _requesterDataList.len = 0;

    //If the client is on auto or manual mode
    if (_configuration.clientMode){//1 for automatic mode
        while (1){
            deleteDone(&_requesterDataList);

            unsigned int burst, priority;

            burst = rand() % (_configuration.maximumBurst+1) + _configuration.minimumBurst;
            priority = rand() % 5 + 1;

            char * line;

            //Will block until it can allocate both variables' data
            do {
                _requesterData = malloc(sizeof(struct requesterData));
            } while (_requesterData == NULL);

            do {
                line = malloc(sizeof(char) * 256);
            } while (line == NULL);

            sprintf(line, "%d,%d", burst, priority);

            _requesterData->isDone = 0;//It's not done
            _requesterData->_configuration = &_configuration;
            _requesterData->threadId = threadCount++;
            _requesterData->line = line;

            //Add it to the ongoing threads
            insertNewThread(&_requesterDataList, _requesterData);

            //Create the job scheduler, check for errors
            int result = pthread_create(&_requesterData->_pthread, NULL, requesterJob, _requesterData);
            if (result)// Couldn't create the new thread
                return 1;

            sleep(rand() % (_configuration.maximumIssuingTime+1) + _configuration.minimumIssuingTime);
        }
    } else{
        //These I use to get the line from the file
        FILE * inputFile = fopen("processes.txt", "r");
        char * line = NULL;
        size_t len = 256;

        //While there is file to read, keep reading
        while ((valread = getline(&line, &len, inputFile)) != -1) {
            //Got the line, I'll copy it into the buffer

            line[valread - 1] = 0;//Add a null to the end of the line

            //First I'll try and recover any usable memory
            deleteDone(&_requesterDataList);

            //Will block until it can get enough memory for more requesters' data
            do {
                _requesterData = malloc(sizeof(struct requesterData));
            } while (_requesterData == NULL);

            _requesterData->isDone = 0;//It's not done
            _requesterData->_configuration = &_configuration;
            _requesterData->threadId = threadCount++;
            _requesterData->line = line;

            //So I can get a new allocated line next time I use getline
            line = NULL;

            //Add it to the ongoing threads
            insertNewThread(&_requesterDataList, _requesterData);

            //Create the job scheduler, check for errors
            int result = pthread_create(&_requesterData->_pthread, NULL, requesterJob, _requesterData);
            if (result)// Couldn't create the new thread
                return 1;

            sleep(rand() % 9 + 3);
        }
    }
    //I need to signal this process to wait until the last process is done
    int result;

    //While there are nodes in requesterDataList
    while (_requesterDataList.firstNode != NULL){
        result = pthread_join(_requesterDataList.firstNode->node->_pthread, NULL);
        if (result){
            return 1;
        }
        if (_requesterDataList.firstNode->node->isDone){//Make sure it is flagged as done
            deleteDone(&_requesterDataList);//Remove this, and hopefully other done threads
        }
    }


    //Make sure to clear this before leaving, although it's not necessary since the process is about to end
    clearList(&_requesterDataList);

    return 0;
}
