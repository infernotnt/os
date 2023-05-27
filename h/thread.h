#pragma once
#include "../lib/hw.h"
#include "../h/my_console.h"
#include "../h/c_api.h"
#include "../h/cpp_api.h"

#define NR_MAX_THREADS 10
#define NR_REGISTERS 32

// needs to be (DEFAULT_STACK_SIZE+8*34) (probably)
#define ACTUAL_STACK_SIZE (DEFAULT_STACK_SIZE+8*34)*10
#define MAX_NR_TOTAL_THREADS 100

class Thread;

class Thread
{
public:
    enum State { RUNNING = 0, SUSPENDED, READY, INITIALIZING, TERMINATING, NONEXISTENT };

    typedef void(*Body)(void*);

    static bool switchedToUserThread;
//    static uint64** pRunningSp;
//    static uint64* runningSp;
    static uint64** pRunningSp;
    static uint64 timeSliceCounter;
    static Thread* pAllThreads[MAX_NR_TOTAL_THREADS];
    static uint64 nrTotalThreads;
    static uint64 initialUserMemoryUsage;

    static  Thread*  getPRunning();
    static void setPRunning(Thread* p);
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
    Thread* pNext;
    Thread* pWaitingHead;
    bool done;
//    uint64** pSp;

    uint64* sp;

    Thread(Body body, void* arg);
    Thread() : pStackStart(nullptr), pNext(nullptr) {} // move to private?. Mind the kernel thread

    void init(Body body, void* arg, void* pLogicalStack);
    void* allocStack();
    void signalDone();

private:
    Thread(Body body, void* arg, void* pStartOfStack); // threads can only be made with createThread. Maybe Thread() = delete;?
    static Thread* pRunning;
};
