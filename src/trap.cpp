#include "../h/thread.h"
#include "../h/scheduler.h"
#include "../h/alloc.h"
#include "../h/0_console.h"

#include "../lib/console.h"

uint64 gTimer = 0;

uint64 fib(uint64 n);

uint64 testCall(uint64 n)
{
    return fib(n);
}

void doTimerStuff()
{
    IThread::timeSliceCounter++;
    if(IThread::timeSliceCounter == 2)
    {
        IThread::timeSliceCounter = 0;
        Scheduler::dispatchUserVersion();
    }

    gTimer++;
    if (gTimer % 10 == 0)
    {
        putString("Time: ");
        putInt(gTimer / 10);
        putString("s");
        putNewline();
    }
}

void cTimerInterruptRoutine()
{
    __asm__ volatile ("mv x10, x10");

    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

    int cause = scause & (~(1UL << 63));

    __asm__ volatile ("csrc sip, 0xA"); // clears the 9th bit in register sip. (the bit signifies external interupt)
//    __asm__ volatile ("csrc sstatus, 0x02");

    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);
    if(cause == 1)
    {
        doTimerStuff();
//        doUnsleepThreads();
    }
    else if (cause == 9)
    {
//        console_handler();
        IConsole::get()->consoleHandler();
    }
    else
    {
        assert(false); // unknown interupt
    }

    __asm__ volatile("mv x10, x10");
}


void cInternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

//    int isExternal = (scause & (0x1UL << 63)) != 0;
    int cause = scause & (~(1UL << 63));

    __asm__ volatile ("csrc sip, 0x2"); // clears the 1st bit in register sip. (the bit signifies internal)

    uint64 code, parameter1, parameter2, parameter3;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(code));
    __asm__ volatile ("mv %[name], a1" : [name] "=r"(parameter1));
    __asm__ volatile ("mv %[name], a2" : [name] "=r"(parameter2));
    __asm__ volatile ("mv %[name], a3" : [name] "=r"(parameter3));


    __asm__ volatile ("mv x1, x1");

//    IThread::getPRunning()->sp = IThread::runningSp;
//    uint64* sp = IThread::getPRunning()->sp;
//    uint64* runningSp = IThread::runningSp;
//    if(sp != runningSp) // temp, maybe changes registers?
//    {
//        __asm__ volatile ("mv x10, x10");
//        assert(false);
//    }
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    __asm__ volatile ("mv x1, x1");

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

    IThread* pOld = IThread::getPRunning();

    uint64 ret = -1;
    if(code == 1)
    {
        ret = (uint64)MemAlloc::get()->allocMem(parameter1);
    }
    else if (code == 2)
    {
        *((int*)&ret) = MemAlloc::get()->freeMem((void*)parameter1);
    }
#ifdef __DEBUG_MODE
    else if (code == 3) // Test call
    {
        ret = (uint64)testCall(parameter1);
    }
#endif
    else if (code == 4)
    {
        IThread::switchToUser();
    }
    else if (code == 17) // 17
    {
        *((int*)&ret) = IThread::createThread((uint64*)parameter1, (IThread::Body)parameter2, (void*)parameter3);
    }
    else if (code == 18)
    {
        __asm__ volatile("mv x10, x10");
        *((int*)&ret) = IThread::exit();
        assert(*((int*)&ret) == 0);
    }
    else if (code == 19) // 19, thread_dispatch
    {
        Scheduler::dispatchUserVersion();

    }
    else if (code == 20)
    {
        IThread::join((uint64)parameter1);
    }
    else if (code == 65) // getc
    {

    }
    else if (code == 66) // putc
    {

    }
    else
    {
        assert(false); // unknown code
        assert(false); // unknown code
        assert(false); // unknown code
        assert(false); // unknown code
    }

//    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (ret)); // upaliti ovo dole umesto ovoga (ovo obrisati)

    bool normalCallWithReturn = (code == 0x1) || (code == 0x2) || (code == 0x11) || (code == 0x12) || (code == 0x21) || (code == 0x22) || (code == 0x23) || (code == 0x24) || (code == 0x31) || (code == 0x41);
    bool customCallWithReturn = (code == 0x3);
    bool customCallWithoutReturn = (code == 0x4);
    bool normalCallWithoutReturn = (code == 0x13) || (code == 0x14) || (code == 0x42);
    if(normalCallWithReturn || customCallWithReturn)
    { // system call with a return (not necessariliy 64bit)

        assert(ret != (uint64)-1); // not 100% check

        *(((uint64*)(pOld->sp))+10) = ret; // set r0, or x10, to the return value

//        __asm__ volatile ("mv a0, %[name]" : : [name] "r" (ret));
    }
    else if(normalCallWithoutReturn || customCallWithoutReturn) // this exists for safety reasons
    {
        ;
    }
    else
    {
        assert(false);
    }

    __asm__ volatile ("mv x10, x10");
}



//    IThread::timeSliceCounter++;

//        if (IThread::timeSliceCounter >= IThread::pRunning->getTimeSlice())
//        {
//            IThread::timeSliceCounter = 0;
//            Scheduler::dispatchToNext();
//        }

void  cConsoleInterruptRoutine()
{
    assert(false);
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

//    int isExternal = (scause & (0x1UL << 63)) != 0;
    int cause = scause & (~(1UL << 63));


    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    assert(cause == 9);

    __asm__ volatile ("csrc sip, 0xA"); // clears the 9th bit in register sip. (the bit signifies external interupt)

//    console_handler();
    __asm__ volatile ("mv x10, x10");

//    IConsole::get()->consoleHandler();

}

