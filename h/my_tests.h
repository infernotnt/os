#pragma once

#include "../h/my_console.h"
#include "../h/syscall_c.hpp"
#include "../h/alloc.h"
#include "../h/thread.h"
#include "../h/semaphore.h"

void testSystemCalls();
void testMemoryAllocator();
void testSyncCall();
void testTimeSlice();
void testTimeSleep();
void testSemaphores();

extern uint64 semVal;

extern int sliceFirstCounter;
extern int sliceSecondCounter;

uint64 fib(uint64);
extern uint64 gTimer;
