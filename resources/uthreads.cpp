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

int uthread_init(int quantum_usecs) {
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
    
    return 0;
}

int uthread_spawn(thread_entry_point entry_point) {
    int ret = -1;
    try {
        ret = schedular->uthreadSpawn(entry_point);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    return ret;
}

int uthread_block(int tid) {
    int ret = -1;
    try {
        ret = schedular->blockThread(tid);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    return ret;
}

int uthread_resume(int tid) {
    return schedular->resumeThread(tid);
}


int uthread_get_total_quantums() {
    return schedular->getTotalQuantum();
}


int uthread_sleep(int num_quantums) {
    int ret = -1;
    try {
        ret = schedular->threadSleep(num_quantums);
    }
    catch (std::bad_alloc&) {
        exit_system_error("memory allocation failure");
    }
    return ret;
}


void test1() {
    // for (;;) {
    //     usleep(SECOND);
    //     std::cout << "test 1" << std::endl;
    // }
    for (int i = 0; i < 5000000 * 90; i++) {
        if (i % 5000000 == 0) {
            std::cout << "test 1" << std::endl;
        }
    }
}

void test2() {
    // for (;;) {
    //     usleep(SECOND);
    //     std::cout << "test 2" << std::endl;
    // }
    for (int i = 0; i < 5000000 * 90; i++) {
        if (i % 5000000 == 0) {
            std::cout << "test 2" << std::endl;
        }
        if (i == 5000000 * 60) {
            std::cout << "----------------------------------------------------------------------------------" << std::endl;
            std::cout << "blocked " << uthread_block(1);
            std::cout << "----------------------------------------------------------------------------------" << std::endl;
            std::cout << "resumed " << uthread_resume(1);            
        }
    }
}


int main(int argc, char const *argv[])
{
    /* code */
    uthread_init(50000);
    uthread_spawn(test1);
    test2();
    // // test2();
    // // return 0;
    // struct itimerval timer;
    // timer.it_value.tv_sec = 1;        // first time interval, seconds part
    // timer.it_value.tv_usec = 0;        // first time interval, microseconds part

    // // configure the timer to expire every 3 sec after that.
    // timer.it_interval.tv_sec = 3;    // following time intervals, seconds part
    // timer.it_interval.tv_usec = 0;    // following time intervals, microseconds part
    // if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    // {
    //     printf("setitimer error.");
    // }
    // for (int i = 0; i < 100000000000; i++) {
    //     if (i % 10000) {
    //         std::cout << i << std::endl;
    //     }
    //     std::cout << timer.it_value.tv_sec << " " << timer.it_value.tv_usec << std::endl;
    // }
}


// int gotit = 0;


// void timer_handler1(int sig)
// {
//     gotit = 1;
//     printf("Timer expired\n");
// }


// int main(void)
// {
//     struct sigaction sa = {0};
//     struct itimerval timer;

//     // Install timer_handler as the signal handler for SIGVTALRM.
//     sa.sa_handler = &timer_handler1;
//     if (sigaction(SIGVTALRM, &sa, NULL) < 0)
//     {
//         printf("sigaction error.");
//     }

//     // Configure the timer to expire after 1 sec... */
//     timer.it_value.tv_sec = 1;        // first time interval, seconds part
//     timer.it_value.tv_usec = 0;        // first time interval, microseconds part

//     // configure the timer to expire every 3 sec after that.
//     // timer.it_interval.tv_sec = 3;    // following time intervals, seconds part
//     // timer.it_interval.tv_usec = 0;    // following time intervals, microseconds part

//     // Start a virtual timer. It counts down whenever this process is executing.
//     if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
//     {
//         printf("setitimer error.");
//     }

//     for (;;)
//     {
//         if (gotit)
//         {
//             printf("Got it!\n");
//             gotit = 0;
//         }
//     }
// }

