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

Thread kernelThread;
char kernelStack[ACTUAL_STACK_SIZE + 16];

void initializeKernelThread(Thread* p)
{
    p->pStackStart = (uint64*)((char*)kernelStack + ACTUAL_STACK_SIZE);
    if((uint64)p->pStackStart % 16 != 0)
    {
        p->pStackStart = (uint64*)(((char*)p->pStackStart) - (((uint64)p->pStackStart) % 16));
    }
    assert((uint64)p->pStackStart % 16 == 0);

    // sets context
    //p->context[1] = (uint64)&thread_exit;
    p->context[2] = (uint64)p->pStackStart;
    p->context[NR_REGISTERS] = (uint64)p->body;

    Thread::setPRunning(p);
}

int main()
{
    uint64* kernelStackPointer = (uint64*)((char*)kernelStack + ACTUAL_STACK_SIZE);
    if((uint64)kernelStackPointer % 16 != 0)
    {
        kernelStackPointer = (uint64*)(((char*)kernelStackPointer) - (((uint64)kernelStackPointer) % 16));
    }
    __asm__ volatile ("mv sp, %[a]" : : [a] "r"(kernelStackPointer));
//    __asm__ volatile ("mv %[name], a0" : [name] "=r"(code));
    assert(((uint64)kernelStackPointer) % 16 == 0);

    initializeKernelThread(&kernelThread); // treba posle disableExternal...

    disableExternalInterrupts();
    enableExternalInterrupts();

    initInterruptVector();
    doInitialAsserts();

    putString("Test call (ret=64)");
    putNewline();
    uint64 a = test_call(32);
    putString("Test: ");
    putU64((uint64)a);
    putNewline();

    putString("Calling mem_alloc");
    putNewline();
    void* p = mem_alloc(32);
    putString("ret of mem_alloc: ");
    putU64((uint64)p);
    putNewline();



    enableExternalInterrupts(); // temp location for debug


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