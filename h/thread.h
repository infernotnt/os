#pragma once
#include "../lib/hw.h"
#include "../h/my_console.h"
#include "../h/syscall_c.hpp"

#define NR_MAX_THREADS 10
#define BUSY_WAIT_THREAD_ID 9999999
#define USER_THREAD_ID 1

// needs to be (DEFAULT_STACK_SIZE+8*34) (probably)
#define ACTUAL_STACK_SIZE (DEFAULT_STACK_SIZE+8*34)*10
#define MAX_NR_TOTAL_THREADS 100

class IThread;

class IThread
{
public:
    enum State { RUNNING = 0, SUSPENDED, READY, INITIALIZING, TERMINATING, NONEXISTENT };

    typedef void(*Body)(void*);

    static IThread* pSleepHead;
    static uint64 timeSliceCounter;
    static bool switchedToUserThread;
    static uint64** pRunningSp;
    static IThread* pAllThreads[MAX_NR_TOTAL_THREADS];
    static uint64 nrTotalThreads;
    static uint64 initialUserMemoryUsage;

    static  IThread*  getPRunning();
    static void setPRunning(IThread* p);
    static int createThread(uint64* id, Body body, void* arg);
    static int exit(); // exit the currently running thread
    static void switchToUser();
    static void join(uint64 id);

    State state;
    Body body;
    uint64 sepc;
    void* pStackStart; // start in terms of the data structure. This adress is the highest one in the stack
    uint64 timeSlice; // private
    uint64 id;
    IThread* pNext;
    IThread* pWaitingHead;
    bool done;
    uint64 remainingSleep;

//    uint64** pSp;

    uint64* sp;

    IThread(Body body, void* arg);
    IThread() : pStackStart(nullptr), pNext(nullptr) {} // move to private?. Mind the kernel thread

    void allocStack();
    void initContext(void* arg);
    void initClass(Body threadBody); // like a normal constructor

    void signalDone();

//    void init(Body body, void* arg, void* pLogicalStack);

private:
    IThread(Body body, void* arg, void* pStartOfStack); // threads can only be made with createThread. Maybe IThread() = delete;?
    static IThread* pRunning;
};
