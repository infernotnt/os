#include "../h/my_console.h"
#include "../h/alloc.h"
#include "../h/thread.h"
#include "../h/semaphore.h"
#include "../h/my_tests.h"

#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

void myUserMain()
{
    __asm__ volatile ("mv x10, x10");

    putString("=== App started");
    putNewline();

    testSystemCalls();
    testMemoryAllocator();
    testSyncCall();
//    testTimeSlice();
//    testTimeSleep();
    testSemaphores();

    putString("=== App ended");
    putNewline();
}
