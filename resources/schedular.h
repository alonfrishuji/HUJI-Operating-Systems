#include <queue>
#include <vector>
#include <map>
#include <sys/time.h>
#include <signal.h>
#include "thread.h"

typedef void (*turn_end_func)(int);

class Schedular {
    public:
    Schedular(int maxThreads, int stackSize, int quantum_usecs, turn_end_func turnEnd);
    int createThread(int id, thread_entry_point entry_point = nullptr);
    int uthreadSpawn(thread_entry_point entry_point);
    int uthreadTerminate(int tid);
    int uthreadBlock(int tid);
    int uthreadResume(int tid);
    int uthreadSleep(int num_quantums);
    int uthreadGetRid();
    int uthreadGetTotalQuantums();
    int uthreadGetQuantums(int tid);

    Thread *getCurrentThread() {return currentThread;};
    Thread *getThread(int tid) {return threads[tid];};
    void switchTurn(bool addToReady);

    struct itimerval timer;


    private:
    int stackSize;
    int quantum_usecs;
    std::queue<Thread*> readyQueue;
    std::map<int, Thread*> threads;
    int totalQuantum;
    struct sigaction timeAction;
    Thread *currentThread;
    std::priority_queue<int, std::vector<int>, std::greater<int>> available_tid;
    void yield(Thread* newThread);
    void startTimer();
};