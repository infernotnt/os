//
// Created by os on 4/25/23.
//

#include "../h/my_console.h"
#include "../h/syscall_c.h"

//inline void _assert(bool valid, const char* file, int line)

[[noreturn]] void stopKernel()
{
    __asm__ volatile("mv x10, x10");
    kPutNewline();
    kPutString("====== Stopping the kernel ======");

    IConsole::get()->toRunAfterLargeOutput();

    disableExternalInterrupts();

    volatile int a = 1;
    while(true)
    {
        a++;
    }
}

void kPutNewline()
{
    IConsole::get()->putc('\n');

    IConsole::get()->toRunAfterLargeOutput();
}

void kPutString(const char* s)
{
    int i=0;
    while(true)
    {
        if(s[i] == '\0')
            break;
        else IConsole::get()->putc(s[i++]);
    }

    IConsole::get()->toRunAfterLargeOutput();
}

void kPutU64(uint64 n)
{
    if(n >= 0 and n <= 9)
        IConsole::get()->putc('0' + n);
    else if (n < 0)
    {
        kPutString("===== FATAL PRINTING ERROR IN FUNCTION putU64. NEGATIVE VALUE. Stoping the kernel ====");
        kPutNewline();
        stopKernel();
    }
    else {
        uint64 initial = 1000000000000000000;

        if (n > initial || n <= 9)
        {
            kPutString("===== FATAL PRINTING ERROR IN FUNCTION putU64() Stoping the kernel ====");
            kPutNewline();
            stopKernel();
        }

        bool alreadyWritten = false;
        while(initial > 0)
        {
            int digit = n / initial;
            if (digit > 0 || alreadyWritten)
            {
                alreadyWritten = true;
                IConsole::get()->putc(digit + '0');
                n = n % initial;
            }
            initial /= 10;
        }
    }
}

void kPutInt(int)
{
    assert(false);
    IConsole::get()->toRunAfterLargeOutput();
}

void putNewline()
{
    putc('\n');
    IConsole::get()->toRunAfterLargeOutput();
}

void putString(const char* s)
{
    int i=0;
    while(true)
    {
        if(s[i] == '\0')
            break;
        else putc(s[i++]);
    }

    IConsole::get()->toRunAfterLargeOutput();
}

void putU64(uint64 n)
{
    if(n >= 0 and n <= 9)
        putc('0' + n);
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
                putc(digit + '0');
                n = n % initial;
            }
            initial /= 10;
        }
    }
    IConsole::get()->toRunAfterLargeOutput();
}

void putInt(int n)
{
    if(n >= 0 and n <= 9)
        putc('0' + n);
    else if (n < 0)
    {
        putc('-');
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
                putc(digit + '0');
                n = n % initial;
            }
            initial /= 10;
        }
    }
    IConsole::get()->toRunAfterLargeOutput();
}
