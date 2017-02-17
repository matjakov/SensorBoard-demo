/**
    POSIX/WIN32 serial port interface
    
    \author Matjaz Kovac
*/
#ifndef _SERIALPORT_H
#define _SERIALPORT_H


#include <SerialIO.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <termios.h>
#endif


#ifdef _WIN32
enum {
    B300=CBR_300,
    B600=CBR_600,
    B1200=CBR_1200,
    B2400=CBR_2400,
    B4800=CBR_4800,
    B9600=CBR_9600,
    B19200=CBR_19200,
    B38400=CBR_38400,
    B57600=CBR_57600,
    B115200=CBR_115200
};
#endif // _WIN32

class SerialPort : public SerialIO
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
    SerialPort(const char *port, int speed, short parityMode = NO_PARITY, short dataBits = 8);
    ~SerialPort();
    bool Open();
    void Close();

    virtual size_t Read(void *buffer, size_t size);
    virtual size_t Write(const void *buffer, size_t size);
    size_t Available();
    void Flush();

    const char* Port() const;
    int BaudRate() const;
    short StopBits() const;
    short DataBits() const;
    short ParityMode() const;
    int Timeout() const;
    bool IsBlocking() const;

    bool SetPort(const char *port);
    bool SetBaudRate(int rate);
    bool SetStopBits(short n);
    bool SetDataBits(short n);
    bool SetParityMode(short mode);
    bool SetTimeout(int timeout);
    bool SetBlocking(bool enable);

    // Most common signal lines
    bool SetRTS(bool enabled);
    bool SetDTR(bool enabled);
    bool IsCD();
    bool IsCTS();
    bool IsDSR();
    bool IsRI();

protected:
    bool Reconfigure();

private:
#ifdef _WIN32
    HANDLE mFile;
#else
    int mFile;
#endif
    std::string mPort;
    int mSpeed;
    short mParity, mDataBits, mStopBits, mFlow;
    int mTimeout;
    bool mBlocking;
};

#endif

