#pragma once
#include "../h/syscall_c.hpp"

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
//    static int sleep (time_t); // TODO
protected:
    Thread ();
    virtual void run () {}

    friend void _Thread_wrapper(void*);

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

