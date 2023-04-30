#include "../h/risc.h"

void enableExternalInterrupts()
{
    __asm__ volatile ("csrs sstatus, 0x02"); // sets the "sie" bit to 1
}

void disableExternalInterrupts()
{
    __asm__ volatile ("csrc sstatus, 0x02"); // sets the "sie" bit to 0
}
