#include "SerialIO.h"

#include <string.h>

char SerialIO::ReadChar()
{
    char c = '\0';
    Read(&c,1);
    return c;
}

size_t SerialIO::ReadLine(std::string &s)
{
    size_t n = 0;
    while (true)
    {
        char c = '\0';
        if (Read(&c, sizeof(char)) <= 0 )
            break;
        if (c == '\n' || c == '\r' || c == '\0')
            break;
        s += c;
        n++;
    }
    return n;
}

size_t SerialIO::ReadLine(char *buf, int size)
{
    size_t n = 0;
    // prelomi vrstice
    while (n < size)
    {
        char c = '\0';
        if (Read(&c, sizeof(char)) <= 0 )
            break;
        if (c == '\n' || c == '\r' || c == '\0')
            break;
        *buf++ = c;
        n++;
    }
    *buf = '\0';
    return n;
}

size_t SerialIO::WriteChar(char c)
{
    return Write(&c, 1);
}

size_t SerialIO::WriteString(const std::string &s)
{
    return Write((void*)s.c_str(), s.size());
}

size_t SerialIO::WriteString(const char *buf)
{
    return Write(buf, strlen(buf));
}


