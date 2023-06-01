#include "../h/syscall_c.h"

// temp
#include "../h/my_console.h"

// temp
#include "../h/thread.h"

uint64 fib(uint64);

int helperRet32P164P232(uint64 code, uint64 parameter1, int parameter2)
{
    __asm__ volatile ("mv a2, %[name]" : : [name] "r" (parameter2));
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return *((int*)&ret);
}

int helperRet32P164P264(uint64 code, uint64 parameter1, uint64 parameter2)
{
    __asm__ volatile ("mv a2, %[name]" : : [name] "r" (parameter2));
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return *((int*)&ret);
}

int helperRet32P164(uint64 code, uint64 parameter1)
{
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

    uint64 ret;
    __asm__ volatile ("mv %[name], a0" : [name] "=r"(ret));

    return *((int*)&ret);
}

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

int helperRet32P164P264P364P464(uint64 code, uint64 parameter1, uint64 parameter2, uint64 parameter3, uint64 parameter4)
{
    __asm__ volatile ("mv a4, %[name]" : : [name] "r" (parameter4));
    __asm__ volatile ("mv a3, %[name]" : : [name] "r" (parameter3));
    __asm__ volatile ("mv a2, %[name]" : : [name] "r" (parameter2));
    __asm__ volatile ("mv a1, %[name]" : : [name] "r" (parameter1));
    __asm__ volatile ("mv a0, %[name]" : : [name] "r" (code)); // WARNING: this instruction must be after the a1 instruction. Reason: if its before it can augment the argument

    __asm__ volatile ("ecall");

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
    if(size % MEM_BLOCK_SIZE != 0)
    {
        size += MEM_BLOCK_SIZE - (size % MEM_BLOCK_SIZE);
    }

    assert(size % MEM_BLOCK_SIZE == 0);

    size /= MEM_BLOCK_SIZE;

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
//    char* stackSpace = (char*) MemAlloc::get()->allocMem(ACTUAL_STACK_SIZE);
    char* stackSpace = (char*)mem_alloc(ACTUAL_STACK_SIZE);
    return helperRet32P164P264P364P464(0x11, (uint64)handle, (uint64)start_routine, (uint64)arg, (uint64)stackSpace);
}

void thread_join(thread_t handle)
{
    helperP164(0x14, handle);
}

int time_sleep(time_t time)
{
    return helperRet32P164(0x31, time);
}

char getc()
{
    return helperRet8(0x41);
}

void putc(char c)
{
    helperP164(0x42, *((uint64*)&c));
}

uint64 test_call(uint64 n)
{
    return helperRet64P164(3, n);
}

int sem_open(sem_t* handle, unsigned init)
{
    return helperRet32P164P232(0x21, (uint64)handle, *((int*)&init));
}

int sem_close(sem_t handle)
{
    return helperRet32P164(0x22, (uint64)handle);
}

int sem_wait(sem_t id)
{
    return helperRet32P164(0x23, (uint64)id);
}

int sem_signal(sem_t id)
{
    return helperRet32P164(0x24, (uint64)id);
}
