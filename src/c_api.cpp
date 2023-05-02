#include "../h/c_api.h"
#include "../h/my_console.h"

static uint64 helper1(uint64 code, uint64 parameter1)
{
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return ret;
}

void* mem_alloc(size_t size)
{
    return (void*)helper1(1, size);

}

uint64 test_call(uint64 n)
{
    return helper1(3, n);
}

int mem_free(void* ptr)
{
    return helper1(2, (uint64)ptr);
}