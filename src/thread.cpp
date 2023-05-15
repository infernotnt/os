#include "../h/thread.h"
#include "../h/alloc.h"
#include "../h/scheduler.h"

Thread* Thread::pRunning = nullptr;
uint64* Thread::pRunningContext = nullptr;
uint64 Thread::timeSliceCounter = 0;
bool Thread::switchedToUserThread = 0;
uint64 Thread::nrTotalThreads = 1;
Thread* Thread::pAllThreads[MAX_NR_TOTAL_THREADS];

void Thread::setPRunning(Thread* p)
{
    pRunning = p;
    pRunningContext = &(p->context[0]);
}

int Thread::createThread(uint64* id, Body body, void* arg)
{
    assert(nrTotalThreads < MAX_NR_TOTAL_THREADS);

    Thread* t = (Thread*)MemAlloc::get()->allocMem(sizeof(Thread));
    pAllThreads[nrTotalThreads] = t;

    t->pStackStart = nullptr;
    void* pLogicalStack = t->allocStack();

    t->init(body, arg, pLogicalStack);

    Scheduler::put(t);

    return 0;
}

Thread* Thread::getPRunning()
{
    __asm__ volatile("mv x10, x10"); // to avoid inline
    return pRunning;
}

void* Thread::allocStack()
{
    assert(pStackStart == nullptr); // error: this threads stack may have already been allocated

    pStackStart = MemAlloc::get()->allocMem(ACTUAL_STACK_SIZE);
    uint64 stack =  (uint64)pStackStart + ACTUAL_STACK_SIZE;

    if(stack % 16 != 0)
    {
        stack -= stack % 16;
    }

    stack -= 34*8; // for the initial context (necessary to avoid exceptions from reading from unallowed adress), valjda

    return (void*)stack;

}

void wrapper(uint64 __DO_NOT_USE, Thread::Body body, void* arg) // this entire function can be run from USER thread
{
    (*body)(arg);
    thread_exit();
}

void Thread::init(Body body, void* arg, void* pLogicalStack) // this is used as a "constructor"
{
    assert(nrTotalThreads < MAX_NR_TOTAL_THREADS);
    id = nrTotalThreads;
    nrTotalThreads++;

    assert((uint64)pLogicalStack % 16 == 0);

    state = INITIALIZING;
    timeSlice = DEFAULT_TIME_SLICE;
    pNext = nullptr;

    // sets context
    context[2] = (uint64)pLogicalStack; // sp field

    // skipping a0 to pass arguments as it is will be not be restored in the context switch because it is assumed to hold return values of a sys. call

    *((uint64*)pLogicalStack + 11) = (uint64)body;
    *((uint64*)pLogicalStack + 12) = (uint64)arg;
    *((uint64*)pLogicalStack + 32) = (uint64)&wrapper;

    state = READY;
}

int Thread::exit()
{
    Thread* t = Thread::getPRunning();
    assert(t->state == RUNNING);

    MemAlloc::get()->freeMem(t->pStackStart);
//    t->signalDone();

    if(Scheduler::get()->pHead == nullptr)
    {
        putString("NO MORE THREADS EXIST, IDK WHAT TO RUN");
        putNewline();
        assert(false);
    }
    Scheduler::dispatchToNext();

    return 0;
}
