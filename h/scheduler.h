#pragma once

#include "../h/thread.h"

class Scheduler // singleton
{
public:
    static Scheduler* get()
    {
        static Scheduler instance;
        return &instance;
    }

    static void dispatchToNext(); // WARNING: different than sys. call dispatch()
    static void dispatchUserVersion();
    static IThread* getNext();
    static void put(IThread*);
    static void printState();

    IThread* pHead; // points to the one that is the next to be run

private:
    Scheduler()
        : pHead(nullptr)
    { }
};