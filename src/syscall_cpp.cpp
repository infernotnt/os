#include "../h/syscall_cpp.hpp"

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
    thread_create(&myHandle, body, arg);
    return 0;
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

//    virtual ~Thread ();
//    int start ();
//    void join();
//    static void dispatch ();
//    static int sleep (time_t);
//protected:
//    Thread ();
//    virtual void run () {}
//private:
//    thread_t myHandle;
//    void (*body)(void*); void* arg;
//};
