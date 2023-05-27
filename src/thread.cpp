#include "../h/thread.h"
#include "../h/alloc.h"
#include "../h/scheduler.h"

Thread* Thread::pRunning = nullptr;
//uint64* Thread::runningSp = nullptr;
uint64** Thread::pRunningSp = nullptr;
uint64 Thread::timeSliceCounter = 0;
bool Thread::switchedToUserThread = 0;
uint64 Thread::nrTotalThreads = 1;
Thread* Thread::pAllThreads[MAX_NR_TOTAL_THREADS];
uint64 Thread::initialUserMemoryUsage;

void Thread::signalDone()
{
    Thread* pCur = pWaitingHead;

    while(pCur)
    {
        pCur->state = READY;
        Scheduler::put(pCur);

        pCur = pCur->pNext;
    }
}

void Thread::join(uint64 id)
{
    Thread* oldRunning = Thread::getPRunning();

    Thread* t = pAllThreads[id];

    extern Thread kernelThread;
    assert(oldRunning != &kernelThread); // you're only supposed to open user threads with system call with code 4

//    putU64(id);

    if(t->done == true)
        return;

    if(t->pWaitingHead == nullptr)
    {
        t->pWaitingHead = oldRunning;
    }
    else
    {
        Thread* pCur = t->pWaitingHead;
        while(pCur->pNext)
        {
            pCur = pCur->pNext;
        }

        pCur->pNext = oldRunning;
    }

    oldRunning->pNext = nullptr;
    oldRunning->state = SUSPENDED;

    Scheduler::dispatchToNext();
}

void Thread::switchToUser()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));
    bool cameFromKernelMode = ((scause & (1 << 8)) == 0);

    if(!cameFromKernelMode) // if user thread calls this it does nothing
    {
        assert(false); // temp
        assert(Thread::pRunning->id != 0);
        return;
    }

    assert(Thread::pRunning->id == 0);

    Scheduler::dispatchToNext();

    assert(Thread::getPRunning()->id == 1);

//    __asm__ volatile ("csrs sstatus, 0x6"); // TODO: probably should be removed when i add permissions
//    __asm__ volatile ("mv x10, x10");
//    __asm__ volatile ("mv x10, x10");
//    __asm__ volatile ("mv x10, x10");
//    putString("KITA PENIS");

//    __asm__ volatile ("csrc sstatus, 0x9");
//    __asm__ volatile ("csrc sstatus, 0x8");
//    __asm__ volatile ("csrw sstatus, 0"); // TODO: radi preko csrc jer ovako menjam sve
}

void Thread::setPRunning(Thread* p)
{
    pRunning = p;
    pRunningSp = &(p->sp);
}

int Thread::createThread(uint64* id, Body body, void* arg)
{
    assert(nrTotalThreads < MAX_NR_TOTAL_THREADS);

    Thread* t = (Thread*)MemAlloc::get()->allocMem(sizeof(Thread));
    pAllThreads[nrTotalThreads] = t;

    t->pStackStart = nullptr;
    void* pLogicalStack = t->allocStack();

    t->init(body, arg, pLogicalStack);

    *id = t->id;

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

void Thread::init(Body body, void* arg, void* pLogicalStack) // this is used as a "constructor", except for kernel thread
{
    assert(nrTotalThreads < MAX_NR_TOTAL_THREADS);
    id = nrTotalThreads;
    nrTotalThreads++;

    assert((uint64)pLogicalStack % 16 == 0);

    state = INITIALIZING;
    timeSlice = DEFAULT_TIME_SLICE;
    pNext = nullptr;
    pWaitingHead = nullptr;
    done = false;

    // sets context
    sp = (uint64*)pLogicalStack; // sp field

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
    assert(t->done == false);
    t->done = true;

    MemAlloc::get()->freeMem(t->pStackStart);

    t->signalDone(); // WARNING: must be in this order

    bool existReadyThread = !(Scheduler::get()->pHead == nullptr); // WARNING: must be after signalDone()
    if(!existReadyThread) // TODO: add (... && !existSleepedThread && !waitingForConsoleThread), vrv ne moram da proveravam dal neko ceka na semaforu
    {
        putString("=== NO MORE THREADS EXIST. RETURNING TO KERNEL THREAD");
        putNewline();

        extern Thread kernelThread;
        assert(pAllThreads[0] == &kernelThread);
        pAllThreads[0]->state = READY;
        Scheduler::put(Thread::pAllThreads[0]);

        // ovo je nesto pogresno
//        __asm__ volatile ("csrc sstatus, 0x6"); // set spie bit to 1. spie signifies that we dont want to mask external interrupts after sret
//        __asm__ volatile ("csrc sstatus, 0x9"); // clear spp bit, so we change to user mode after sret-ing to user thread
    }

    Scheduler::dispatchToNext();

    return 0;
}
