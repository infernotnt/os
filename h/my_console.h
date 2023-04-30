//
// Created by os on 4/25/23.
//

#ifndef PROJECT_BASE_V1_1_MY_CONSOLE_H
#define PROJECT_BASE_V1_1_MY_CONSOLE_H

#endif //PROJECT_BASE_V1_1_MY_CONSOLE_H

#include "../lib/console.h"
#include "../lib/hw.h"

void putString(const char* s);
void putInt(int n);
void putNewline();
void putU64(uint64 n);
void putBinary(uint64 n);

void _assert(bool valid, const char*, int);

// WARNING! DO NOT USE EXPRESSIONS IN "valid"
#define assert(valid) _assert(valid, __FILE__, __LINE__)
