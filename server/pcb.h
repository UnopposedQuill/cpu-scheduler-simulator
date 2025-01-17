//
// Created by esteban on 17/10/20.
//

#ifndef SERVER_PCB_H
#define SERVER_PCB_H

struct pcb{
    unsigned int pid, burst, priority, progress, tickOfEntry, tickOfCompletion;
};

struct pcbNode{
    struct pcb * node;
    struct pcbNode * next, * previous;
};

struct pcbList{
    struct pcbNode * firstNode;
    unsigned int len;
};

struct pcb * createPcb(unsigned int pid, unsigned int burst, unsigned int priority, unsigned int tick);
void destroyPcb(struct pcb * _pcb);
struct pcbNode * insertNewPcb(struct pcbList * _pcbList, struct pcb * _pcb);
struct pcbNode * removePcbPid(struct pcbList * _pcbList, unsigned int pid);
int clearList(struct pcbList * _pcbList);
void printList(struct pcbList * _pcbList);

#endif //SERVER_PCB_H
