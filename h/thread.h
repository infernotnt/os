#pragma once

#include "../lib/hw.h"
#include "../h/my_console.h"
#include "../h/c_api.h"
#include "../h/cpp_api.h"

#define NR_MAX_THREADS 10
#define NR_REGISTERS 32
#define ACTUAL_STACK_SIZE 4096*2

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
    typedef uint64 Context[NR_REGISTERS+1]; // the +1 is for the sepc

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

    static bool userThreadExists;
    static Thread* pRunning;
    static uint64* pRunningContext;
    static uint64 timeSliceCounter;

    static int createThread(thread_t* handle, Body body, void* arg, void* pStartOfStack = nullptr);
    static int exit(); // exit the currently running thread
    static void setPRunning(Thread* pNew);

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
};

inline uint64 Thread::getTimeSlice()
{
    return timeSlice;
}

