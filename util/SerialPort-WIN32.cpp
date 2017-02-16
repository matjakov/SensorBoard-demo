/**
WIN32 implementation of SerialPort
by Matjaz Kovac
*/

#include <SerialPort.h>
#include <io.h>


SerialPort::SerialPort():
    mFile(INVALID_HANDLE_VALUE),
    mSpeed(B9600),
    mParity(NO_PARITY),
    mDataBits(8),
    mStopBits(0),
    mFlow(0),
    mBlocking(false)
{
}


SerialPort::SerialPort(const char *port, int speed, short parityMode, short dataBits):
    mPort(port),
    mFile(INVALID_HANDLE_VALUE),
    mSpeed(speed),
    mParity(parityMode),
    mDataBits(dataBits),
    mStopBits(0),
    mFlow(0),
    mBlocking(false)
{    
}

SerialPort::~SerialPort()
{
    Close();
}


bool SerialPort::Open()
{
    mFile = CreateFile(Port(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mFile != INVALID_HANDLE_VALUE)
    {
        Reconfigure();
        Flush();
    }
    return (mFile != INVALID_HANDLE_VALUE);
}


void SerialPort::Close()
{
    CloseHandle(mFile);
    mFile = INVALID_HANDLE_VALUE;
}


/**
    Clears buffers.
*/
void SerialPort::Flush()
{
    PurgeComm(mFile, PURGE_RXCLEAR | PURGE_TXCLEAR);
}


/**
    Reads max 'size' bytes into 'buffer'.
    \return Number of bytes read
*/
size_t SerialPort::Read(void *buffer, size_t size)
{
    DWORD count = 0;
    ReadFile(mFile, buffer, size, &count, NULL);
    return count;
}


/**
    Writes 'size' bytes from 'buffer'.
    \return Number of bytes written
*/
size_t SerialPort::Write(const void *buffer, size_t size)
{
    DWORD count = 0;
    WriteFile(mFile, buffer, size, &count, NULL);
    return count;
}


/**
    Number of waiting chars.
*/
size_t SerialPort::Available()
{
	DWORD flags;
	COMSTAT stat;
	if (!ClearCommError(mFile, &flags, &stat))
        return -1;
	return stat.cbInQue;
}


const char* SerialPort::Port() const
{
    return mPort.c_str();
}

/**
 * Returns set baud rate.
 */
int SerialPort::BaudRate() const {
    return mSpeed;
}

short SerialPort::DataBits() const {
    return mDataBits;
}

short SerialPort::StopBits() const {
    return mStopBits;
}

short SerialPort::ParityMode() const {
    return mParity;
}

int SerialPort::Timeout() const {
    return mTimeout;
}

bool SerialPort::IsBlocking() const {
    return mBlocking;
}


bool SerialPort::SetPort(const char* port)
{
    mPort = port;
    return port != NULL;
}

bool SerialPort::SetBaudRate(int rate)
{
    mSpeed = rate;
    return Reconfigure();
}


bool SerialPort::SetDataBits(short n)
{
    mDataBits = n;
    return Reconfigure();
}


bool SerialPort::SetStopBits(short n)
{
    mStopBits = n;
    return Reconfigure();
}


bool SerialPort::SetParityMode(short mode)
{
    mParity = mode;
    return Reconfigure();
}


bool SerialPort::SetBlocking(bool enable)
{
    mBlocking = enable;
    return SetTimeout(mTimeout);
}



/**
	Sets timeouts for Read() and Write() on WIN32.
*/
bool SerialPort::SetTimeout(int timeout)
{
    mTimeout = timeout;
    COMMTIMEOUTS cto = {MAXDWORD, 0, 0, 0, 0};
    cto.ReadIntervalTimeout = mBlocking ? timeout : MAXDWORD;
    cto.ReadTotalTimeoutConstant = mBlocking ? timeout : 0;
    cto.WriteTotalTimeoutConstant = timeout;
    return SetCommTimeouts(mFile, &cto);
}


/**
	Data Terminal Ready.
	\param enabled true/false: set this line high/low
*/
bool SerialPort::SetDTR(bool enable)
{
    DCB dcb;
    GetCommState(mFile, &dcb);
    dcb.fDtrControl = enable ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;
    BOOL ret = SetCommState(mFile, &dcb);
    return ret;
}


/**
	Ready To Send.
	\param enabled true: set this line high/low
*/
bool SerialPort::SetRTS(bool enable)
{
    DCB dcb;
    GetCommState(mFile, &dcb);
    dcb.fRtsControl = enable ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
    BOOL ret = SetCommState(mFile, &dcb);
    return ret;
}


bool SerialPort::IsCD()
{
    DWORD status = 0;
    GetCommModemStatus(mFile, &status);
    return status & MS_RLSD_ON;
}


bool SerialPort::IsCTS()
{
    DWORD status = 0;
    GetCommModemStatus(mFile, &status);
    return status & MS_CTS_ON;
}


bool SerialPort::IsDSR()
{
    DWORD status = 0;
    GetCommModemStatus(mFile, &status);
    return status & MS_DSR_ON;
}


bool SerialPort::IsRI()
{
    DWORD status = 0;
    GetCommModemStatus(mFile, &status);
    return status & MS_RING_ON;
}



/**
    Sets serial port attributes.
*/
bool SerialPort::Reconfigure()
{
    DCB dcb;
    dcb.DCBlength = sizeof(dcb);
    BOOL ok = GetCommState(mFile, &dcb);
    if (!ok)
        return false;
    dcb.BaudRate = mSpeed;
    dcb.fParity = mParity; // enables error checking
    dcb.Parity = mParity; // 0-4=no,odd,even,mark,space
    dcb.ByteSize = mDataBits;
    dcb.StopBits = (mStopBits == 2 ? 2 : 0); // 0 = 1 bit, 2 = 2 bits
    dcb.fOutxCtsFlow = (mFlow & HW_FLOW);
    dcb.fRtsControl = (mFlow & HW_FLOW) ? RTS_CONTROL_TOGGLE : RTS_CONTROL_ENABLE;
    dcb.fOutX = (mFlow & SW_FLOW);
    dcb.fInX = (mFlow & SW_FLOW);
    return SetCommState(mFile, &dcb);
}



