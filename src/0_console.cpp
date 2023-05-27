#include "../h/0_console.h"
#include "../h/my_console.h"

void Console::consoleHandler()
{
    int a = plic_claim();
    assert(a == 10);

    if( ((*((char *) CONSOLE_STATUS)) & CONSOLE_TX_STATUS_BIT) != 0 )
    {
        if(putBufferItems > 0)
        {
            *((char *) CONSOLE_TX_DATA) = putBuffer[putBufferTail];

            putBufferItems--;
            putBufferTail = (putBufferTail+1) % PUT_BUFFER_SIZE;
        }
    }

    if( ((*((char *) CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT) != 0 )
    {
        char c = *((char *) CONSOLE_TX_DATA); // ovde nista ne radim zapravo, samo retriev-ujem karakter
        assert(c != ' ');
    }

    plic_complete(10);
}

void Console::putc(char c)
{
    assert(putBufferItems < PUT_BUFFER_SIZE);

    putBuffer[putBufferHead] = c;

    putBufferItems++;
    putBufferHead = (putBufferHead+1) % PUT_BUFFER_SIZE;
}

char Console::getc()
{
    assert(false);
    return 'a';
}
