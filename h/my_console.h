#pragma once

#include "../lib/hw.h"
#include "../h/risc.h"

// switches from my console to theirs
#define USE_MY_CONSOLE 1

// for some debug printing (not assert)
#define __DEBUG_PRINT 1

// for debug system calls
#define __DEBUG_MODE

#define BUFFER_SIZE 1000

// WARNING! DO NOT USE EXPRESSIONS IN "valid"
#define assert(valid) _assert(valid, __FILE__, __LINE__)

class ISemaphore;

class IConsole // singleton class
{
public:
    static IConsole* get()
    {
        __asm__ volatile("mv x10, x10");
        __asm__ volatile("mv x10, x10");
        __asm__ volatile("mv x10, x10");
        __asm__ volatile("mv x10, x10");
        __asm__ volatile("mv x10, x10");
        static IConsole instance;
        return &instance;
    }

//    void writeToConsole();
    void toRunAfterLargeOutput();

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

void putString(const char* s);
void putInt(int n);
void putNewline();
void putU64(uint64 n);

void kPutNewline();
void kPutString(const char*);
void kPutU64(uint64);
void kPutInt(int);

[[noreturn]] void stopKernel();

inline void _assert(bool valid, const char* file, int line) // MUST be inline
{
    if(valid)
        return;

    __asm__ volatile ("mv x10, x10");

    kPutString("===================== ASSERT file: ");
    kPutString(file);
    kPutString("    line: ");
    kPutInt(line);
    kPutString("   ===========================");
    kPutNewline();

    IConsole::get()->toRunAfterLargeOutput();

    stopKernel();
}
