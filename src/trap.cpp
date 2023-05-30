#include "../h/trap.h"

volatile uint64 gTimer = 0;

void cExternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

    int cause = scause & (~(1UL << 63));

    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    long int getItems = IConsole::get()->getBufferItems;
    long int putItems = IConsole::get()->putBufferItems;
    if(getItems < 0 || putItems < 0)
    {
        __asm__ volatile("mv x10, x10");
        assert(false);
    }

    if(cause == 1)
    {
        __asm__ volatile ("csrc sip, 0x2"); // clears the 2nd bit which signifies software interrupt (timer for project)
//        gTimer++;
        Scheduler::doSleepStuffOnTick();
        Scheduler::doTimeSliceAndGTimeOnTick(); // must be after doSleepStuffOnTick
    }
    else if (cause == 9)
    {
        __asm__ volatile("mv x10, x10");
        if(IThread::getPRunning()->id == BUSY_WAIT_THREAD_ID)
        {
            __asm__ volatile("mv x10, x10");
        }

#ifdef USE_MY_CONSOLE
        IConsole::get()->consoleHandler();
#else
        console_handler();
#endif
        __asm__ volatile("mv x10, x10");
    }
    else
    {
        assert(false); // unknown interupt
    }
}

void cInternalInterruptRoutine()
{
    uint64 scause;
    __asm__ volatile ("csrr %[name], scause" : [name] "=r"(scause));

    int cause = scause & (~(1UL << 63));

    uint64 code, parameter1, parameter2, parameter3, parameter4;

    __asm__ volatile ("mv %[name], a0" : [name] "=r"(code));
    __asm__ volatile ("mv %[name], a1" : [name] "=r"(parameter1));
    __asm__ volatile ("mv %[name], a2" : [name] "=r"(parameter2));
    __asm__ volatile ("mv %[name], a3" : [name] "=r"(parameter3));
    __asm__ volatile ("mv %[name], a4" : [name] "=r"(parameter4));

    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    handleExeptions(cause);

    IThread* pOld = IThread::getPRunning();

    uint64 ret = handleSystemCall(code, parameter1, parameter2, parameter3, parameter4);

    bool normalCallWithReturn = (code == 0x1) || (code == 0x2) || (code == 0x11) || (code == 0x12) || (code == 0x21) || (code == 0x22) || (code == 0x23) || (code == 0x24) || (code == 0x31) || (code == 0x41);
    bool customCallWithReturn = (code == 0x3);
    bool customCallWithoutReturn = (code == 0x4) || (code == 5);
    bool normalCallWithoutReturn = (code == 0x13) || (code == 0x14) || (code == 0x42);

    bool isSemWaitCall = (code == 0x23);
    if(isSemWaitCall == true) // dont touch the return in this case, as the semaphore changes it directly on the stack of the waiting thread
    {

    }
    else if(normalCallWithReturn || customCallWithReturn)
    { // system call with a return (not necessariliy 64bit)

        assert(ret != (uint64)-1); // not 100% check

        *( ((uint64*)(pOld->sp))+10 ) = ret; // set r0, or x10, to the return value

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

void handleExeptions(uint64 cause)
{
    if (cause != 8 && cause != 9)
    {
        kPutString("=== EXCEPETION occured: ");
        if (cause == 2)
        {
            kPutString(" Illegal instruction");
        }
        else if (cause == 5)
        {
            kPutString("Unallowed read adress");
        }
        else if (cause == 7)
        {
            kPutString("Unallowed write adress");
        }
        else
        {
            kPutString("UKNOWN EXCEPTION. ??????????????");
            assert(false);
        }

        kPutNewline();
        kPutString("Terminating kernel");
        kPutNewline();
        assert(false);
    }
}

uint64 handleSystemCall(uint64 code, uint64 parameter1, uint64 parameter2, uint64 parameter3, uint64 parameter4)
{
    uint64 ret = -1;
    if(code == 1)
    {
        // convert to bytes, because of ABI dumbness
        ret = (uint64)MemAlloc::get()->allocMem(parameter1 * MEM_BLOCK_SIZE);
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
//    else if (code == 5)
//    {
//        Scheduler::specialBusyWaitDispatch();
//    }
    else if (code == 17) // 17
    {
        *((int*)&ret) = IThread::createThread( *((uint64**)&parameter1), *((IThread::Body*)&parameter2), *((void**)&parameter3), (void*) parameter4);
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
    else if (code == 33) // sem_open
    {
        *((int*)&ret) = ISemaphore::create((uint64*)parameter1, parameter2);
    }
    else if (code == 34) // sem_close
    {
        *((int*)&ret) = ISemaphore::close(parameter1); // TODO: ovde gde su int-ovi parameter1 param2, .... mozda nista ne valja
    }
    else if (code == 35) // sem_wait
    {
        *((int*)&ret) = ISemaphore::wait(parameter1);
    }
    else if (code == 36) // sem_signal
    {
        *((int*)&ret) = ISemaphore::signal(parameter1);
    }
    else if (code == 49)
    {
        *((int*)&ret) = Scheduler::get()->sleep(parameter1);
    }
    else if (code == 65) // getc
    {
        *((char*)&ret) = IConsole::get()->getc();
    }
    else if (code == 66) // putc
    {
        IConsole::get()->putc(*((char*)&parameter1));
    }
    else
    {
        assert(false); // unknown code
        assert(false); // unknown code
        assert(false); // unknown code
        assert(false); // unknown code
    }

    return ret;
}

void __exeptionError()
{
    __asm__ volatile("mv x10, x10");
    assert(false);
}

uint64 testCall(uint64 n)
{
    return fib(n);
}


