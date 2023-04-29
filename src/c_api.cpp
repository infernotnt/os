#include "../h/c_api.h"

void* mem_alloc(size_t size)
{
    __asm__ volatile ("ld a0, 1");

//    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&trapRoutine));
    __asm__ volatile ("ld a1, %[name]" : [name] "r=" (size));

    __asm__ volatile ("ecall");
}

int mem_free(void*)
{
    assert(false);
}