#include "thread.h"
#include <signal.h>
#include <cassert>



#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
        "rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}


#endif


Thread::Thread(int id, int stackSize, thread_entry_point enteryPoint): 
    id(id), quantomCount(0) {
    setReady(false);
    setRunning(false);
    setSleeping(false);
    setBlocked(false);
    sigsetjmp(env, 1);
    if (id != 0) {
        stack = new char[stackSize];
        address_t sp = (address_t) stack + stackSize - sizeof(address_t);
        address_t pc = (address_t) enteryPoint;
        (env->__jmpbuf)[JB_SP] = translate_address(sp);
        (env->__jmpbuf)[JB_PC] = translate_address(pc);
    }
    else {
        stack = nullptr;
    }
    if (sigemptyset(&env->__saved_mask) == -1) {
        exit_errno();
    };
}


    void Thread::setReady(bool state) {
        ready = state;
        verifyStates();
    }

    void Thread::setRunning(bool state) {
        running = state;
        verifyStates();
    }


    void Thread::setSleeping(bool state) {
        sleeping = state;
        verifyStates();
    }


    void Thread::setBlocked(bool state) {
        blocked = state;
        verifyStates();
    }


    void Thread::verifyStates() {
        assert(!(ready && (running || blocked || sleeping)));
        assert(!(blocked && id == 0));
    }

    bool Thread::getReady() {return ready;};
    bool Thread::getRunning() {return running;};
    bool Thread::getSleeping() {return sleeping;};
    bool Thread::getBlocked() {return blocked;};


