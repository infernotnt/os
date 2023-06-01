#include "../h/scheduler.h"
#include "../h/semaphore.h"

extern IThread kernelThread;

extern uint64 gTimer;

void Scheduler::reduceSleepTimerForAll()
{
    IThread* pCur = Scheduler::get()->pSleepHead;

    while (pCur)
    {
        pCur->remainingSleep--;

        assert(pCur->remainingSleep >= 0);

        pCur = pCur->pNext;
    }
}

void Scheduler::doSleepStuffOnTick()
{
    reduceSleepTimerForAll();

    while(true)
    {
        IThread* pCur = Scheduler::get()->pSleepHead;
        IThread* pPrev = nullptr;
        bool changedList = false;

        while (pCur)
        {
            IThread *pNext = pCur->pNext;

            if(pCur->remainingSleep == 0)
            {
                if(pCur == Scheduler::get()->pSleepHead) // first in list
                    Scheduler::get()->pSleepHead = pCur->pNext;
                else pPrev->pNext = pCur->pNext; // not first in list

                changedList = true;

                pCur->state = IThread::READY;
                pCur->pNext = nullptr;
                Scheduler::put(pCur); // WARNING: must be last command to preserve the pCur->pNext

                break;
            }
            pPrev = pCur;
            pCur = pNext;
        }

        if(changedList == false) // this means there was one pass trough the loop and no changes were made
            break;
    }
}

void Scheduler::doTimeSliceAndGTimeOnTick()
{

    assert(IThread::timeSliceCounter <= DEFAULT_TIME_SLICE);

    if(IThread::timeSliceCounter == DEFAULT_TIME_SLICE && IThread::getPRunning()->id != BUSY_WAIT_THREAD_ID)
        Scheduler::dispatchUserVersion();
    else if(IThread::getPRunning()->id == BUSY_WAIT_THREAD_ID && Scheduler::get()->pHead != nullptr)
        Scheduler::dispatchToNext();

#ifdef __DEBUG_PRINT
    if (gTimer % 50 == 0)
    {
        kPutString("Time: ");
        kPutU64(gTimer / 10);
        kPutString("s");
        kPutNewline();
    }
#endif

    if(IThread::timeSliceCounter == DEFAULT_TIME_SLICE)
        IThread::timeSliceCounter = 0;
    else
    {
        IThread::timeSliceCounter++;
    }

    gTimer++;

}

bool checkIfWaitingForSemaphore()
{
    if(IConsole::get()->inputSemaphore->pBlockedHead != nullptr)
        return true;

    for (uint64 i = 0; i < ISemaphore::nrSemaphores; i++)
        if (ISemaphore::pAllSemaphores[i]->isOpen == true)
            if (ISemaphore::pAllSemaphores[i]->pBlockedHead != nullptr)
                return true;

    return false;
}

int Scheduler::sleep(time_t time)
{
    assert(IThread::getPRunning()->state == IThread::RUNNING);
    assert(IThread::getPRunning()->done == false);

    if(pSleepHead == nullptr)
    {
        pSleepHead = IThread::getPRunning();
        IThread::getPRunning()->pNext = nullptr;
    }
    else
    {
        IThread::getPRunning()->pNext = pSleepHead;
        pSleepHead = IThread::getPRunning();
    }

    IThread::getPRunning()->state = IThread::SUSPENDED;
    IThread::getPRunning()->remainingSleep = time;
    dispatchToNext();

    return 0;
}

void Scheduler::dispatchUserVersion()
{
    assert(IThread::getPRunning() !=
           &kernelThread); // you're only supposed to open user threads with system call with code 4

    assert(IThread::getPRunning()->id != BUSY_WAIT_THREAD_ID);

    IThread::getPRunning()->state = IThread::READY;
    Scheduler::put(IThread::getPRunning());
    Scheduler::dispatchToNext();
}

/*
void Scheduler::specialBusyWaitDispatch()
{
    IThread* t = IThread::getPRunning();
    if(t->id != BUSY_WAIT_THREAD_ID)
    {
#ifdef __DEBUG_MODE
        assert(false);
#endif
        return;
    }

    if(t->pNext == nullptr)
        return;

    Scheduler::put(t->pNext);
    dispatchToNext();
}
 */

void Scheduler::printState()
{
    kPutU64(IThread::getPRunning()->id);
    kPutString(":  ");

    IThread* pCur = get()->pHead;
    while(pCur)
    {
        kPutU64(pCur->id);

        if(pCur->pNext)
        {
            kPutString("->");
        }
        else
        {
            break;
        }

        pCur = pCur->pNext;
    }

    kPutNewline();
}

IThread* doWaitingStuff()
{
    IThread* pNew;

#ifdef __DEBUG_PRINT
//    kPutString("===== SLEEPING WAIT OR INPUT WAIT");
//    kPutNewline();
#endif

    pNew = IThread::pAllThreads[BUSY_WAIT_THREAD_ID];

    return pNew;
}

IThread* doKernelSwitchStuff()
{
    IThread* pNew;

    pNew = &kernelThread;

#ifdef __DEBUG_PRINT
    kPutString("=== NO MORE USER THREADS EXIST. RETURNING TO KERNEL THREAD");
    kPutNewline();
#endif

    assert(IThread::pAllThreads[0] == &kernelThread);
    IThread::pAllThreads[0]->state = IThread::READY;

    __asm__ volatile("li t1, 256");
    __asm__ volatile("csrs sstatus, t1"); // changes to kernel mode by changing the "spp" bit

    __asm__ volatile("li t1, 32");
    __asm__ volatile("csrc sstatus, t1"); // change "spie" bit in sstatus register so external interupts are disabled when switching to kernel thread

    return pNew;
}

void Scheduler::dispatchToNext() // WARNING: different than sys. call thread_dispatch()
{
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    IThread *pOld = IThread::getPRunning();
    IThread *pNew = Scheduler::getNext();

    bool existReadyThread = (pNew != nullptr);

    if(existReadyThread == false)
    {
        bool existSleeper = (Scheduler::get()->pSleepHead != nullptr);
        bool isWaitingForSemaphore = checkIfWaitingForSemaphore();
        if (existReadyThread == false && (existSleeper == true || isWaitingForSemaphore == true))
            pNew = doWaitingStuff();
        else if (existReadyThread == false && existSleeper == false && isWaitingForSemaphore == false)
            pNew = doKernelSwitchStuff();
    }

    assert(pNew->state == IThread::READY);

    pOld->state = IThread::READY; // must be in this order
    pNew->state = IThread::RUNNING;

    IThread::setPRunning(pNew);
}

void Scheduler::put(IThread* p)
{
    if(!p)
    {
        assert(false);
    }

    assert(p->state == IThread::READY);

    p->pNext = nullptr; // ensure no thread is after this one (and ensure no random pointers)

    if(!(get()->pHead))
    {
        get()->pHead = p;
        return;
    }

    IThread* pCur = get()->pHead;
    while(pCur->pNext)
    {
        pCur = pCur->pNext;
    }
    pCur->pNext = p;
}

IThread* Scheduler::getNext()
{
    IThread* ret = get()->pHead;

    if(ret == nullptr)
        return ret;

    get()->pHead = get()->pHead->pNext;

    return ret;
}
