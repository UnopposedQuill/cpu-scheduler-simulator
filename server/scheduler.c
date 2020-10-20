//
// Created by esteban on 18/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "scheduler.h"
#include "pcb.h"

void * clockTickerWork(void * arguments){
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;
    while (_schedulerInfo->working){
        sleep(1);
        _schedulerInfo->tick++;
    }
    return NULL;
}

void * jobSchedulerWork(void * arguments){
    printf("Job scheduler initialized\n");
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;

    // <editor-fold defaultstate=collapsed desc="Socket preparation">
    //Now I need to setup the serving socket
    /*
     * First I create the handles for the sockets I'll use:
     * server_fd: Server's main socket, will take care of listening to requests
     * new_socket: Handler for the incoming request, will be used to setup the new communication channel
     */
    int server_fd, new_socket, valread;
    int opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[256];memset(buffer, 0, 256);//Create and clear the buffer

    //First, I create the handle for the server socket
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        //Ocurrió un error al crear el socket
        perror("Socket creation error");
        _schedulerInfo->working = 0;
        return NULL;
    }
    //Successful creation, now I'll signal the OS to reuse the address, so I don't get too much trouble in case I don't
    //get to close any server socket
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0){
        perror("SO couldn't be instructed to reuse the socket address and port");
        _schedulerInfo->working = 0;
        return NULL;
    }

    /*
     * Now I can set the IP, ports and protocols from which I desire to read:
     * sin_family: Read ipv4 requests
     * sin_addr.s_addr: Read from any IP address (can be used as a filter)
     * sin_port: The port, read from the configuration file
     */
    //En este punto ya es seguro colocar la ip, puertos y protocolos de los que deseo leer
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_schedulerInfo->_configuration->port);

    //I need to bind the socket & address
    if(bind(server_fd, (struct sockaddr*) &address, (socklen_t) addrlen) < 0){
        perror("Socket couldn't be bound to specified address, port or protocols");
        _schedulerInfo->working = 0;
        return NULL;
    }

    //Now I signal the OS to start listening to said port, and keep these maximum pending connections
    if(listen(server_fd, _schedulerInfo->_configuration->maximumPendingConnections) < 0){
        perror("Socket couldn't be signaled to listen");
        _schedulerInfo->working = 0;
        return NULL;
    }

    if((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0){
        //No pudo aceptarla
        perror("\nError upon accepting a new connection");
        close(new_socket);
        _schedulerInfo->working = 0;
        return NULL;
    }

    // </editor-fold>

    // <editor-fold defaultstate=collapsed desc="Socket serving cycle">
    while (_schedulerInfo->working){
        //I'll try to read the line which is in format "burst,priority\n" from the socket
        if ((valread = recv(new_socket, buffer, 256, 0)) < 0){
            perror("Error upon reading new request, client probably disconnected, finalizing job scheduler");
            close(new_socket);
            return NULL;
        }
        //Now I need to convert them
        char * burst, * priority;
        //Use strtok to separate both values
        burst = strtok(buffer, ",");
        priority = strtok(NULL, ",");

        //Now I can use strtol to convert them into long, which I cast into unsigned integers
        struct pcbNode * inserted = insertNewPcb(_schedulerInfo->readyList,
                                            createPcb(_schedulerInfo->lastPid++, (unsigned int)strtol(burst, NULL, 10),
                                                    (unsigned int)strtol(priority, NULL, 10), _schedulerInfo->tick));

        printf("Added new process to ready queue pid: %d, burst: %d, priority: %d, tick of entry: %d\n", inserted->node->pid, inserted->node->burst, inserted->node->priority, inserted->node->tickOfEntry);
        //Now I have to reply the Pid assigned
        memset(buffer, 0, 256);
        sprintf(buffer, "%d", inserted->node->pid);

        //Check for errors after sending
        if ((valread = send(new_socket, buffer, 256, 0)) < 0){
            perror("Error upon reading new request, client probably disconnected, finalizing job scheduler");
            close(new_socket);
            return NULL;
        }
    }

    // </editor-fold>

    return NULL;
}

void * cpuSchedulerWork(void * arguments){
    printf("Cpu Scheduler initialized\n");
    struct schedulerInfo * _schedulerInfo = (struct schedulerInfo *) arguments;
    //While it is allowed to keep working
    while (_schedulerInfo->working){
        printf("Selecting new process to execute\n");
        struct pcbNode * scheduledPcb = schedule(_schedulerInfo);
        if (scheduledPcb == NULL){//Didn't find any work to do, get into idle mode
            printf("No ready jobs in queue\n");
            _schedulerInfo->idleTicks++;
            sleep(1);
        }
        else{ //There's work I can do
            printf("Job selected: %d\n", scheduledPcb->node->pid);
            _schedulerInfo->currentProcess = scheduledPcb;//I design the found pcb as the one it's being worked
            if (    _schedulerInfo->_configuration->schedulerType != ASJF &&
                    _schedulerInfo->_configuration->schedulerType != AHPF &&
                    _schedulerInfo->_configuration->schedulerType != ROUNDROBIN){
                sleep(_schedulerInfo->currentProcess->node->burst);
                _schedulerInfo->currentProcess->node->progress = _schedulerInfo->currentProcess->node->burst;
                _schedulerInfo->currentProcess->node->tickOfCompletion = _schedulerInfo->tick;
                printf("Completed job: %d\n", _schedulerInfo->currentProcess->node->pid);
                removePcbPid(_schedulerInfo->readyList, scheduledPcb->node->pid);
                free(scheduledPcb);//Discard the wrapper
                insertNewPcb(_schedulerInfo->doneList, _schedulerInfo->currentProcess->node);//I completed it, add it to done
            }
            else {
                unsigned int amountOfWork = 1;
                if (_schedulerInfo->_configuration->schedulerType == ROUNDROBIN){
                    amountOfWork = _schedulerInfo->_configuration->quantum < scheduledPcb->node->burst - scheduledPcb->node->progress ?
                            _schedulerInfo->_configuration->quantum : scheduledPcb->node->burst - scheduledPcb->node->progress;
                }
                sleep(amountOfWork);
                scheduledPcb->node->progress += amountOfWork;
                if (scheduledPcb->node->progress == scheduledPcb->node->burst){
                    _schedulerInfo->currentProcess->node->tickOfCompletion = _schedulerInfo->tick;
                    printf("Completed job: %d\n", _schedulerInfo->currentProcess->node->pid);
                    free(scheduledPcb);//Discard the wrapper
                    insertNewPcb(_schedulerInfo->doneList, _schedulerInfo->currentProcess->node);//I completed it, add it to done
                }
            }
        }
    }
    return NULL;
}

struct pcbNode * schedule(struct schedulerInfo * _schedulerInfo){

    /**
     * I'll employ a strategy pattern here, depending on the value of the scheduler type
     */
    switch (_schedulerInfo->_configuration->schedulerType) {
        case FIFO:{//I place the last received one in last place, so I just pick the first one
            return _schedulerInfo->readyList->firstNode;
        }
        case ASJF://I'll recycle this, since what matters is that it may be interrupting while working, not when choosing
        case SJF:{
            //First will be used to go through the list, second will be used to store the best one
            struct pcbNode * ptr = _schedulerInfo->readyList->firstNode, * bestPcb = NULL;
            while (ptr->node != NULL){
                //I found a better pcb to target next
                if (bestPcb == NULL || ptr->node->burst - ptr->node->progress < bestPcb->node->burst - bestPcb->node->progress){
                    bestPcb = ptr;
                }
                ptr = ptr->next;
            }
            return bestPcb;
        }
        case AHPF:
        case HPF:{
            //First will be used to go through the list, second will be used to store the best one
            struct pcbNode * ptr = _schedulerInfo->readyList->firstNode, * bestPcb = NULL;
            while (ptr->node != NULL){
                //I found a better pcb to target next
                if (bestPcb == NULL || ptr->node->priority < bestPcb->node->priority){
                    bestPcb = ptr;
                }
                ptr = ptr->next;
            }
            return bestPcb;
        }
        case ROUNDROBIN:{
            if (_schedulerInfo->currentProcess != NULL && _schedulerInfo->currentProcess->next != NULL){
                return _schedulerInfo->currentProcess->next;
            } else{
                return _schedulerInfo->readyList->firstNode;
            }
        }
    }
    return NULL;
}

void showStatistics(struct schedulerInfo * _schedulerInfo){
    printf("\n\n----------------------------------------\nStatistics: ");
    switch (_schedulerInfo->_configuration->schedulerType) {
        case FIFO:{
            printf("FIFO");
            break;
        }
        case SJF:{
            printf("SJF");
            break;
        }
        case HPF:{
            printf("HPF");
            break;
        }
        case ROUNDROBIN:{
            printf("Round Robin");
            break;
        }
        case ASJF:{
            printf("Appropiative SJF");
            break;
        }
        case AHPF:{
            printf("Appropiative HPF");
            break;
        }
    }
    printf(" Scheduler\nIdle Time: %d\nProcesses completed: %d\nProcesses still ready: %d\n\nDetailed per-process statistics:\n", _schedulerInfo->idleTicks, _schedulerInfo->doneList->len, _schedulerInfo->readyList->len);

    struct pcbNode * ptr = _schedulerInfo->doneList->firstNode;
    double averageTurnAroundTime = 0, averageWaitingTime = 0;

    while (ptr != NULL && ptr->node != NULL){//No wrapper should have it's node NULL, but I prefer to prevent errors
        unsigned int turnaroundTime = ptr->node->tickOfCompletion - ptr->node->tickOfEntry;
        printf("Process pid: %d, burst: %d, tick of entry: %d, tick of completion: %d, turn-around time: %d, waiting time: %d\n",
               ptr->node->pid, ptr->node->burst, ptr->node->tickOfEntry, ptr->node->tickOfCompletion, turnaroundTime, turnaroundTime - ptr->node->burst);

        averageTurnAroundTime += turnaroundTime;
        averageWaitingTime += (turnaroundTime - ptr->node->burst);
        ptr = ptr->next;
    }

    printf("\nAverage Turn-around time: %f, average waiting time: %f", averageTurnAroundTime / _schedulerInfo->doneList->len, averageWaitingTime / _schedulerInfo->doneList->len);
}
