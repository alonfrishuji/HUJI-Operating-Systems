#include <iostream>
#include "uthreads.h"
#include "schedular.h"

Schedular* schedular;

int uthread_init(int quantum_usecs) {
    /*init of schedular, creating main thread and adding to queue*/
    schedular = new Schedular();
    schedular->createThread(0, STACK_SIZE);
    return 0;
}


int main(int argc, char const *argv[])
{
    /* code */
    uthread_init(1000);
    return 0;
}
