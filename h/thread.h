#pragma once

#include "../lib/hw.h"
#include "../h/my_console.h"
#include "../h/c_api.h"
#include "../h/cpp_api.h"

#define NR_MAX_THREADS 10
#define NR_REGISTERS 32
#define ACTUAL_STACK_SIZE 4096*10

class Thread;
typedef Thread* thread_t;

class Thread
{
public:
    enum State
    {
        RUNNING = 0, SUSPENDED, READY, INITIALIZING, TERMINATING, NONEXISTENT
    };
    typedef void(*Body)(void*);
    typedef uint64 Context[50+1]; // the +1 is for the sepc // temp 50

    static Thread** getAllThreads()
    {
        static bool firstCall = true;
        static Thread* allThreads[NR_MAX_THREADS];

        if(firstCall)
        {
            for(int i=0; i<NR_MAX_THREADS; i++)
                allThreads[i] = nullptr;
            firstCall = false;
        }

        return allThreads;
    }

    static uint64* pRunningContext;
    static bool userThreadExists;
    static uint64 timeSliceCounter;

    static Thread* getPRunning()
    {
        return pRunning;
    }

    static void setPRunning(Thread* p)
    {
        pRunning = p;
        pRunningContext = &(p->context[0]);
    }

    static int createThread(thread_t* handle, Body body, void* arg, void* pStartOfStack = nullptr);
    static int exit(); // exit the currently running thread

    State state;
//    bool started;
    Body body;
    Context context;
    uint64* pStackStart; // start in terms of the data structure. This adress is the highest one in the stack
    uint64 timeSlice; // private

    void deleteThread(); // can be called when you want to delete the thread resources from the system and free up a thread slot
    inline uint64 getTimeSlice();

    Thread* pNext;

    Thread() = default; // move to private?
private:
    Thread(Body body, void* arg, void* pStartOfStack = nullptr); // threads can only be made with createThread. Maybe Thread() = delete;?
    static Thread* pRunning;
};

inline uint64 Thread::getTimeSlice()
{
    return timeSlice;
}

