//
// Created by os on 4/25/23.
//

#include "../h/my_console.h"

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
            putString("FATAL PRINTING ERROR IN FUNCTION putInt(int)");
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

void _assert(bool valid, const char* file, int line)
{
    if(valid)
        return;

    putString("===================== ASSERT file: ");
    putString(file);
    putString("    line: ");
    putInt(line);
    putString("   ===========================");
    putNewline();
}
