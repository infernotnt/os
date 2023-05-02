#include "../h/alloc.h"
#include "../h/my_console.h"
#include "../h/trap.h"
#include "../h/c_api.h"

extern "C" void trapRoutine();
void userMain();


void main()
{
    disableExternalInterrupts();

    // should be before changing sstatus 0x2 bit
    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&trapRoutine));

    // set MODE to 1 - vector mode
    __asm__ volatile ("csrs stvec, 0x1");
    __asm__ volatile ("csrc stvec, 0x2");

    assert(sizeof(void*) == 8);
    assert((((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / 1024 / 1024) == 127);

    userMain();
}