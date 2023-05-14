#include "../h/scheduler.h"

void Scheduler::dispatchToNext() // WARNING: different than sys. call dispatch()
{
    assert(&(Thread::getPRunning()->context[0]) == Thread::pRunningContext);

    Thread *pOld = Thread::getPRunning();
    Thread *pNew = Scheduler::getNext();

    Scheduler::put(pOld);
    Thread::setPRunning(pNew);
    //__asm__ volatile ("csrw scratch, %[name]" : : [name] "r" (Thread::pRunning->context));
}

void Scheduler::put(Thread* p)
{
    if(!p) return;
    assert(p->state == Thread::State::READY);

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