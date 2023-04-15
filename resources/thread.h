#include <setjmp.h>
#include "ex_utils.h"

typedef unsigned long address_t;
typedef void (*thread_entry_point)(void);

class Thread {
    public:
    int id;
    int quantomCount;
    char *stack;
    sigjmp_buf env;

    Thread(int id, int stackSize, thread_entry_point entry_point = nullptr);
    void setReady(bool state);
    void setRunning(bool state);
    void setSleeping(bool state);
    void setBlocked(bool state);
    bool getReady();
    bool getRunning();
    bool getSleeping();
    bool getBlocked();

    private:
    bool ready;
    bool running;
    bool sleeping;
    bool blocked;
    void verifyStates();
    
};