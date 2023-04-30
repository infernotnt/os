#pragma once

#include "../lib/hw.h"

void* mem_alloc(size_t size);
int mem_free(void*);
uint64 test_call(uint64 n);
