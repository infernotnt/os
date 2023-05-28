#include "../h/alloc.h"
#include "../h/syscall_c.hpp"
#include "../h/thread.h"
#include "../h/scheduler.h"
#include "../h/0_console.h"

extern "C" void trapRoutine();

void myUserMain();
void doInitialAsserts();
void initInterruptVector();

uint64 fib(uint64 n);

IThread kernelThread;

void userWrapper(void* p)
{
    assert(p == nullptr);
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    enableExternalInterrupts();

    myUserMain();
}

int main()
{
    disableExternalInterrupts();

    initInterruptVector();
    doInitialAsserts();

    uint64* sp;
    __asm__ volatile ("mv %[name], sp" : [name] "=r"(sp));
    kernelThread.sp = sp;
    IThread::setPRunning(&kernelThread);
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);
    kernelThread.id = 0;
    IThread::pAllThreads[0] = &kernelThread;

    thread_t t;
    thread_create(&t, &userWrapper, nullptr);

    IThread::initialUserMemoryUsage = MemAlloc::get()->getUserlandUsage();

    plic_claim();
    plic_complete(10);

    __asm__ volatile ("li a0, 4"); // this is a system call that calls IThread::switchToUser()
    __asm__ volatile ("ecall");

    __asm__ volatile ("csrw sscratch, x0"); //TEST permissions

    return 0;
}

void doC(void* p)
{
    volatile uint64 i=0;
    extern uint64 gTimer;
    uint64 oldTimer = gTimer;
    while(i>=0)
    {
        if(gTimer != oldTimer)
        {
            __asm__ volatile ("mv x1, x1");
        }
        i++;
        oldTimer = gTimer;
    }
    __asm__ volatile ("mv x1, x1");
}

void testAsyncCall()
{

    assert(MemAlloc::get()->getUserlandUsage() == 0);

//    void doB(void*);

    int argA = 69;
//    int argB = 420;
    thread_t a;
//    thread_t b;
    thread_create(&a, doC, &argA);
//    thread_create(&b, doB, &argB);
    thread_dispatch();

//    assert(IThread::pAllThreads[a]->id == IThread::pAllThreads[b]->id-1);
    assert(MemAlloc::get()->getUserlandUsage() == 0);
}

void doInitialAsserts()
{
    assert(sizeof(void*) == 8);
    assert((((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / 1024 / 1024) == 127);
    assert(DEFAULT_STACK_SIZE == 4096);
    assert(DEFAULT_TIME_SLICE == 2);
    assert(sizeof(char) == 1);
    assert(sizeof(IThread) < 1000);
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


void consoleStuff(void* p)
{
    assert(false);

    assert(p == nullptr);

    IConsole* cons = IConsole::get();

    while(true)
    {
        thread_dispatch();

        bool readyWrite = false;
        bool readyRead = false;

//        int a = plic_claim();
//
//        if (a == 0)
//            continue;
//
//        assert(a == 10);

        if (((*((char *) CONSOLE_STATUS)) & CONSOLE_TX_STATUS_BIT) != 0) {
            readyWrite = true;
            if (cons->putBufferItems > 0)
            {
                int a = plic_claim();

                char output = cons->putBuffer[cons->putBufferTail];
                *((char *) CONSOLE_TX_DATA) = output;

                __asm__ volatile ("mv x10, x10");
                assert(a == 10);

                cons->putBufferItems--;
                cons->putBufferTail = (cons->putBufferTail + 1) % BUFFER_SIZE;
            }
        }

        if (((*((char *) CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT) != 0)
        {
            assert(false);
            assert(cons->getBufferItems < BUFFER_SIZE - 1);

            readyRead = true;
            char c = *((char *) CONSOLE_TX_DATA); // ovde nista ne radim zapravo, samo retriev-ujem karakter

            cons->getBuffer[cons->getBufferHead] = c;

            cons->getBufferHead = (cons->getBufferHead + 1) % BUFFER_SIZE;
            cons->getBufferItems++;
        }

        __asm__ volatile ("mv x10, x10");

        plic_complete(10);

        assert(readyRead || readyWrite);
    }
}
