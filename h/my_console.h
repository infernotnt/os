#pragma once

#include "../h/risc.h"

#include "../lib/console.h"
#include "../lib/hw.h"

#define __DEBUG_MODE


void putString(const char* s);
void putInt(int n);
void putNewline();
void putU64(uint64 n);
void putBinary(uint64 n);
void stopKernel();

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

    stopKernel();
}

// WARNING! DO NOT USE EXPRESSIONS IN "valid"
#define assert(valid) _assert(valid, __FILE__, __LINE__)
