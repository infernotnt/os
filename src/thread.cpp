#include "../h/thread.h"
#include "../h/alloc.h"
#include "../h/scheduler.h"

uint64 IThread::timeSliceCounter = 0;

IThread* IThread::pRunning = nullptr;
uint64** IThread::pRunningSp = nullptr;
uint64 IThread::nrTotalThreads = 1;
IThread* IThread::pAllThreads[MAX_NR_TOTAL_THREADS];

void wrapper(uint64 __DO_NOT_USE, IThread::Body body, void* arg) // this entire function can be run from USER thread
{
    (*body)(arg);
    thread_exit();
}

void IThread::signalDone()
{
    IThread* pCur = pWaitingHead;

    while(pCur)
    {
        pCur->state = READY;
        Scheduler::put(pCur);

        pCur = pCur->pNext;
    }
}

void IThread::join(uint64 id)
{
    IThread* oldRunning = IThread::getPRunning();

    IThread* t = pAllThreads[id];

    extern IThread kernelThread;
    assert(oldRunning != &kernelThread); // you're only supposed to switch to user threads with system call with code 4

    if(t->done == true)
        return;

    if(t->pWaitingHead == nullptr)
    {
        t->pWaitingHead = oldRunning;
    }
    else
    {
        IThread* pCur = t->pWaitingHead;
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

void IThread::switchToUser()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));
    bool cameFromKernelMode = ((scause & (1 << 8)) == 0);

    if(!cameFromKernelMode) // if user thread calls this it does nothing
    {
#ifdef __DEBUG_MODE
        assert(false);
        assert(IThread::pRunning->id != 0);
#endif
        return;
    }

    assert(IThread::pRunning->id == 0);

    Scheduler::dispatchToNext();

    assert(IThread::getPRunning()->id == USER_THREAD_ID);

    __asm__ volatile("li t1, 256");
    __asm__ volatile("csrc sstatus, t1"); // changes to user mode by changing the "spp" bit
}

void IThread::setPRunning(IThread* p)
{
    pRunning = p;
    pRunningSp = &(p->sp);
}

int IThread::createThread(uint64* id, Body body, void* arg, void* stackSpace)
{
    IThread* t = (IThread*)MemAlloc::get()->allocMem(sizeof(IThread));

    if(t == nullptr)
        return -1;

    t->initClass(body, stackSpace);
    t->configureStack(stackSpace);
    t->initContext(arg);

    *id = t->id;
    Scheduler::put(t);

    return 0;
}

void IThread::initClass(Body threadBody, void* stackSpace)
{
    assert(nrTotalThreads < MAX_NR_TOTAL_THREADS);

    IThread::pAllThreads[nrTotalThreads] = this;
    id = IThread::nrTotalThreads;
    IThread::nrTotalThreads++;

    body = threadBody;

    sp = nullptr;
    pNext = nullptr;
    pWaitingHead = nullptr;
    done = false;
    state = READY;
    initialSp = stackSpace;
}

void IThread::initContext(void* arg)
{
    // skipping a0 to pass arguments as it is will be not be restored in the context switch because it is assumed to hold return values of a sys. call

    for(uint64 i=0; i<32; i++)
    {
        *(sp+i) = 0; // should be set to 0 because of tp and gp and maby other shit
    }

    *(sp+11) = (uint64)body;
    *(sp+12) = (uint64)arg;
    *(sp+32) = (uint64)&wrapper;
}

IThread* IThread::getPRunning()
{
    __asm__ volatile("mv x10, x10"); // to avoid inline
    return pRunning;
}

void IThread::configureStack(void* stack)
{
    assert(sp == nullptr);

    sp = (uint64*)((char*)stack);

    assert(((uint64)sp) % 16 == 0);
    sp = sp - 34; // for the initial context (necessary to avoid exceptions from reading from unallowed adress), valjda
    assert(((uint64)sp) % 16 == 0);
}

int IThread::exit()
{
    IThread* t = IThread::getPRunning();
    assert(t->state == RUNNING);
    assert(t->done == false);
    t->done = true;

    MemAlloc::get()->freeMem((char*)t->initialSp - DEFAULT_STACK_SIZE);

    t->signalDone(); // WARNING: must be in this order

    Scheduler::dispatchToNext(); // WARNING: must be before signalDone()

    return 0;
}
