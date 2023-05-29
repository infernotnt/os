#pragma once

#include "../h/risc.h"
#include "../lib/hw.h"

#include "../h/0_console.h"

// for debug system calls
#define __DEBUG_MODE

//#define __DEBUG_PRINT


void putString(const char* s);
void putInt(int n);
void putNewline();
void putU64(uint64 n);
void putBinary(uint64 n);

void kPutNewline();
void kPutString(const char*);
void kPutU64(uint64);
void kPutInt(int);

[[noreturn]] void stopKernel();

inline void _assert(bool valid, const char* file, int line) // MUST be inline
{
    if(valid)
        return;

    putString("===================== ASSERT file: ");
    putString(file);
    putString("    line: ");
    putInt(line);
    putString("   ===========================");
    putNewline();

    IConsole::get()->writeToConsole();


    stopKernel();
}

// WARNING! DO NOT USE EXPRESSIONS IN "valid"
#define assert(valid) _assert(valid, __FILE__, __LINE__)
