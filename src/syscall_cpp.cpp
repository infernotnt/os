#include "../h/syscall_cpp.hpp"

int Thread::sleep (time_t time)
{
    return time_sleep(time);
}

void periodicWrapper(void* p)
{
    PeriodicThread* t = (PeriodicThread*)p;

    volatile uint64 i;
    while(i++)
    {
        time_sleep(t->getPeriod());
        if(t->hasTerminated == true)
            break;
        t->periodicActivation();
    }
}

PeriodicThread::PeriodicThread (time_t period)
    : period(period), Thread(&periodicWrapper, (void*)this), hasTerminated(false)
{

}

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
    return ::getc(); // dal ovo "::"?
}

void Console::putc(char c)
{
    ::putc(c); // dal ovo "::"?
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
    thread_exit();
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
