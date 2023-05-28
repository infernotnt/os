#include "../h/my_console.h"
#include "../h/syscall_c.hpp"
#include "../h/alloc.h"
#include "../h/thread.h"

void testSystemCalls();
void testMemoryAllocator();
void testSyncCall();
void testTimeSlice();
void testTimeSleep();

uint64 fib(uint64);
extern uint64 gTimer;

void myUserMain()
{
//    __asm__ volatile ("csrw sscratch, x1"); // set spie bit to 1. spie signifies that we dont want to mask external interrupts after sret

    __asm__ volatile ("mv x10, x10");

    putString("=== App started");
    putNewline();

//    testSystemCalls();
//    testMemoryAllocator();
//    testSyncCall();
//    testTimeSlice();
//    testTimeSleep();

    __asm__ volatile ("mv x10, x10");
    time_sleep(500);
    __asm__ volatile ("mv x10, x10");

    putString("=== App ended");
    putNewline();
}


void testTimeSleep()
{
    putString("=== Testing \"testTimeSleep\"");
    putNewline();

    thread_t a, b;

    uint64 argA = 4;
    uint64 argB = 1;

    void doSleepA(void*);

    thread_create(&a, doSleepA, &argA);
    thread_create(&b, doSleepA, &argB);

    thread_join(a);
    thread_join(b);

    putString("=== PASS in \"testTimeSleep\"");
    putNewline();
}

void doSleepA(void* p)
{
    uint64 *n = (uint64*)p;

    uint64 i = *n;
    while(i <= 8)
    {
        int a = fib(i);

        uint64 oldTimer = gTimer;
        int b = time_sleep(a);

        assert(b == 0);
        assert((oldTimer - gTimer) <= (((uint64)a) + 2));
        assert((oldTimer - gTimer) >= ((uint64)a));

        i++;
    }
}

int sliceFirstCounter = 0;
int sliceSecondCounter = 0;

void testTimeSlice()
{
    putString("=== Testing \"testTimeSlice\"");
    putNewline();

    void doP(void*);
    void doQ(void*);

    uint64 n = 150000000;

    void doSliceFirst(void*);
    void doSliceSecond(void*);

    thread_t sliceFirst;
    thread_create(&sliceFirst, &doSliceFirst, &n);

    thread_t sliceSecond;
    thread_create(&sliceSecond, &doSliceSecond, &n);

    thread_join(sliceFirst);
    thread_join(sliceSecond);

    int dif = (sliceFirstCounter - sliceSecondCounter);
    if(dif < 0) // dif = abs(dif);
        dif = -dif;

    assert(dif < sliceFirstCounter * 8/10); // WARNING: not 100% check, depends on "n" and many other stuff

    putString("=== PASS in testing \"testTimeSlice\"");
    putNewline();
}


void doSliceFirst(void* n)
{
    uint64 oldTimer = gTimer;
    for(uint64 i=0; i<(*((uint64*)n)); i++)
    {
        if(oldTimer != gTimer)
        {
            __asm__ volatile("mv x10, x10");

            sliceFirstCounter++;
            oldTimer = gTimer;
        }
    }
    assert(sliceFirstCounter > 0); // "n" in test too small probably
}

void doSliceSecond(void* n)
{
    uint64 newN = *(uint64*)n;

    uint64 oldTimer = gTimer;
    for(uint64 i=0; i<newN; i++)
    {
        __asm__ volatile("mv x10, x10");
        assert((*((uint64*)n)) == newN);

        if(oldTimer != gTimer)
        {
            __asm__ volatile("mv x10, x10");

            sliceSecondCounter++;
            assert((((int)sliceFirstCounter) - ((int)sliceSecondCounter)) <= 1);
            oldTimer = gTimer;
        }

        __asm__ volatile("mv x10, x10");
    }

    assert(sliceSecondCounter > 0); // "n" in test too small probably
}

void testSyncCall()
{
    disableExternalInterrupts();

//    assert(MemAlloc::get()->getUserlandUsage() == IThread::initialUserMemoryUsage);

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

    thread_join(b);
    thread_join(a);

    putString("=== PASS in testing \"testSyncCall\"");
    putNewline();

    enableExternalInterrupts();
}

void doA(void* p)
{
    assert(*((int*)p) == 69);

    for(int i=0; i<=5; i++)
    {
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

//    assert(MemAlloc::get()->getUserlandUsage() == IThread::initialUserMemoryUsage);
    putString("=== PASS in testing system calls");
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

//    assert(MemAlloc::get()->getUserlandUsage() == IThread::initialUserMemoryUsage);
    putString("=== PASS in testing memory allocator");
    putNewline();
}
