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

    while(true)
    {
//        int n = 1;
//        putInt(fib(n));
//        putNewline();
//        n++;
    }

    putString("Application ended");
    putNewline();
}