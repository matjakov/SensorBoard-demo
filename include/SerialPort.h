/**
    Portable POSIX/WIN32 interface to a serial comm. device.
    by Matjaz Kovac
*/
#ifndef _SERIALPORT_H
#define _SERIALPORT_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <stdio.h>
#include <termios.h>
#endif

class SerialPort
{
    public:

    enum {
        NO_PARITY = 0,
        ODD_PARITY = 1,
        EVEN_PARITY = 2
    };

    enum {
        HW_FLOW = 1,
        SW_FLOW = 2
    };


    SerialPort();
    ~SerialPort();

    bool Open(const char *name);
    void Close();

    size_t Read(void *buffer, size_t size);
    char ReadChar();
    size_t Write(const void *buffer, size_t size);
    size_t Available();
    void Flush();

    bool SetMode(int baudRate, short dataBits = 8, short stopBits = 1, short parity = NO_PARITY);
    bool SetTimeout(int timeout);
    bool SetFlowControl(short flow);
    bool SetRTS(bool enabled);
    bool SetDTR(bool enabled);

    int BaudRate() const;
    short StopBits() const;
    short DataBits() const;
    short ParityMode() const;
    int Timeout() const;
    bool IsBlocking() const;
    short FlowControl() const;

    bool IsCD();
    bool IsCTS();
    bool IsDSR();
    bool IsRI();

protected:
    bool Reconfigure();
    bool SetBlocking(bool enable);

private:
#ifdef _WIN32
    HANDLE mFile;
#else
    static speed_t ConvertBaudRate(int baudrate);
    int mFile;
#endif
    int mSpeed;
    short mParity, mDataBits, mStopBits, mFlow;
    int mTimeout;
    bool mBlocking;
};




#endif

