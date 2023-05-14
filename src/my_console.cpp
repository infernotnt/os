//
// Created by os on 4/25/23.
//

#include "../h/my_console.h"

//inline void _assert(bool valid, const char* file, int line)

[[noreturn]] void stopKernel()
{
    putNewline();
    putString("====== Stopping the kernel ======");

    disableExternalInterrupts();

    volatile int a = 1;
    while(true)
    {
        a++;
    }
}

void putNewline()
{
    __putc('\n');
}

void putString(const char* s)
{
    int i=0;
    while(true)
    {
        if(s[i] == '\0')
            return;
        else __putc(s[i++]);
    }
}

void putBinary(uint64 n)
{
    assert(false);
    if(n < 0)
    {
        putString("===== FATAL PRINTING ERROR IN FUNCTION putU64. NEGATIVE VALUE. Stoping the kernel ====");
        putNewline();
        stopKernel();
    }
    else if(n == 0 || n == 1)
    {
        __putc('0' + n);
    }
    else
    {
       uint64 initial = 1 << 30;
       putString("AAA");
//       putInt(initial);
       putNewline();
       putInt(sizeof(uint64));

        bool alreadyWritten = false;
        while(initial > 0)
        {
            bool digit = n / initial;
            if (digit > 0 || alreadyWritten)
            {
                alreadyWritten = true;
                __putc(digit + '0');
                n = n % initial;
            }
            initial = ((initial) >> 1);
        }
    }
}

void putU64(uint64 n)
{
    if(n >= 0 and n <= 9)
        __putc('0' + n);
    else if (n < 0)
    {
        putString("===== FATAL PRINTING ERROR IN FUNCTION putU64. NEGATIVE VALUE. Stoping the kernel ====");
        putNewline();
        stopKernel();
    }
    else {
        uint64 initial = 1000000000000000000;

        if (n > initial || n <= 9)
        {
            putString("===== FATAL PRINTING ERROR IN FUNCTION putU64() Stoping the kernel ====");
            putNewline();
            stopKernel();
        }

        bool alreadyWritten = false;
        while(initial > 0)
        {
            int digit = n / initial;
            if (digit > 0 || alreadyWritten)
            {
                alreadyWritten = true;
                __putc(digit + '0');
                n = n % initial;
            }
            initial /= 10;
        }
    }
}

void putInt(int n)
{
    if(n >= 0 and n <= 9)
        __putc('0' + n);
    else if (n < 0)
    {
        __putc('-');
        putInt(-n);
    }
    else {
        int initial = 1000000000;

        if (n > initial || n <= 9)
        {
            putString("===== FATAL PRINTING ERROR IN FUNCTION putInt(int). Stoping the kernel ====");
            putNewline();
            stopKernel();
        }

        bool alreadyWritten = false;
        while(initial > 0)
        {
            int digit = n / initial;
            if (digit > 0 || alreadyWritten)
            {
                alreadyWritten = true;
                __putc(digit + '0');
                n = n % initial;
            }
            initial /= 10;
        }
    }

}
