#include "../h/syscall_c.hpp"
#include "../h/my_console.h"
#include "../h/thread.h"
#include "../h/0_console.h"
#include "../lib/console.h"

uint64 fib(uint64);

char helperRet8(uint64 code)
{

    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return *((char*)&ret);
}

void helperP164(uint64 code, uint64 parameter1)
{
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");
}

int helperRet32P164P264P364(uint64 code, uint64 parameter1, uint64 parameter2, uint64 parameter3)
{
    __asm__ volatile ("mv a3, %[name]" : : [name] "r" (parameter3));
    __asm__ volatile ("mv a2, %[name]" : : [name] "r" (parameter2));
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    __asm__ volatile("mv x10, x10"); // temp

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return *((int*)&ret);
}

static uint64 helperRet64P164(uint64 code, uint64 parameter1) // returns 64bit and takes a 64bit parameter
{
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    __asm__ volatile ("mv x10, x10");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return ret;
}

static int helperRet32(uint64 code) // returns 64bit and takes no params
{
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code));

    __asm__ volatile ("ecall");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return *((int*)&ret);
}

static void helper(uint64 code) // returns nothing and takes no parameters (eg thread_dispatch)
{
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv x10, x10");
}

void* mem_alloc(size_t size)
{
    return (void*) helperRet64P164(1, size);
}

int mem_free(void* ptr)
{
    return helperRet64P164(2, (uint64) ptr);
}

void thread_dispatch() // WARNING: different than Scheduler::dispatchToNext()
{
    helper(0x13);
}

int thread_exit()
{
    return helperRet32(0x12);
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void*arg)
{
    return helperRet32P164P264P364(0x11, (uint64)handle, (uint64)start_routine, *((uint64*)&arg));
}

void thread_join(thread_t handle)
{
    helperP164(0x14, handle);
}

uint64 test_call(uint64 n)
{
    return helperRet64P164(3, n);
}

char getc()
{
//    return __getc();
    return 'a';
}

void putc(char c)
{
//    uint64 a;
//    *((char*)(&a)) = c;
//
//    helperP164(0x42, a);

//    __putc(c);
    IConsole::get()->putc(c);
}