#include "../h/semaphore.h"
#include "../h/alloc.h"
#include "../h/scheduler.h"

uint64 ISemaphore::nrSemaphores = 0;
ISemaphore* ISemaphore::pAllSemaphores[NR_MAX_SEMAPHORES];

int ISemaphore::close(uint64 id)
{
    ISemaphore* target = pAllSemaphores[id];
    assert(target->id == id);

    IThread *pCur = target->pBlockedHead;
    IThread *pPrev = nullptr;

    while(pCur != nullptr)
    {
        *( ((uint64*)(pCur->sp))+10 ) = -1; // sem_wait will return -1, an error
        pCur->state = IThread::READY;
        Scheduler::put(pCur);

        if(pPrev != nullptr)
        {
            pPrev->pNext = nullptr;
        }

        pPrev = pCur;
        pCur = pCur->pNext;
    }

    if(pCur != nullptr)
        pCur->pNext = nullptr;

    return 0;
}

int ISemaphore::create(uint64* pId, uint64 initialValue)
{
    assert(nrSemaphores < NR_MAX_SEMAPHORES);

    pAllSemaphores[nrSemaphores++] = (ISemaphore*) MemAlloc::get()->allocMem(sizeof(ISemaphore));

    ISemaphore* s = pAllSemaphores[nrSemaphores-1];

    s->id = nrSemaphores-1;
    *pId = s->id;
    s->value = initialValue;
    s->pBlockedHead = nullptr;

    return 0;
}

int ISemaphore::wait(uint64 id)
{
    ISemaphore* target = pAllSemaphores[id];
    assert(target->id == id);

    target->value--;

    if(target->value < 0)
    {
        target->addToQueue();
        IThread::getPRunning()->state = IThread::SUSPENDED;

        Scheduler::get()->dispatchToNext();
    }
    else
    {
        *( ((uint64*)(IThread::getPRunning()->sp))+10 ) = 0; // succefull sem_wait, as the thread was never even blocked

    }

    return 0;
}

void ISemaphore::addToQueue()
{
    IThread* pCur = pBlockedHead;

    if(pCur == nullptr)
    {
        pBlockedHead = IThread::getPRunning();
    }
    else
    {
        while(pCur->pNext != nullptr)
        {
            pCur = pCur->pNext;
        }

        pCur->pNext = IThread::getPRunning();
    }

    IThread::getPRunning()->pNext = nullptr;
}

int ISemaphore::signal(uint64 id)
{
    ISemaphore* target = pAllSemaphores[id];
    assert(target->id == id);

    target->value++;
    if(target->value <= 0)
    {
        IThread* thr = target->removeFromQueue();

        *( ((uint64*)(thr->sp))+10 ) = 0; // succesfull wait, as it was unblocked by sem_signal, and not by sem_close

        thr->state = IThread::READY;
        Scheduler::put(thr);
    }

    return 0;
}

IThread* ISemaphore::removeFromQueue()
{
    assert(pBlockedHead != nullptr);

    IThread* t = pBlockedHead;
    pBlockedHead = pBlockedHead->pNext;

    t->pNext = nullptr;

    return t;
}