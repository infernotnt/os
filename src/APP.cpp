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

    const int n = 1000000;
    int a, b;
    a = b = n*37;
    while(a < n*39)
    {
        a++;
        b++;
        assert(a == b);

        if(a % n == 0) {
            int out = fib(a/n);
            putString("fib(");
            putInt(a/n);
            putString(") = ");
            putInt(out);
            putNewline();

            if(a/n == 38)
            {
                assert(out == 39088169);
            }

            uint64 ret = (uint64)test_call(a / n);
            putString("ret= ");
            putU64(ret);
            putNewline();
            putNewline();

            assert(ret == (uint64)(a/n * 2));
        }
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

    MemAlloc::get()->printUserlandUsage();

    putInt(result);
    putNewline();
    assert(result == (n-1) * n / 2);

    int t;
    for(int i=0; i<n; i++)
    {
        t = mem_free(a[i]);
        assert(t == 0);
    }

    t = mem_free(a);
    assert(t == 0);

    MemAlloc::get()->printUserlandUsage();
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
