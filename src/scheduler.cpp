#include "../h/scheduler.h"

void Scheduler::printState()
{
    putU64(Thread::getPRunning()->id);
    putString(":  ");

    Thread* pCur = get()->pHead;
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
    assert(&(Thread::getPRunning()->context[0]) == Thread::pRunningContext);

    Thread *pOld = Thread::getPRunning();
    Thread *pNew = Scheduler::getNext();

    assert(pNew->state == Thread::READY);

    Thread::setPRunning(pNew);

    if(pOld != pNew) // split into two cases for safety
    {
        pNew->state = Thread::RUNNING;
    }
    else
    {
        pNew->state = Thread::RUNNING;
    }
}

void Scheduler::put(Thread* p)
{
    if(!p)
    {
        assert(false);
    }

    assert(p->state == Thread::READY);

    p->pNext = nullptr; // ensure no thread is after this one (and ensure no random pointers)

    if(!(get()->pHead))
    {
        get()->pHead = p;
        return;
    }

    Thread* pCur = get()->pHead;
    while(pCur->pNext)
    {
        pCur = pCur->pNext;
    }
    pCur->pNext = p;
}

Thread* Scheduler::getNext()
{
//    if(!pHead)
//    {
//        assert(false); // disabled because maby you can get the thread* and do something wrong with it assuming its a legit thread.
//        return spinThread;
//    }

    Thread* ret = get()->pHead;
    get()->pHead = get()->pHead->pNext;

    return ret;
}