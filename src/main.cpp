#include "../h/alloc.h"
#include "../h/c_api.h"
#include "../h/thread.h"
#include "../h/c_api.h"
#include "../h/scheduler.h"
#include "../h/0_console.h"

extern "C" void trapRoutine();

void myUserMain();
//void userMain();
void doInitialAsserts();
void initInterruptVector();

uint64 fib(uint64 n);
void testAsyncCall();

Thread kernelThread;
char kernelStack[ACTUAL_STACK_SIZE + 16];

void userWrapper(void* p)
{
    assert(p == nullptr);
    assert(&(Thread::getPRunning()->sp) == Thread::pRunningSp);

    enableExternalInterrupts();

    myUserMain();

//    for(int i=0; i<10; i++)
//    {
//        Console::get()->putc(i + '0');
//
//        Console::get()->putc(' ');
//        Console::get()->putc('b');
//        Console::get()->putc('a');
//        Console::get()->putc('b');
//        Console::get()->putc('a');
//
//        Console::get()->putc('\n');
//    }
}

int main()
{
    disableExternalInterrupts();

    initInterruptVector();
    doInitialAsserts();

    uint64* sp;
    __asm__ volatile ("mv %[name], sp" : [name] "=r"(sp));
    kernelThread.sp = sp;
    Thread::setPRunning(&kernelThread);
    assert(&(Thread::getPRunning()->sp) == Thread::pRunningSp);
    kernelThread.id = 0;
    Thread::pAllThreads[0] = &kernelThread;

    thread_t t;
    thread_create(&t, &userWrapper, nullptr);

    Thread::initialUserMemoryUsage = MemAlloc::get()->getUserlandUsage();

    plic_claim();
    plic_complete(10);

    __asm__ volatile ("li a0, 4"); // this is a system call that calls Thread::switchToUser()
    __asm__ volatile ("ecall");

    __asm__ volatile ("csrw sscratch, x0"); //TEST permissions

    return 0;
}

void doC(void* p)
{
    volatile uint64 i=0;
    extern uint64 gTimer;
    uint64 oldTimer = gTimer;
    while(i>=0)
    {
        if(gTimer != oldTimer)
        {
            __asm__ volatile ("mv x1, x1");
        }
        i++;
        oldTimer = gTimer;
    }
    __asm__ volatile ("mv x1, x1");
}

void testAsyncCall()
{
    enableExternalInterrupts();

    assert(MemAlloc::get()->getUserlandUsage() == 0);

//    void doB(void*);

    int argA = 69;
//    int argB = 420;
    thread_t a;
//    thread_t b;
    thread_create(&a, doC, &argA);
//    thread_create(&b, doB, &argB);
    thread_dispatch();

//    assert(Thread::pAllThreads[a]->id == Thread::pAllThreads[b]->id-1);
    assert(MemAlloc::get()->getUserlandUsage() == 0);
}

void doInitialAsserts()
{
    assert(sizeof(void*) == 8);
    assert((((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / 1024 / 1024) == 127);
    assert(DEFAULT_STACK_SIZE == 4096);
    assert(DEFAULT_TIME_SLICE == 2);
    assert(sizeof(char) == 1);
    assert(sizeof(Thread) < 1000);
    assert(sizeof(int) == 4);
    assert(sizeof(uint64) == 8);
}

void initInterruptVector()
{

// should be before changing sstatus 0x2 bit
    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r"(&trapRoutine));

// set MODE to 1 - vector mode
    __asm__ volatile ("csrs stvec, 0x1");
    __asm__ volatile ("csrc stvec, 0x2");
}
