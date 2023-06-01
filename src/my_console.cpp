#include "../h/my_console.h"
#include "../h/semaphore.h"

void IConsole::flush()
{
    while ((((*(char *) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) != 0) && putBufferItems > 0)
    {
        assert(((*(char *) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) == CONSOLE_TX_STATUS_BIT);
        *((char *) CONSOLE_TX_DATA) = putBuffer[putBufferTail];

        putBufferItems--;
        putBufferTail = (putBufferTail + 1) % BUFFER_SIZE;
    }
}

void IConsole::consoleHandler()
{
    assert(inputSemaphore != nullptr);

    int a = plic_claim();

    assert(a == 10);
//    assert(a == 0 || a != 0);

    if(a == 10)
    {
        if(getBufferItems < BUFFER_SIZE )
        {
            if (((*((char *) CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT) != 0)
            {
                assert(getBufferItems < BUFFER_SIZE - 1);

                uint64 fakeChar;
                *((char*)&fakeChar) = *(char *) CONSOLE_RX_DATA;
                char realChar = *((char*)&fakeChar);

                IThread* pNextBlocked = inputSemaphore->pBlockedHead;

                ISemaphore::signal(IConsole::get()->inputSemaphore->id);

                if(pNextBlocked != nullptr)
                {
                    *(pNextBlocked->sp + 10) = fakeChar;
                }
                else
                {
                    getBuffer[getBufferHead] = realChar;
                    getBufferHead = (getBufferHead + 1) % BUFFER_SIZE;
                    getBufferItems++;

                }
            }
        }
    }

    plic_complete(a);

}

void IConsole::putc(char c)
{
    if(putBufferItems >= BUFFER_SIZE)
        return; // dismiss new characters

    assert(putBufferItems < BUFFER_SIZE);

    assert(putBufferHead < BUFFER_SIZE && putBufferTail < BUFFER_SIZE);

    putBuffer[putBufferHead] = c;

    putBufferItems++;
    putBufferHead = (putBufferHead+1) % BUFFER_SIZE;
}

char IConsole::getc()
{
    assert(inputSemaphore != nullptr);
    char ret;

    if(getBufferItems > 0)
    {
        ret = getBuffer[getBufferTail];
        getBufferItems--;
        getBufferTail++;
    }
    else
    {
        ret = 69; // actual return comes from trap routine
    }

    ISemaphore::wait(inputSemaphore->id);

    return ret;
}

[[noreturn]] void stopKernel()
{
    __asm__ volatile("mv x10, x10");
    kPutNewline();
    kPutString("====== Stopping the kernel ======");

    IConsole::get()->flush();

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

void kPutInt(int n)
{
    if(n < 0)
    {
        IConsole::get()->putc('-');
        n = -n;
    }
    kPutU64(n);
}

void putNewline()
{
    putc('\n');
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
}

void putInt(int n)
{
    if(n>0)
        putU64(n);
    else
    {
        putc('-');
        putU64(-n);
    }
}
