#include "../h/c_api.h"
#include "../h/my_console.h"

void* mem_alloc(size_t size)
{
//    uint64 a = 1;
//    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (a));
    __asm__ volatile ("li a0, 1");

//    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&trapRoutine));
//    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (size));
    __asm__ volatile ("li a1, 123");

    __asm__ volatile ("ecall");

    return nullptr; // TODO
}

int mem_free(void*)
{
    assert(false);
    return 0;
}