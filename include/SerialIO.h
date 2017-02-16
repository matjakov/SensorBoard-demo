#ifndef SERIALIO_H
#define SERIALIO_H

#include <stdlib.h>
#include <string>

class SerialIO
{
    public:
    virtual size_t Read(void *buffer, size_t size) = 0;
    virtual size_t Write(const void *buffer, size_t size) = 0;

    char ReadChar();
    size_t ReadLine(char *buf, int size);    
    size_t ReadLine(std::string &s);
    size_t WriteChar(char c);
    size_t WriteString(const std::string &s);
    size_t WriteString(const char *s);

};

#endif // SERIALIO_H
