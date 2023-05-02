#include "../h/trap.h"

int gTimer = 0;

uint64 testCall(uint64 n)
{
    return n * 2;
}

void cExternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

    int isExternal = (scause & (0x1UL << 63)) != 0;
    int cause = scause & (~(1UL << 63));

    assert(isExternal == 1);

    __asm__ volatile ("csrc sip, 0xA"); // clears the 9th bit in register sip. (the bit signifies external interupt)

    if (cause == 1)
    {
        if (gTimer % 10 == 0) {
            putString("Time: ");
            putInt(gTimer / 10);
            putString("s");
            putNewline();
        }
        gTimer++;
    }

    console_handler();
}

void cInternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

//    int isExternal = (scause & (0x1UL << 63)) != 0;
    int cause = scause & (~(1UL << 63));

    __asm__ volatile ("csrc sip, 0x2"); // clears the 1st bit in register sip. (the bit signifies internal

    if(cause != 8 && cause != 9)
    {
        putString("=== EXCEPETION occured: ");
        if(cause == 2)
        {
            putString(" Illegal instruction");
        }
        else if (cause == 5)
        {
            putString("Unallowed read adress");
        }
        else if (cause == 7)
        {
            putString("Unallowed write adress");
        }
        else
        {
            putString("UKNOWN EXCEPTION. ??????????????");
            assert(false);
        }
        putNewline();
        putString("Terminating kernel");
        putNewline();
        assert(false);
    }

    // if we are here it means it was a system call that caused the trap handler

    uint64 sepc;
    __asm__ volatile ("csrr %[name], sepc" : [name] "=r"(sepc));
    sepc += 4;
    __asm__ volatile ("csrw sepc, %[name]" : : [name] "r" (sepc));

    //dispatchSystemCall();
    uint64 code, parameter1;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(code));
    __asm__ volatile ("mv %[name], a1" : [name] "=r"(parameter1));

//    putString("=== system call. code= ");
//    putU64(code);
//    putString( " parameter1= ");
//    putU64(parameter1);
//    putNewline();
//
//    assert(code != 0);
//    assert(sizeof(void*) == 8);

    uint64 ret = -1;
    if(code == 1)
    {
        ret = (uint64)MemAlloc::get()->allocMem(parameter1);
    }
    else if (code == 2)
    {
        ret = (uint64)MemAlloc::get()->freeMem((void*)parameter1);
    }
#ifdef __DEBUG_MODE
    else if (code == 3) // Test call
    {
        ret = (uint64)testCall(parameter1);
    }
#endif
    else
    {
        assert(false); // unknown code
    }

    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (ret));

//    assert(1);
//    assert(isExternal == 0);
}