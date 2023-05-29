#include "../h/0_console.h"
#include "../h/my_console.h"

void IConsole::writeToConsole()
{
#ifdef USE_MY_CONSOLE
    while(( (*(char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT ) != 0)
    {
        if(putBufferItems > 0)
        {
            assert(( (*(char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT ) == CONSOLE_TX_STATUS_BIT);
            *((char *) CONSOLE_TX_DATA) = '1';
//            *((char *) CONSOLE_TX_DATA) = putBuffer[putBufferTail];

            putBufferItems--;
            putBufferTail = (putBufferTail+1) % BUFFER_SIZE;
        }
        else break;
    }
#endif
}

void IConsole::consoleHandler()
{
    int a = plic_claim();

//    assert(a == 10);
//    assert(a == 0 || a != 0);

    if(a == 10)
    {
        if(( (*(char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT ) != 0)
        {
            assert(getBufferItems < BUFFER_SIZE - 1);

            char c = *(char*)CONSOLE_TX_DATA; // ovde nista ne radim zapravo, samo retriev-ujem karakter

            getBuffer[getBufferHead] = c;

            getBufferHead = (getBufferHead + 1) % BUFFER_SIZE;
            getBufferItems++;
        }
    }

    plic_complete(a);

}

void IConsole::putc(char c)
{
    assert(putBufferItems < BUFFER_SIZE);
    assert(putBufferHead < BUFFER_SIZE && putBufferTail < BUFFER_SIZE);

    putBuffer[putBufferHead] = c;

    putBufferItems++;
    putBufferHead = (putBufferHead+1) % BUFFER_SIZE;
}

char IConsole::getc()
{
//    while(getBufferItems < 0)
//    assert(getBufferItems > 0);

    disableExternalInterrupts(); // TODO: remove after making this a sys call
    char ret;

    while(getBufferItems == 0)
    { }

    ret = getBuffer[getBufferTail];

    getBufferItems--;
    getBufferTail = (getBufferTail + 1) % BUFFER_SIZE;

    enableExternalInterrupts(); // TODO: remove this after making this a sys call
    return ret;
}
