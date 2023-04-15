#include "schedular.h"
#include <iostream>


Schedular::Schedular(int maxThreads, int stackSize, int quantum_usecs, turn_end_func turnEnd_): 
    stackSize(stackSize), 
    quantum_usecs(quantum_usecs) {
    for (int i=1; i < maxThreads; i++) {
        available_tid.push(i);
    }
    timeAction = {0};
    timeAction.sa_handler = turnEnd_;
    sigaction(SIGVTALRM, &timeAction, NULL);
}


int Schedular::createThread(int id, thread_entry_point entry_point) { 
    Thread *thread = new Thread(id, stackSize, entry_point);
    threads[id] = thread;
    if (id == 0) {
        currentThread = thread;
        std::cout << "started timer" << std::endl;
        startTimer();
    }
    else {
        readyQueue.push(thread);
    }
    return 0;
}


int Schedular::uthreadSpawn(thread_entry_point entry_point) {
    if (available_tid.empty()) {
        return -1;
    }
    int new_thid = available_tid.top();
    available_tid.pop();
    return createThread(new_thid, entry_point);
}


void Schedular::switchTurn(bool addToReady) {
    if (addToReady) {
        readyQueue.push(currentThread);
    }
    Thread* newThread = readyQueue.front();
    readyQueue.pop();
    yield(newThread);
}


void Schedular::yield(Thread* newThread) {
    int ret_val = sigsetjmp(currentThread->env, 1);
    bool did_just_save_bookmark = ret_val == 0;
    if (did_just_save_bookmark) {
        currentThread = newThread;
        startTimer();
        siglongjmp(currentThread->env, 1);
    }
}


void Schedular::startTimer() {
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;
    int ret = setitimer(ITIMER_VIRTUAL, &timer, NULL);
}