#include "../h/my_console.h"
#include "../h/c_api.h"
#include "../h/alloc.h"

uint64 fib(uint64 n)
{
    if(n == 1)
        return 1;
    else if (n == 0)
        return 0;
    else return fib(n-1) + fib(n-2);
}

void testSystemCalls()
{
    putString("=== Testing \"testSystemCalls\"");
    putNewline();

    int a = 0;
    int b = 0;

    for(int i=1; i<10; i++)
    {
        a++;
        b++;

        uint64 called = test_call(i);
        uint64 actual = fib(i);

        if(called != actual)
        {
            putString("test_call() error. i=");
            putInt(i);
            putString(" fib(i)=");
            putU64(actual);
            putString(" test_call(i)=");
            putU64(called);
            putNewline();
            assert(false);
        }

        assert(a == b);
    }

    assert(MemAlloc::get()->getUserlandUsage() == 0);
    putString("====== Done testing system calls");
    putNewline();
}

void testMemoryAllocator()
{
    putString("=== Testing \"testMemoryAllocator\"");
    putNewline();
    int n = 1000;
    int **a;
    a = (int**)mem_alloc(n * sizeof(void*));
    for(int i=0; i<n; i++)
    {
        a[i] = (int*)mem_alloc(sizeof(int));
        *a[i] = i;
    }

    int result = 0;
    for(int i=0; i<n; i++)
    {
        result += *a[i];
    }

//    uint64 vau = n * sizeof(void*);
//    if(n % MEM_BLOCK_SIZE != 0)
//        vau +=  MEM_BLOCK_SIZE - ((sizeof(int)* n) % MEM_BLOCK_SIZE);
//    vau += MEM_BLOCK_SIZE * n;
//    assert(MemAlloc::get()->getUserlandUsage() == vau);

//    putInt(result);
//    putNewline();

    assert(result == (n-1) * n / 2);

//    uint64 oldTaken = MemAlloc::get()->getUserlandUsage();
//    uint64 newTaken;

    int t;
    for(int i=0; i<n; i++)
    {
        t = mem_free(a[i]);
        assert(t == 0);

//        newTaken = MemAlloc::get()->getUserlandUsage();
//        assert(oldTaken - newTaken == MEM_BLOCK_SIZE);
    }

//    assert(MemAlloc::get()->getUserlandUsage() == MEM_BLOCK_SIZE);

    t = mem_free(a);
    assert(t == 0);

    assert(MemAlloc::get()->getUserlandUsage() == 0);
    putString("=== Done testing memory allocator");
    putNewline();
}

void userMain()
{
    putString("App started");
    putNewline();
    assert(MemAlloc::get()->getUserlandUsage() == 0);

    testSystemCalls();

    testMemoryAllocator();

    assert(MemAlloc::get()->getUserlandUsage() == 0);
    putString("App ended");
    putNewline();
}

void callFromUserMode(void (*f)(void))
{
    assert(false); // disabled because it gives an error after void main() finishes, idk why probably easy fix, pitaj na diskordu
    __asm__ volatile ("csrc sstatus, 0x9"); // set spp (previous privilegde) bit to 0 signifieng user mode
    __asm__ volatile ("csrc sstatus, 0x6"); // spie bit to 1 (enable interupts (idk if external or internal))
    __asm__ volatile ("csrw sepc, %[name]" : : [name] "r" (f));
    __asm__ volatile ("sret");
}
