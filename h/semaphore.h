#pragma once

#include "../lib/hw.h"
#include "../h/thread.h"

#define NR_MAX_SEMAPHORES 100

// this is the id/index of the first user available semaphore (the kernel uses the 0th one)
#define INIT_SEM 1

class ISemaphore
{
public:
    static uint64 nrSemaphores;
    static ISemaphore* pAllSemaphores[NR_MAX_SEMAPHORES];

    static int create(uint64* pId, uint64 initialValue);
    static int close(uint64 id);
    static int wait(uint64 id);
    static int signal(uint64 id);

    uint64 id;
    int value;
    IThread* pBlockedHead;
    bool isOpen;

    IThread* removeFromQueue();
    void addToQueue();
    ISemaphore() = delete;
};