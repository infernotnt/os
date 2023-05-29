#include "../h/alloc.h"
#include "../h/syscall_c.h"
#include "../h/thread.h"
#include "../h/scheduler.h"
#include "../h/0_console.h"
#include "../h/my_tests.h"

extern "C" void trapRoutine();

void myUserMain();
void userMain();
void doInitialAsserts();
void initInterruptVector();
void doBusyWaitThread(void*);
void initInputSemaphore();
void doMainTest();
void initializeKernelThread();
void initializeBusyWaitThread();
void initializeUserThread();

void externalInterruptTest();

IThread kernelThread;

void userWrapper(void* p)
{
    assert(p == nullptr);
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);

    __asm__ volatile("li t1, 512");         // temp: disables console interrupt
    __asm__ volatile("csrc sie, t1");
    enableExternalInterrupts();

    myUserMain();

//    userMain();
}

int main()
{
    disableExternalInterrupts();
    initInterruptVector();
    doInitialAsserts();

    initializeKernelThread();

    initializeBusyWaitThread();
    initializeUserThread();

    initInputSemaphore();

    IThread* a = Scheduler::get()->pHead;
    assert(a->id == USER_THREAD_ID);


//    enableExternalInterrupts();
//    volatile uint64 sss;
//    while(sss>=0)
//    {
//        char c = getc();
//        putc(c+1);
//        assert(c != 1);
//    }
//    externalInterruptTest();
//    disableExternalInterrupts();

//    doMainTest();

    uint64* sp;                                                 /// WARNING: this must be immediately before calling the user thread
    __asm__ volatile ("mv %[name], sp" : [name] "=r"(sp));
    kernelThread.sp = sp;

    __asm__ volatile ("li a0, 4"); // this is a system call that calls IThread::switchToUser()
    __asm__ volatile ("ecall");

    return 0;
}

void initInputSemaphore()
{
    uint64 inputSemaphore=69;
    ISemaphore::create(&inputSemaphore, 0);
    assert(inputSemaphore == 0);
    IConsole::get()->inputSemaphore = ISemaphore::pAllSemaphores[inputSemaphore];
}

void doBusyWaitThread(void* p)
{
    assert(p == nullptr);
    __asm__ volatile("mv x10, x10");

    volatile uint64 a;
    while(a++)
    {
        assert(IThread::getPRunning()->id == BUSY_WAIT_THREAD_ID);

        __asm__ volatile("mv x10, x10");
        if(IThread::getPRunning()->pNext != nullptr)
        {
            __asm__ volatile("mv x10, x10");
            thread_dispatch();
        }
    }
}

void initializeUserThread()
{
    thread_t userThread;
    thread_create(&userThread, &userWrapper, nullptr);
    assert(IThread::pAllThreads[userThread]->id == USER_THREAD_ID);
}

void initializeBusyWaitThread()
{
    thread_t busyWaitThread;
    thread_create(&busyWaitThread, &doBusyWaitThread, nullptr);
    assert(IThread::pAllThreads[busyWaitThread]->id == BUSY_WAIT_THREAD_ID);
    Scheduler::get()->getNext();                                                 // get this function out of the Scheduler algorithm
}

void initializeKernelThread()
{
    IThread::setPRunning(&kernelThread);
    kernelThread.id = 0;
    IThread::pAllThreads[0] = &kernelThread;
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);
}

void doMainTest()
{

    volatile uint64 k = 10;

    void testSystemCalls();
    void testMemoryAllocator();
    void testSyncCall();
    void testSemaphores();

    testSystemCalls();
    testMemoryAllocator();
    testSyncCall();
    testSemaphores();

    while(k++)
    {
        __asm__ volatile ("mv x10, x10");
    }

    /// ----

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
    assert(sizeof(unsigned long) == sizeof(uint64));
    assert(sizeof(unsigned) == sizeof(int));
    assert(USER_THREAD_ID > BUSY_WAIT_THREAD_ID); // user thread should have the largest thread id from all the threads that the user himself did not create explicitly
    assert(sizeof(long int) == sizeof(uint64));
}

void initInterruptVector()
{

// should be before changing sstatus 0x2 bit
    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r"(&trapRoutine));

// set MODE to 1 - vector mode
    __asm__ volatile ("csrc stvec, 0x2");
    __asm__ volatile ("csrs stvec, 0x1");
}