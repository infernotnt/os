#pragma once

#include "../lib/hw.h"

void* mem_alloc(size_t size);
int mem_free(void*);

class _thread;
typedef uint64 thread_t;

void thread_dispatch(); // WARNING: different than Scheduler::dispatchToNext
int thread_exit();
int thread_create(thread_t* handle, void(*start_routine)(void*), void*arg);
void thread_join(thread_t handle);

uint64 test_call(uint64 n);
