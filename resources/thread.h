#include <setjmp.h>

typedef unsigned long address_t;
typedef void (*thread_entry_point)(void);

enum ThreadState {RUNNING, READY, BLOCKED, SLEEPING};

class Thread {
    public:
    int id;
    int quantomCount;
    ThreadState state;
    char *stack;
    sigjmp_buf env;

    Thread(int id, int stackSize, thread_entry_point entry_point = nullptr);
};