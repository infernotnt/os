#include "../h/alloc.h"
#include "../h/my_console.h"
#include "../h/trap.h"
#include "../h/c_api.h"

extern "C" void trapRoutine();
void userMain();

void main()
{
    // should be before changing sstatus 0x2 bit
    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&trapRoutine));

    // set MODE to 1 - vector mode
    __asm__ volatile ("csrs stvec, 0x1");
    __asm__ volatile ("csrc stvec, 0x2");

    enableExternalInterrupts();

    int i = 999;
    while(i<3)
    {
        i++;
        i++;
        i++;
        uint64 ret = (uint64)mem_alloc(i);
        putString("ret= ");
        putU64(ret);
        putNewline();
        putNewline();
        putNewline();
        putNewline();
    }
    userMain();
}