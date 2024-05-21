#pragma once
#include "../lib/hw.h"
#include "../h/my_console.h"
#include "../h/syscall_c.h"
#include "../h/my_console.h"

#define NR_MAX_THREADS 10
#define BUSY_WAIT_THREAD_ID 1
#define USER_THREAD_ID 2

// needs to be (DEFAULT_STACK_SIZE+8*34) (probably)
#define ACTUAL_STACK_SIZE (DEFAULT_STACK_SIZE+8*34)*2
#define MAX_NR_TOTAL_THREADS 40

class IThread;

class IThread
{
public:
    enum State { RUNNING = 0, SUSPENDED, READY, INITIALIZING, TERMINATING, NONEXISTENT };

    typedef void(*Body)(void*);

    static IThread* pAllThreads[MAX_NR_TOTAL_THREADS];
    static uint64 timeSliceCounter;
    static uint64** pRunningSp;
    static uint64 nrTotalThreads;

    static  IThread*  getPRunning();
    static void setPRunning(IThread* p);
    static int createThread(uint64* id, Body body, void* arg, void* stack);
    static int exit(); // exit the currently running thread
    static void switchToUser();
    static void join(uint64 id);

    State state;
    Body body;
    void* initialSp;
    uint64 id;
    IThread* pNext;
    IThread* pWaitingHead;
    bool done;
    uint64 remainingSleep;
    uint64* sp;

    IThread(Body body, void* arg);
    IThread() : pNext(nullptr) {} // move to private?. Mind the kernel thread

    void configureStack(void* stack);
    void initContext(void* arg);
    void initClass(Body threadBody, void* stackSpace); // like a normal constructor

    void signalDone();

private:
    IThread(Body body, void* arg, void* pStartOfStack); // threads can only be made with createThread. Maybe IThread() = delete;?
    static IThread* pRunning;
};
