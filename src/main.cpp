#include "../h/alloc.h"
#include "../h/c_api.h"
#include "../h/thread.h"
#include "../h/c_api.h"

extern "C" void trapRoutine();

void userMain();
void doInitialAsserts();
void initInterruptVector();

uint64 fib(uint64 n);

Thread kernelThread;
char kernelStack[ACTUAL_STACK_SIZE + 16];

int main()
{
    disableExternalInterrupts();

    initInterruptVector();
    doInitialAsserts();

    Thread::setPRunning(&kernelThread);
    kernelThread.id = 0;
    Thread::pAllThreads[0] = &kernelThread;

    enableExternalInterrupts(); // mozda treba bez?

    userMain();

    disableExternalInterrupts(); // shouldnt be here but rather should be implicit

    return 0;
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
