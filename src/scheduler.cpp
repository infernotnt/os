#include "../h/scheduler.h"
#include "../h/semaphore.h"

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

    IThread::getPRunning()->state = IThread::SUSPENDED;
    IThread::getPRunning()->remainingSleep = time;

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

    dispatchToNext();

    return 0;
}

void Scheduler::dispatchUserVersion()
{
//    extern IThread kernelThread;

    // reenable, temp comment
//    assert(IThread::getPRunning() !=
//           &kernelThread); // you're only supposed to open user threads with system call with code 4

    if(IThread::getPRunning()->id != BUSY_WAIT_THREAD_ID)
    {
        IThread::getPRunning()->state = IThread::READY;
        Scheduler::put(IThread::getPRunning());
    }

    Scheduler::dispatchToNext();
}

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

void Scheduler::dispatchToNext() // WARNING: different than sys. call thread_dispatch()
{
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    IThread *pOld = IThread::getPRunning();
    IThread *pNew = Scheduler::getNext();

    extern IThread kernelThread;
    // temp comment
//    assert(pNew != &kernelThread);

    bool existReadyThread = (pNew != nullptr);

    if(existReadyThread == false)
    {
        bool existSleeper = (Scheduler::get()->pSleepHead != nullptr);
        bool isWaitingForSemaphore = checkIfWaitingForSemaphore();
        if (existReadyThread == false && (existSleeper == true || isWaitingForSemaphore == true))
        {
//            kPutString("===== SLEEPING WAIT OR INPUT WAIT");
//            kPutNewline();

            pNew = IThread::pAllThreads[BUSY_WAIT_THREAD_ID];
            Scheduler::put(pNew);
        }
        else if (existReadyThread == false && existSleeper == false && isWaitingForSemaphore == false)
        {
            pNew = &kernelThread;

#ifdef __DEBUG_PRINT
            kPutString("=== NO MORE USER THREADS EXIST. RETURNING TO KERNEL THREAD");
            kPutNewline();
#endif

            assert(IThread::pAllThreads[0] == &kernelThread);
            IThread::pAllThreads[0]->state = IThread::READY;

            pNew = IThread::pAllThreads[0];

            // TODO: set permission
        }
    }

    assert(pNew->state == IThread::READY);

    pOld->state = IThread::READY; // must be in this order (maby)
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
