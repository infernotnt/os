#pragma once

// this file is only to be included in trap.cpp

#include "../h/thread.h"
#include "../h/scheduler.h"
#include "../h/alloc.h"
#include "../h/my_console.h"
#include "../h/semaphore.h"

uint64 fib(uint64 n);
uint64 testCall(uint64 n);

void doTimerStuff();
void doSleepStuff();

void handleExeptions(uint64 cause);
uint64 handleSystemCall(uint64 code, uint64 parameter1, uint64 parameter2, uint64 parameter3, uint64 parameter4);