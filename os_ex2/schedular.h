#include <queue>
#include <vector>
#include <map>
#include <sys/time.h>
#include <signal.h>
#include "thread.h"

typedef void (*timer_end_func)(int);

class Schedular {
    public:
    Schedular(int maxThreads, int stackSize, int quantum_usecs, timer_end_func turnEnd);
    void createThread(int id, thread_entry_point entry_point = nullptr);
    int uthreadSpawn(thread_entry_point entry_point);
    int uthreadTerminate(int tid);
    int uthreadBlock(int tid);
    int uthreadResume(int tid);
    int uthreadSleep(int num_quantums);
    int uthreadGetRid();
    int uthreadGetTotalQuantums();
    int uthreadGetQuantums(int tid);
    int get_quantums(int tid);
    Thread *getCurrentThread() {return currentThread;};
    Thread *getThread(int tid) {return threads[tid];};
    void switchTurn();
    int blockThread(int tid);
    int resumeThread(int tid);
    int getTotalQuantum();
    int threadSleep(int num_quantums);


    private:
    int stackSize;
    int quantum_usecs;
    std::queue<Thread*> *readyQueue;
    std::map<int, Thread*> threads;
    std::map<int, std::vector<Thread*>*> sleepingThreads;
    int totalQuantum;
    struct sigaction timeAction;
    Thread *currentThread;
    std::priority_queue<int, std::vector<int>, std::greater<int>> available_tid;
    struct itimerval timer;
    void startTurn();
    void removeFromReady(Thread *threadToRemove);
    void checkWakeUp();
    void addToReady(Thread *thread);
    Thread* popFromReady();
};