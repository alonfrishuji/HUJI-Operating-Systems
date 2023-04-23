#include <iostream>
#include "uthreads.h"
#include "schedular.h"
#include <sys/time.h>
#include <unistd.h>

#define SECOND 1000000

Schedular* schedular;

void exit_system_error(std::string msg) {
    std::cerr << "system error: " << msg << std::endl;
    exit(1);
}

void endTurn(int sig) {
    schedular->switchTurn();   
}


void blockTimer() {
    sigset_t mask;
    if (sigemptyset(&mask) == -1) {
        exit_errno(); 
    }
    if (sigaddset(&mask, SIGVTALRM) == -1) {
        exit_errno();
    }
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        exit_errno();
    }
}


void unblockTimer() {
    sigset_t mask;
    if (sigemptyset(&mask) == -1) {
        exit_errno();
    }
    if (sigaddset(&mask, SIGVTALRM) == -1) {
        exit_errno();
    }
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        exit_errno();
    }
}


int uthread_init(int quantum_usecs) {
    blockTimer();
    int ret = -1;
    if (quantum_usecs <= 0) {
        std::cerr << "quantum_usecs must be positive" << std::endl;
        ret = -1;
    }
    else {
        /*init of schedular, creating main thread and adding to queue*/
        try {
            schedular = new Schedular(MAX_THREAD_NUM, STACK_SIZE, quantum_usecs, &endTurn);
            schedular->createThread(0);
        }
        catch (std::bad_alloc&) {
            exit_system_error("memory allocation failure");
        } 
        ret = 0;
    }
    unblockTimer();
    return ret;
}

int uthread_spawn(thread_entry_point entry_point) {
    blockTimer();
    int ret = -1;
    try {
        ret = schedular->uthreadSpawn(entry_point);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    unblockTimer();
    return ret;
}

int uthread_block(int tid) {
    blockTimer();
    int ret = -1;
    try {
        ret = schedular->blockThread(tid);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    unblockTimer();    
    return ret;
}

int uthread_resume(int tid) {
    blockTimer();
    int ret = schedular->resumeThread(tid);
    unblockTimer();
    return ret;
}


int uthread_get_total_quantums() {
    blockTimer();
    int ret = schedular->getTotalQuantum();
    unblockTimer();
    return ret;
}


int uthread_sleep(int num_quantums) {
    blockTimer();
    int ret = -1;
    try {
        ret = schedular->threadSleep(num_quantums);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    unblockTimer();
    return ret;
}


int uthread_get_quantums(int tid) {
    blockTimer();
    int ret = -1;
    ret = schedular->get_quantums(tid);
    unblockTimer();
    return ret;
}


int uthread_get_tid(){
    blockTimer();
    int ret = schedular->getCurrentThread()->id;
    unblockTimer();
    return ret;
}


int uthread_terminate(int tid) {
    blockTimer();
    int ret = -1;
    try {
        ret = schedular->terminateThread(tid);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    if (tid == 0) {
        delete schedular;
        exit(0);
    }
    unblockTimer();
    return ret;
}


int main(int argc, char const *argv[])
{
    return 0;
}

