#include "../h/alloc.h"
#include "../h/c_api.h"
#include "../h/thread.h"

extern "C" void trapRoutine();
extern "C" void newAsmTrap();

void userMain();
void doA(void*);
void doB(void*);
void doInitialAsserts();
void initInterruptVector();

uint64 fib(uint64 n);

Thread kernelThread;
char kernelStack[ACTUAL_STACK_SIZE + 16];

int main()
{
    __asm__ volatile ("mv %[name], sp" : [name] "=r"(kernelThread.pStackStart)); // mozda netacno
    disableExternalInterrupts();
    initInterruptVector();
    Thread::setPRunning(&kernelThread);
    doInitialAsserts();

    putString("Prvi");
    putNewline();
    uint64 a = test_call(1);
    putString("Drugi about to be executed");
    putNewline();

    uint64 b = test_call(1);
    putString("======== Drugi gotov");
//    assert(c == 1);

    assert(a == 1);
    assert(b == 1);

    assert(false);

    for(int i=1; i<5; i++)
    {
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
    }

    putString("Calling mem_alloc");
    putNewline();
    void* p = mem_alloc(32);
    putString("ret of mem_alloc: ");
    putU64((uint64)p);
    putNewline();

//    userMain();

//    enableExternalInterrupts(); // temp location for debug

    thread_t t1;
    int Aarg = 3;

    putString("Creating thread");
    putNewline();
    Thread::createThread(&t1, &doA, &Aarg);

    putString("Thread created");
    putNewline();
    //Thread::createThread(&t2, &doB, &Aarg);

    putString("Going into the loop");
    putNewline();
    int i=0;
    while(1)
    {
//        thread_dispatch();
        i++;
    };

    disableExternalInterrupts();
    return 0;
}

void doA(void* p)
{
    while(1)
    {
        putString("inside doA()");
        putNewline();
        thread_dispatch();
    }
}

void doB(void* p)
{
    while(1)
    {
        putString("B");
        putNewline();
        thread_dispatch();
    }
}

void doInitialAsserts()
{
    assert(sizeof(void*) == 8);
    assert((((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / 1024 / 1024) == 127);
    assert(DEFAULT_STACK_SIZE == 4096);
    assert(DEFAULT_TIME_SLICE == 2);
    assert(sizeof(char) == 1);
    assert(sizeof(Thread) < 1000);
}

void initInterruptVector()
{

// should be before changing sstatus 0x2 bit
    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r"(&trapRoutine));

// set MODE to 1 - vector mode
    __asm__ volatile ("csrs stvec, 0x1");
    __asm__ volatile ("csrc stvec, 0x2");
}