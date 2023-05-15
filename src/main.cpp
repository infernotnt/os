#include "../h/alloc.h"
#include "../h/c_api.h"
#include "../h/thread.h"
#include "../h/scheduler.h"

extern "C" void trapRoutine();

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
    disableExternalInterrupts();
    initInterruptVector();
    Thread::setPRunning(&kernelThread);
    doInitialAsserts();

//    enableExternalInterrupts(); // should not be here but rather incorporated into user thread
    disableExternalInterrupts(); // temp
    kernelThread.id = 0;
    Thread::pAllThreads[0] = &kernelThread;

//    userMain();

    assert(fib(3) == 2);

    int argA = 69;
    int argB = 420;
    thread_t a;
    thread_t b;
    thread_create(&a, doA, &argA);
    thread_create(&b, doB, &argB);
    thread_dispatch();
    assert(Thread::pAllThreads[a]->id == 1);
    assert(Thread::pAllThreads[b]->id == 2);


    disableExternalInterrupts(); // shouldnt be here but rather should be implicit
    return 0;
}

void doA(void* p)
{
    assert(*((int*)p) == 69);

    for(int i=0; ; i++)
    {
        putString("A i=");
        putU64(i);
        putNewline();

        assert(fib(i) == test_call(i));
        if(i == 10)
        {
            break;
            assert(false);
        }
        thread_dispatch();
    }
}

void doB(void* p)
{
    assert(*((int*)p) == 420);
    for(int i=0; ; i++)
    {
        putString("B i=");
        putU64(i);
        putNewline();

        assert(fib(i) == test_call(i));
        thread_dispatch();
    }
}


void test1(void* p)
{
    uint64 i=4;
    while(true)
    {
        uint64 a = fib(i);
        uint64 b = test_call(i);
        assert(a == b);
        i++;
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

//void doA(void* p)
//{
//    while(1)
//    {
//        assert(fib(1) == 1);
////        userMain();
////        putString("inside doA()");
////        putNewline();
//        thread_dispatch();
//    }
//}
