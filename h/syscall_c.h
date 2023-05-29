#pragma once

#include "../lib/hw.h"

const int EOF = -1;

void* mem_alloc(size_t size);
int mem_free(void*);

class _thread;
typedef uint64 thread_t;

void thread_dispatch(); // WARNING: different than Scheduler::dispatchToNext
int thread_exit();
int thread_create(thread_t* handle, void(*start_routine)(void*), void*arg);
void thread_join(thread_t handle);

int time_sleep (time_t);

char getc();
void putc(char c);

uint64 test_call(uint64 n);

typedef uint64 sem_t;
int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
