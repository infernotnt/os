#pragma once

#include "../lib/hw.h"

#define BUFFER_SIZE 1000

class IConsole // singleton class
{
public:
    static IConsole* get()
    {
        static IConsole instance;
        return &instance;
    }

    uint64 putBufferHead, putBufferTail;
    char putBuffer[BUFFER_SIZE];
    uint64 putBufferItems;

    char getBuffer[BUFFER_SIZE];
    uint64 getBufferItems;
    uint64 getBufferHead, getBufferTail;

    void putc(char c);
    char getc();
    void consoleHandler();

private:
    IConsole()
        : putBufferHead(0), putBufferTail(0), putBufferItems(0)
    { }
};
