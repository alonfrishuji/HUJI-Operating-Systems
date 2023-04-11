#include "schedular.h"


int Schedular::createThread(int id, int stackSize, char *stack, thread_entry_point entry_point) { 
    Thread *thread = new Thread(id, stackSize, stack, entry_point);
    threads[id] = thread;
    readyQueue.push(thread);
    return 0;
}