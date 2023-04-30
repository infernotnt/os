#include "../h/my_console.h"
#include "../h/c_api.h"

int fib(int n)
{
    if(n == 1)
        return 1;
    else if (n == 0)
        return 0;
    else return fib(n-1) + fib(n-2);
}

void userMain()
{
    putString("Application started");
    putNewline();

    mem_alloc(6969);

    const int n = 1000000;
    int a, b;
    a = b = n*37;
    while(true)
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

    putString("Application ended");
    putNewline();
}