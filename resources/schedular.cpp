#include "schedular.h"
// #include <cassert>

std::string LIBRARY_ERROR_MSG = "thread library error: ";

Schedular::Schedular(int maxThreads, int stackSize, int quantum_usecs, timer_end_func turnEnd_): 
    stackSize(stackSize), 
    quantum_usecs(quantum_usecs),
    totalQuantum(0),
    deleteMode(false),
    tidToDelete(-1)
    {
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
        startTurn(false);
    }
}


int Schedular::uthreadSpawn(thread_entry_point entry_point) {
    if (available_tid.empty()) {
        std::cerr << LIBRARY_ERROR_MSG << "reached max threads" << std::endl;
        return -1;
    }
    if (entry_point == nullptr) {
        std::cerr << LIBRARY_ERROR_MSG << "invalid entry point" << std::endl;
        return -1;
    }
    int new_tid = available_tid.top();
    available_tid.pop();
    createThread(new_tid, entry_point);
    return new_tid;
}


void Schedular::switchTurn() {
    bool did_just_save_bookmark = false;
    if (currentThread != nullptr) {
        currentThread->setRunning(false);
        if (!currentThread->getBlocked() && !currentThread->getSleeping()) {
            addToReady(currentThread);
        }
        int ret_val = sigsetjmp(currentThread->env, 1);
        did_just_save_bookmark = ret_val == 0;
    }
    if (deleteMode) {
        // assert(!did_just_save_bookmark);
        deleteByMain();
    }
    if (did_just_save_bookmark || currentThread == nullptr) {
        startTurn(true);
    }
}


void Schedular::startTurn(bool shouldJump) {
    totalQuantum++;
    checkWakeUp();
    currentThread = popFromReady();
    currentThread->setRunning(true);
    currentThread->quantomCount++;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        exit_errno();
    }
    if (shouldJump) {
        siglongjmp(currentThread->env, 1);
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
    if (tid == 0) {
        std::cerr << LIBRARY_ERROR_MSG << "cant block main thread" << std::endl;
        return -1;
    }
    if (!threads.count(tid)) {
        std::cerr << LIBRARY_ERROR_MSG << "tid not found" << std::endl;
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
        std::cerr << LIBRARY_ERROR_MSG << "tid not found" << std::endl;
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
        std::cerr << LIBRARY_ERROR_MSG << "cant sleep if main thread" << std::endl;
        return -1;
    }
    if (num_quantums <= 0) {
        std::cerr << LIBRARY_ERROR_MSG << "non positive num_quantums" << std::endl;
        return -1;
    }
    currentThread->setSleeping(true);
    int wakeUpTurn = totalQuantum + num_quantums;
    currentThread->wakeUpTurn = wakeUpTurn;
    if (!sleepingThreads.count(wakeUpTurn)) {
        std::vector<Thread*> *wakeUpThreads = new std::vector<Thread*>();
        sleepingThreads[wakeUpTurn] = wakeUpThreads;
    }
    sleepingThreads[wakeUpTurn]->push_back(currentThread);
    switchTurn();
    return 0;
}


int Schedular::get_quantums(int tid) {
    if (!threads.count(tid)){
        std::cerr << LIBRARY_ERROR_MSG << "tid not found" << std::endl;
        return -1;
    }
    Thread* t = threads[tid];
    return t->quantomCount;
}


void Schedular::deleteSleeping(Thread *thread) {
    std::vector<Thread*>* wakeUpThreads = sleepingThreads[thread->wakeUpTurn];
    if (wakeUpThreads->size() == 1) {
        delete wakeUpThreads;
        sleepingThreads.erase(thread->wakeUpTurn);
    }
    else {
        for (size_t idx = 0; idx < wakeUpThreads->size(); idx++) {
            if ((*wakeUpThreads)[idx] == thread) {
                wakeUpThreads->erase(wakeUpThreads->begin() + idx);
            }
        }
    }
}


int Schedular::terminateThread(int tid) {
    if (tid != 0 && tid != currentThread->id) {
        return deleteThread(tid);
    }
    else if (tid == 0) {
        terminateAllThreads();
        return 0;
    }
    else {
        deleteSelf(tid);
        return 0;
    }
}


int Schedular::deleteThread(int tid) {
    if (!threads.count(tid)) {
        std::cerr << LIBRARY_ERROR_MSG << "tid not found" << std::endl;
        return -1;
    }
    Thread *thread = threads[tid];
    if (thread->getReady()) {
        removeFromReady(thread);
    }
    if (thread->getSleeping()) {
        deleteSleeping(thread);
    }
    available_tid.push(tid);
    threads.erase(tid);
    delete thread;
    return 0;
}


void Schedular::terminateAllThreads() {
    std::vector<int> allTids;
    for (auto const& imap: threads) {
        allTids.push_back(imap.first);
    }
    for (int tid: allTids) {
        deleteThread(tid);
    }
    delete readyQueue;
}


void Schedular::deleteSelf(int tid) {
    deleteMode = true;
    tidToDelete = tid;
    siglongjmp(threads[0]->env, 1);
}


void Schedular::deleteByMain() {
    deleteThread(tidToDelete);
    deleteMode = false;
    tidToDelete = -1;
    currentThread = nullptr;
    switchTurn();
}
