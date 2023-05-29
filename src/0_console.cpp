#include "../h/0_console.h"
#include "../h/my_console.h"
#include "../h/semaphore.h"

void IConsole::toRunAfterLargeOutput()
{
    assert(inputSemaphore != nullptr);

#ifdef USE_MY_CONSOLE
    while (((*(char *) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) != 0)
    {
        if (putBufferItems > 0)
        {
            assert(((*(char *) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) == CONSOLE_TX_STATUS_BIT);
            *((char *) CONSOLE_TX_DATA) = putBuffer[putBufferTail];

            putBufferItems--;
            putBufferTail = (putBufferTail + 1) % BUFFER_SIZE;
        }
        else break;
    }
#endif
}

//void IConsole::writeToConsole()
//{
//}


void IConsole::consoleHandler()
{
    assert(inputSemaphore != nullptr);

    int a = plic_claim();

//    assert(a == 10);
//    assert(a == 0 || a != 0);

    if(a == 10)
    {
        if(getBufferItems < BUFFER_SIZE )
        {
            if (((*((char *) CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT) != 0)
            {
                assert(getBufferItems < BUFFER_SIZE - 1); // temp

                char c = *(char *) CONSOLE_RX_DATA; // ovde nista ne radim zapravo, samo retriev-ujem karakter

                if(inputSemaphore->pBlockedHead != nullptr)
                {
                    *((inputSemaphore->pBlockedHead->sp) + 10) = *((uint64*)&c);
                }
                else
                {
                    getBuffer[getBufferHead] = c;
                    getBufferHead = (getBufferHead + 1) % BUFFER_SIZE;
                    getBufferItems++;

                }
                ISemaphore::signal(IConsole::get()->inputSemaphore->id);
            }
        }
    }

    plic_complete(a);

}

void IConsole::putc(char c)
{
    assert(inputSemaphore != nullptr);

    if(!(putBufferItems < BUFFER_SIZE)) // temp, equivalent to assert below
    {
        __asm__ volatile("mv x10, x10");
        assert(false);
    }
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
