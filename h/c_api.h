#pragma once

#include "../lib/hw.h"

void* mem_alloc(size_t size);
int mem_free(void*);

void thread_dispatch(); // WARNING: different than Scheduler::dispatchToNext
int thread_exit();
//void thread_join(thread_t handle);

uint64 test_call(uint64 n);
