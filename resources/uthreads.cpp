#include <iostream>
#include "uthreads.h"
#include "schedular.h"
#include <sys/time.h>
#include <unistd.h>

#define SECOND 1000000

Schedular* schedular;

void endTurn(int sig) {
    std::cout << "reached 1" << std::endl;
    schedular->switchTurn(true);
}

int uthread_init(int quantum_usecs) {
    /*init of schedular, creating main thread and adding to queue*/
    schedular = new Schedular(MAX_THREAD_NUM, STACK_SIZE, quantum_usecs, &endTurn);
    schedular->createThread(0);
    return 0;
}

int uthread_spawn(thread_entry_point entry_point) {
    return schedular->uthreadSpawn(entry_point);
}

void test1() {
    // for (;;) {
    //     usleep(SECOND);
    //     std::cout << "test 1" << std::endl;
    // }
    for (int i = 0; i > -1; i++) {
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
    for (int i = 0; i > -1; i++) {
        if (i % 5000000 == 0) {
            std::cout << "test 2" << std::endl;
        }
    }
}


int main(int argc, char const *argv[])
{
    /* code */
    uthread_init(500000);
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

