#pragma once

#include "../lib/hw.h"

#define BUFFER_SIZE 100

#define USE_MY_CONSOLE 1

class ISemaphore;

class IConsole // singleton class
{
public:
    static IConsole* get()
    {
        __asm__ volatile("mv x10, x10");
        static IConsole instance;
        return &instance;
    }

    void writeToConsole();
    void actuallyWriteToConsole();

    long int putBufferHead, putBufferTail;
    char putBuffer[BUFFER_SIZE];
    long int putBufferItems;

    char getBuffer[BUFFER_SIZE];
    long int getBufferItems;
    long int getBufferHead, getBufferTail;
    ISemaphore* inputSemaphore;

    void putc(char c);
    char getc();
    void consoleHandler();

private:
    IConsole()
        : putBufferHead(0), putBufferTail(0), putBufferItems(0), getBufferItems(0), getBufferHead(0), getBufferTail(0), inputSemaphore(nullptr)
    { }
};
