#pragma once

#include "../lib/hw.h"

#define PUT_BUFFER_SIZE 1000

class Console // singleton class
{
public:
    static Console* get()
    {
        static Console instance;
        return &instance;
    }

    uint64 putBufferHead, putBufferTail;
    char putBuffer[PUT_BUFFER_SIZE];
    uint64 putBufferItems;

    void putc(char c);
    char getc();
    void consoleHandler();

private:
    Console()
        : putBufferHead(0), putBufferTail(0), putBufferItems(0)
    { }
};
