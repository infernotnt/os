#pragma once

#include "../h/my_console.h"
#include "../h/syscall_c.h"
#include "../h/alloc.h"
#include "../h/thread.h"
#include "../h/semaphore.h"

void testSystemCalls();
void testMemoryAllocator();
void testSyncCall();
void testTimeSlice();
void testTimeSleep();
void testSemaphores();


void externalInterruptTest();

extern uint64 semVal;

extern int sliceFirstCounter;
extern int sliceSecondCounter;

uint64 fib(uint64);
volatile extern uint64 gTimer;

