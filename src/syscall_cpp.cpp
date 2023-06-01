#include "../h/syscall_cpp.hpp"

time_t PeriodicThread::getPeriod()
{
    return period;
}

void PeriodicThread::terminate()
{
    period = 0;
}

int Thread::sleep (time_t time)
{
    return time_sleep(time);
}

void periodicWrapper(void* p)
{
    PeriodicThread* t = (PeriodicThread*)p;

    if(t->getPeriod() == 0)
        return;

    volatile uint64 i;
    while(i>=0)
    {
        time_sleep(t->getPeriod());

        if(t->period == 0)
            break;

        t->periodicActivation();
    }
}

PeriodicThread::PeriodicThread (time_t period)
    : Thread(&periodicWrapper, (void*)this), period(period)
{ }

Semaphore::Semaphore(unsigned init)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore()
{
    sem_close(myHandle);
}

int Semaphore::wait()
{
    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    return sem_signal(myHandle);
}

void* operator new(size_t size)
{
    return mem_alloc(size);
}

void operator delete(void* p)
{
    mem_free(p);
}

char Console::getc()
{
    return ::getc();
}

void Console::putc(char c)
{
    ::putc(c);
}

Thread::Thread (void (*body)(void*), void* arg)
    : body(body), arg(arg)
{ }

int Thread::start()
{
    return thread_create(&myHandle, body, arg);
}

void Thread::join()
{
    thread_join(myHandle);
}

Thread::~Thread()
{
    join();
}

void Thread::dispatch()
{
    thread_dispatch();
}

void _Thread_wrapper(void* t)
{
    ((Thread*)t)->run();
}

Thread::Thread()
    :body(_Thread_wrapper), arg(this)
{ }
