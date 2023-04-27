#include "../lib/console.h"

#include "../h/alloc.h"
#include "../h/my_console.h"

extern "C" void trapRoutine();

int main()
{
//    putString("Start");
//    putNewline();

    MemAlloc* memAlloc = MemAlloc::get();
    memAlloc->allocMem(10);

    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&trapRoutine));
    __asm__ volatile ("csrs sstatus, 0x02");

    while(true)
    {

    }

    return 0;
}