#include "../h/my_console.h"
#include "../h/c_api.h"
#include "../h/alloc.h"
#include "../h/thread.h"

void testSystemCalls();
void testMemoryAllocator();
void testSyncCall();
uint64 fib(uint64);

void myUserMain()
{
//    __asm__ volatile ("csrw sscratch, x1"); // set spie bit to 1. spie signifies that we dont want to mask external interrupts after sret

    __asm__ volatile ("mv x10, x10");

//    putString("=== App started");
//    putNewline();
    assert(MemAlloc::get()->getUserlandUsage() == Thread::initialUserMemoryUsage);

    for(int i=0; i<1000000000; i++)
    {
        __asm__ volatile ("mv x10, x10");
    }

    testSystemCalls();
    testMemoryAllocator();
    testSyncCall();

//    assert(MemAlloc::get()->getUserlandUsage() == Thread::initialUserMemoryUsage); // TODO: enable after thread_exit


    putString("=== App ended");
    putNewline();
}

void testSyncCall()
{
//    disableExternalInterrupts();

//    assert(MemAlloc::get()->getUserlandUsage() == Thread::initialUserMemoryUsage);

    putString("=== Testing \"testSyncCall\"");
    putNewline();

    void doA(void*);
    void doB(void*);

    int argA = 69;
    int argB = 420;
    thread_t a;
    thread_t b;
    thread_create(&a, doA, &argA);
    thread_create(&b, doB, &argB);
    thread_dispatch();

//    putString("thread_t a = ");
//    putU64(a);
//    putNewline();
//    putString("thread_t b = ");
//    putU64(b);
//    putNewline();

    thread_join(b);
    thread_join(a);
//    assert(Thread::pAllThreads[a]->id == Thread::pAllThreads[b]->id-1);
//    assert(MemAlloc::get()->getUserlandUsage() == Thread::initialUserMemoryUsage);

//    enableExternalInterrupts();
    __asm__ volatile("mv x10, x10");

    putString("=== Success in testing \"testSyncCall\"");
    putNewline();
}


void doA(void* p)
{
    assert(*((int*)p) == 69);

    for(int i=0; i<=5; i++)
    {
//        putString("A i=");
//        putU64(i);
//        putNewline();

        assert(fib(i) == test_call(i));
        thread_dispatch();
    }

    __asm__ volatile("mv x10, x10");
    thread_exit();
    assert(false);
}

void doB(void* p)
{
    assert(*((int*)p) == 420);

    for(int i=0; i<=10; i++)
    {
//        putString("B i=");
//        putU64(i);
//        putNewline();

        assert(fib(i) == test_call(i));
        thread_dispatch();
    }

    thread_exit();
    assert(false);
}


uint64 fib(uint64 n)
{
    if(n == 1)
        return 1;
    else if (n == 0)
        return 0;
    else return fib(n-1) + fib(n-2);
}

void testSystemCalls()
{
    putString("=== Testing \"testSystemCalls\"");
    putNewline();

    int a = 0;
    int b = 0;

    for(int i=1; i<10; i++)
    {
        a++;
        b++;

        uint64 called = test_call(i);
        uint64 actual = fib(i);

        if(called != actual)
        {
            putString("test_call() error. i=");
            putInt(i);
            putString(" fib(i)=");
            putU64(actual);
            putString(" test_call(i)=");
            putU64(called);
            putNewline();
            assert(false);
        }

        assert(a == b);
    }

    assert(MemAlloc::get()->getUserlandUsage() == Thread::initialUserMemoryUsage);
    putString("=== Success in testing system calls");
    putNewline();
}

void testMemoryAllocator()
{
    putString("=== Testing \"testMemoryAllocator\"");
    putNewline();
    int n = 1000;
    int **a;
    a = (int**)mem_alloc(n * sizeof(void*));
    for(int i=0; i<n; i++)
    {
        a[i] = (int*)mem_alloc(sizeof(int));
        *a[i] = i;
    }

    int result = 0;
    for(int i=0; i<n; i++)
    {
        result += *a[i];
    }

//    uint64 vau = n * sizeof(void*);
//    if(n % MEM_BLOCK_SIZE != 0)
//        vau +=  MEM_BLOCK_SIZE - ((sizeof(int)* n) % MEM_BLOCK_SIZE);
//    vau += MEM_BLOCK_SIZE * n;
//    assert(MemAlloc::get()->getUserlandUsage() == vau);

//    putInt(result);
//    putNewline();

    assert(result == (n-1) * n / 2);

//    uint64 oldTaken = MemAlloc::get()->getUserlandUsage();
//    uint64 newTaken;

    int t;
    for(int i=0; i<n; i++)
    {
        t = mem_free(a[i]);
        assert(t == 0);

//        newTaken = MemAlloc::get()->getUserlandUsage();
//        assert(oldTaken - newTaken == MEM_BLOCK_SIZE);
    }

//    assert(MemAlloc::get()->getUserlandUsage() == MEM_BLOCK_SIZE);

    t = mem_free(a);
    assert(t == 0);

    assert(MemAlloc::get()->getUserlandUsage() == Thread::initialUserMemoryUsage);
    putString("=== Success in testing memory allocator");
    putNewline();
}
