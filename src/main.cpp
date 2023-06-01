#include "../h/alloc.h"
#include "../h/syscall_c.h"
#include "../h/thread.h"
#include "../h/scheduler.h"
#include "../h/my_console.h"
#include "../h/my_tests.h"

extern "C" void trapRoutine();

void myUserMain();
void userMain();
void doInitialAsserts();
void initInterruptVector();
void doBusyWaitThread(void*);
void initInputSemaphore();
void initializeKernelThread();
void initializeBusyWaitThread();
void initializeUserThread();
void initSemaphores();

IThread kernelThread;

void userWrapper(void* p)
{
    assert(p == nullptr);
    assert(&(IThread::getPRunning()->sp) == IThread::pRunningSp);
//    __asm__ volatile("csrw sscratch, 1"); // to check for permissions

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

    initSemaphores();
    initInputSemaphore();

    IThread* a = Scheduler::get()->pHead;
    assert(a->id == USER_THREAD_ID);

    uint64* sp;                                                 /// WARNING: this must be immediately before calling the user thread
    __asm__ volatile ("mv %[name], sp" : [name] "=r"(sp));
    kernelThread.sp = sp;

    __asm__ volatile ("li a0, 4"); // this is a system call that calls IThread::switchToUser()
    __asm__ volatile ("ecall");

    __asm__ volatile("csrw sscratch, 1"); // to check for permissions

    return 0;
}

void initSemaphores()
{
    for(int i=0; i<NR_MAX_SEMAPHORES; i++)
        ISemaphore::pAllSemaphores[i] = nullptr;
}

void initInputSemaphore()
{
    uint64 inputSemaphore=69;
    ISemaphore::create(&inputSemaphore, 0);
    assert(inputSemaphore == 0);
    IConsole::get()->inputSemaphore = ISemaphore::pAllSemaphores[inputSemaphore];
    IConsole::get()->inputSemaphore->pBlockedHead = nullptr;
}

void doBusyWaitThread(void* p)
{
    assert(p == nullptr);
    __asm__ volatile("mv x10, x10");

    volatile uint64 a;
    while(true)
    {
        assert(IThread::getPRunning()->id == BUSY_WAIT_THREAD_ID);
        assert(Scheduler::get()->pHead != IThread::getPRunning());
//        kPutString("Busy wait Thread");
//        kPutNewline();
        __asm__ volatile("mv x10, x10");
        a++;
    }

    assert(false); // should never get here
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