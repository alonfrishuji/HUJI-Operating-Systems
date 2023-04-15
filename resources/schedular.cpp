#include "schedular.h"


Schedular::Schedular(int maxThreads, int stackSize, int quantum_usecs, timer_end_func turnEnd_): 
    stackSize(stackSize), 
    quantum_usecs(quantum_usecs),
    totalQuantum(0) {
    readyQueue = new std::queue<Thread*>();
    for (int i=1; i < maxThreads; i++) {
        available_tid.push(i);
    }
    timeAction = {0};
    timeAction.sa_handler = turnEnd_;
    if (sigaction(SIGVTALRM, &timeAction, NULL) < 0) {
        exit_errno();
    };
}


void Schedular::createThread(int id, thread_entry_point entry_point) { 
    Thread *thread = new Thread(id, stackSize, entry_point);
    threads[id] = thread;
    addToReady(thread);
    if (id == 0) {
        startTurn();
    }
}


int Schedular::uthreadSpawn(thread_entry_point entry_point) {
    if (available_tid.empty() || entry_point == nullptr) {
        return -1;
    }
    int new_tid = available_tid.top();
    available_tid.pop();
    createThread(new_tid, entry_point);
    return new_tid;
}


void Schedular::switchTurn() {
    currentThread->setRunning(false);
    if (!currentThread->getBlocked() && !currentThread->getSleeping()) {
        addToReady(currentThread);
    }
    int ret_val = sigsetjmp(currentThread->env, 1);
    bool did_just_save_bookmark = ret_val == 0;
    if (did_just_save_bookmark) {
        startTurn();
        siglongjmp(currentThread->env, 1);
    }
}


void Schedular::startTurn() {
    totalQuantum++;
    checkWakeUp();
    currentThread = popFromReady();
    currentThread->setRunning(true);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        exit_errno();
    }
}


void Schedular::checkWakeUp() {
    if (sleepingThreads.count(totalQuantum)) {
        std::vector<Thread*>* wakeUpThreads = sleepingThreads[totalQuantum];
        for (Thread* thread: *wakeUpThreads) {
            thread->setSleeping(false);
            if (!thread->getBlocked()) {
                addToReady(thread);
            }
        }
        delete wakeUpThreads;
        sleepingThreads.erase(totalQuantum);
    }
}


int Schedular::blockThread(int tid) {
    if (tid == 0 || !threads.count(tid)) {
        return -1;
    }
    Thread *thread = threads[tid];
    if (thread->getReady()) {
        removeFromReady(thread);
    }
    thread->setBlocked(true);
    if (thread->getRunning()) {
        switchTurn();
    }
    return 0;
}


int Schedular::resumeThread(int tid) {
    if (!threads.count(tid)) {
        return -1;
    }
    Thread *thread = threads[tid];
    if (thread->getBlocked()) {
        thread->setBlocked(false);
        if (!thread->getSleeping()) {
            addToReady(thread);
        }   
    }
    return 0;
}


void Schedular::removeFromReady(Thread *threadToRemove) {
    threadToRemove->setReady(false);
    std::queue<Thread*> *newQueue = new std::queue<Thread*>();
    while (!readyQueue->empty()) {
        Thread *curThread = readyQueue->front();
        readyQueue->pop();
        if (curThread->id != threadToRemove->id) {
            newQueue->push(curThread);
        }
    }
    delete readyQueue;
    readyQueue = newQueue;
}


void Schedular::addToReady(Thread *thread) {
    readyQueue->push(thread);
    thread->setReady(true);
}


Thread *Schedular::popFromReady() {
    Thread *thread = readyQueue->front();
    readyQueue->pop();
    thread->setReady(false);
    return thread;
}


int Schedular::getTotalQuantum() {
    return totalQuantum;
}


int Schedular::threadSleep(int num_quantums) {
    if (currentThread->id == 0) {
        return -1;
    }
    currentThread->setSleeping(true);
    int wakeUpTurn = totalQuantum + num_quantums;
    if (!sleepingThreads.count(wakeUpTurn)) {
        std::vector<Thread*> *wakeUpThreads = new std::vector<Thread*>();
        sleepingThreads[wakeUpTurn] = wakeUpThreads;
    }
    sleepingThreads[wakeUpTurn]->push_back(currentThread);
    switchTurn();
    return 0;
}