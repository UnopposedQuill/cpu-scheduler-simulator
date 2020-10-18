//
// Created by esteban on 17/10/20.
//

#ifndef SERVER_PCB_H
#define SERVER_PCB_H

struct pcb{
    unsigned int pid, burst, priority, progress;
};

struct pcbNode{
    struct pcb * node;
    struct pcbNode * next, * previous;
};

struct pcbList{
    struct pcbNode * firstNode;
    int len;
};

struct pcb * createPcb(unsigned int pid, unsigned int burst, unsigned int priority);
void destroyPcb(struct pcb * _pcb);
struct pcbNode * insertNewPcb(struct pcbList * _pcbList, struct pcb * _pcb);
struct pcbNode * removePcbPid(struct pcbList * _pcbList, int pid);
int clearList(struct pcbList * _pcbList);

#endif //SERVER_PCB_H
