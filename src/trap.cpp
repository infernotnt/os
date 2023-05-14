#include "../h/trap.h"
#include "../h/thread.h"
#include "../h/scheduler.h"

int gTimer = 0;

uint64 fib(uint64 n);

uint64 testCall(uint64 n)
{
    return fib(n);
}
/*
   const int N = 1;
    int a, b;
    a = b = N*37;
    while(a < N*39)
    {
        a++;
        b++;
        assert(a == b);

        if(a % N == 0) {
            int out = fib(a/N);
//            putString("fib(");
//            putInt(a/N);
//            putString(") = ");
//            putInt(out);
//            putNewline();

            if(a/N == 38)
            {
                assert(out == 39088169);
            }

//            putString("ret= ");
//            putU64(ret);
//            putNewline();
//            putNewline();

        }
    }
    return fib(a / N);
//    return n * 2;
 */

void cExternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

    int isExternal = (scause & (0x1UL << 63)) != 0;
    int cause = scause & (~(1UL << 63));

    assert(isExternal == 1);

    __asm__ volatile ("csrc sip, 0xA"); // clears the 9th bit in register sip. (the bit signifies external interupt)

    if(&(Thread::getPRunning()->context[0]) != Thread::pRunningContext)
    {
        assert(false);
    }

    if (cause == 1)
    {
        Thread::timeSliceCounter++;
        gTimer++;

        if (gTimer % 10 == 0)
        {
            putString("Time: ");
            putInt(gTimer / 10);
            putString("s");
            putNewline();
        }

//        if (Thread::timeSliceCounter >= Thread::pRunning->getTimeSlice())
//        {
//            Thread::timeSliceCounter = 0;
//            Scheduler::dispatchToNext();
//        }
    }
    else if (cause == 9)
    {
        console_handler();
    }
    else
    {
        putString("=== Error: unknown external interrupt");
        putNewline();
        assert(false);
    }
}

void cInternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

//    int isExternal = (scause & (0x1UL << 63)) != 0;
    int cause = scause & (~(1UL << 63));

    __asm__ volatile ("csrc sip, 0x2"); // clears the 1st bit in register sip. (the bit signifies internal)

    uint64 code, parameter1;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(code));
    __asm__ volatile ("mv %[name], a1" : [name] "=r"(parameter1));


//    if(&(Thread::pRunning->context[0]) != Thread::pRunningContext) // temp, maybe changes registers?
//    {
//        __asm__ volatile ("mv x10, x10");
//        assert(false);
//    }

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


    uint64 ret = -1;
    if(code == 1)
    {
        ret = (uint64)MemAlloc::get()->allocMem(parameter1);
    }
    else if (code == 2)
    {
        ret = (uint64)MemAlloc::get()->freeMem((void*)parameter1);
    }
    else if (code == 0x12)
    {
        ret = Thread::exit();
    }
    else if (code == 0x13)
    {
//        if(Thread::userThreadExists)
//        {
//            Scheduler::dispatchToNext();
//        }
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
        assert(false); // unknown code
        assert(false); // unknown code
        assert(false); // unknown code
    }

    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (ret)); // upaliti ovo dole umesto ovoga (ovo obrisati)

//    bool normalCallWithReturn = (code == 0x1 || code == 0x2 || code == 0x11 || code == 0x12 || code == 0x21 || code == 0x22 || code == 0x23 || code == 0x24 || code == 0x31 || code == 0x41);
//    bool customCallWithReturn = (code == 0x3);
//    bool normalCallWithoutReturn = (code == 0x13 && code != 0x14 && code != 0x42);
//    if(normalCallWithReturn || customCallWithReturn)
//    { // system call with a return (not necessariliy 64bit)
//        __asm__ volatile ("mv a0, %[name]" : : [name] "r" (ret));
//    }
//    else if(normalCallWithoutReturn) // this exists for safety reasons
//    {
//        ;
//    }
//    else
//    {
//        assert(false);
//    }

}