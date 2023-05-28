#include "../h/scheduler.h"

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
    extern IThread kernelThread;
    assert(IThread::getPRunning() !=
           &kernelThread); // you're only supposed to open user threads with system call with code 4

    if(IThread::getPRunning()->id != BUSY_WAIT_THREAD_ID)
    {
        IThread::getPRunning()->state = IThread::READY;
        Scheduler::put(IThread::getPRunning());
    }

    Scheduler::dispatchToNext();
}

void Scheduler::printState()
{
    putU64(IThread::getPRunning()->id);
    putString(":  ");

    IThread* pCur = get()->pHead;
    while(pCur)
    {
        putU64(pCur->id);

        if(pCur->pNext)
        {
            putString("->");
        }
        else
        {
            break;
        }

        pCur = pCur->pNext;
    }

    putNewline();
}

void Scheduler::dispatchToNext() // WARNING: different than sys. call thread_dispatch()
{
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    IThread *pOld = IThread::getPRunning();
    IThread *pNew = Scheduler::getNext();

    extern IThread kernelThread;
    assert(pNew != &kernelThread);

    bool existReadyThread = (pNew != nullptr);
    bool existSleeper = (Scheduler::get()->pSleepHead != nullptr);

    if(existReadyThread == false && existSleeper == false)
    {
        pNew = &kernelThread;

        putString("=== NO MORE USER THREADS EXIST. RETURNING TO KERNEL THREAD");
        putNewline();

        assert(IThread::pAllThreads[0] == &kernelThread);
        IThread::pAllThreads[0]->state = IThread::READY;

        pNew = IThread::pAllThreads[0];

        // TODO: set permission
    }
    else if(existReadyThread == false && existSleeper == true)
    {
        assert(existReadyThread == false && existSleeper == true);

        pNew = IThread::pAllThreads[BUSY_WAIT_THREAD_ID];
        Scheduler::put(pNew);
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
