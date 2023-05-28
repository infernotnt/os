#include "../h/scheduler.h"

void Scheduler::dispatchUserVersion()
{
    extern IThread kernelThread;
    assert(IThread::getPRunning() !=
           &kernelThread); // you're only supposed to open user threads with system call with code 4

    IThread::getPRunning()->state = IThread::READY;
    Scheduler::put(IThread::getPRunning());

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

void Scheduler::dispatchToNext() // WARNING: different than sys. call dispatch()
{
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    IThread *pOld = IThread::getPRunning();
    IThread *pNew = Scheduler::getNext();

    assert(pNew->state == IThread::READY);

    IThread::setPRunning(pNew);

    if(pOld != pNew) // split into two cases for safety
    {
        pNew->state = IThread::RUNNING;
    }
    else
    {
        pNew->state = IThread::RUNNING;
    }
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
//    if(!pHead)
//    {
//        assert(false); // disabled because maby you can get the thread* and do something wrong with it assuming its a legit thread.
//        return spinThread;
//    }

    IThread* ret = get()->pHead;
    get()->pHead = get()->pHead->pNext;

    return ret;
}
