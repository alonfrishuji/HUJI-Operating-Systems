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
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}


void unblockTimer() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}


int uthread_init(int quantum_usecs) {
    blockTimer();
    if (quantum_usecs <= 0) {
        return -1;
    }
    /*init of schedular, creating main thread and adding to queue*/
    try {
        schedular = new Schedular(MAX_THREAD_NUM, STACK_SIZE, quantum_usecs, &endTurn);
        schedular->createThread(0);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    } 
    unblockTimer();
    return 0;
}

int uthread_spawn(thread_entry_point entry_point) {
    blockTimer();
    sigset_t current_mask;

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
    int ret = schedular->terminateThread(tid);
    if (tid == 0) {
        delete schedular;
        exit(0);
    }
    unblockTimer();
    return ret;
}


void test1() {
    int lastQuantum = 0;
    bool slept = false;
    bool terminate = false;
    for (int i = 0; i < 5000000 * 30; i++) {
        int curQuantum = uthread_get_quantums(uthread_get_tid());
        if (curQuantum > lastQuantum) {
            std::cout << "test 1 quantum: " << curQuantum << " , total quantum: " << uthread_get_total_quantums() << std::endl;
            lastQuantum = curQuantum;
        }
        // if (curQuantum == 12 && !slept) {
        //     slept == true;
        //     std::cout << "----------------------------------start sleeping 1--------------------------" << std::endl;
        //     uthread_sleep(20);
        //     std::cout << "----------------------------------end sleeping 1--------------------------" << uthread_resume(1);            
        // }        
    }
}


void test2() {
    bool slept = false;
    int lastQuantum = 0;
    bool terminate = false;
    for (int i = 0; i < 5000000 * 30; i++) {
        int curQuantum = uthread_get_quantums(uthread_get_tid());
        if (curQuantum > lastQuantum) {
            std::cout << "test 2 quantum: " << curQuantum << " , total quantum: " << uthread_get_total_quantums() << std::endl;
            lastQuantum = curQuantum;
        }   
        if (curQuantum == 12 && !slept) {
            slept == true;
            std::cout << "----------------------------------start sleeping 2--------------------------" << std::endl;
            uthread_sleep(19);
            std::cout << "----------------------------------end sleeping 2--------------------------" << uthread_resume(1);            
        }
    }
}


void test0() {
    int lastQuantum = 0;
    bool terminate = false;
    for (int i = 0; i < 5000000 * 30; i++) {
        int curQuantum = uthread_get_quantums(uthread_get_tid());
        if (curQuantum > lastQuantum) {
            std::cout << "test 0 quantum: " << curQuantum << " , total quantum: " << uthread_get_total_quantums() << std::endl;
            lastQuantum = curQuantum;
        }       
        if (lastQuantum >= 45) {
        exit(0);
        }
        if (lastQuantum == 18 && !terminate) {
            std::cout << "-------------------- terminate 0 from 1" << "-------------------" << std::endl;
            terminate = true;
            uthread_terminate(1);
            uthread_spawn(test1);
        }
    }
}


int main(int argc, char const *argv[])
{
    /* code */
    uthread_init(50000);
    uthread_spawn(test1);
    // uthread_spawn(test2);
    // uthread_spawn(test2);
    // uthread_spawn(test3);
    test0();
}


