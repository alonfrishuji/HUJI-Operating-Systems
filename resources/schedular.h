#include <queue>
#include <map>
#include <sys/time.h>
#include <signal.h>
#include "thread.h"

class Schedular {
    public:
    int createThread(int id, int stackSize, char *stack = nullptr, thread_entry_point entry_point = nullptr);
    int uthreadSpawn(thread_entry_point entry_point);
    int uthreadTerminate(int tid);
    int uthreadBlock(int tid);
    int uthreadResume(int tid);
    int uthreadSleep(int num_quantums);
    int uthreadGetRid();
    int uthreadGetTotalQuantums();
    int uthreadGetQuantums(int tid);


    private:
    std::queue<Thread*> readyQueue;
    std::map<int, Thread*> threads;
    int totalQuantum;
    struct itimerval timer;
    struct sigaction timeAction;
    Thread *currentThread;

};