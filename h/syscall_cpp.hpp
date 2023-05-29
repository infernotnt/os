#pragma once
#include "../h/syscall_c.h"

void* operator new(size_t size);
void operator delete(void* p);

class Thread
{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    void join();
    static void dispatch ();
    static int sleep (time_t);
protected:
    Thread ();
    virtual void run () {}

    friend void _Thread_wrapper(void*);

    void forceExit();

private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
};

class Console
{
public:
    static char getc ();
    static void putc (char);
};


class Semaphore
{
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread
{
public:
    void terminate ();
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    time_t period;

    time_t getPeriod();

    friend void periodicWrapper(void* p);
};
