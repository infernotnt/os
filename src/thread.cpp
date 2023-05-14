#include "../h/thread.h"
#include "../h/alloc.h"
#include "../h/scheduler.h"

uint64 Thread::timeSliceCounter = 0;
Thread* Thread::pRunning = nullptr;
uint64* Thread::pRunningContext = nullptr;
bool Thread::userThreadExists = false;

//void Thread::setPRunning(Thread* pNew)
//{
//    pRunning = pNew;
//    pRunningContext = &(pRunning->context[0]);
//}

void* allocStack(void* pStartOfStack)
{
    if (pStartOfStack)
    {
        return pStartOfStack;
    } else
    {
       return (uint64 *) (((char *) MemAlloc::get()->allocMem(ACTUAL_STACK_SIZE)) + ACTUAL_STACK_SIZE);
    }
}

void wrapper(Thread::Body body, void* arg)
{
    (*body)(arg);
    Thread::exit(); // TODO: turn into system call
}

Thread::Thread(Body body, void* arg, void* pStartOfStack)
{
    pStackStart = (uint64*)allocStack(pStartOfStack);

    assert((uint64)pStackStart % 16 == 0);

    state = INITIALIZING;
    timeSlice = DEFAULT_TIME_SLICE;
    pNext = nullptr;

    // sets context
    context[1] = (uint64)&thread_exit;
    context[2] = (uint64)pStackStart;
    context[NR_REGISTERS] = (uint64)body;

    state = READY;
}

int Thread::createThread(thread_t* handle, Body body, void* arg, void* pStartOfStack)
{
    Thread* thread = new Thread(body, arg, pStartOfStack);

    Scheduler::put(thread);

    int status = -1;

    for(int i=0; i<NR_MAX_THREADS; i++)
        if(getAllThreads()[i] == nullptr)
        {
            getAllThreads()[i] = thread;
            status = 0;
            break;
        }

    assert(status == 0); // temp
    return status;
}

int Thread::exit()
{
    // nesto preko ::pRunning
    assert(false);

    assert(Thread::pRunning->state == RUNNING); // ??
    Thread::pRunning->deleteThread(); // ??

    return 0;
}

void Thread::deleteThread()
{
    assert(state != INITIALIZING && state != NONEXISTENT);
    /// ...
    state = NONEXISTENT;
}