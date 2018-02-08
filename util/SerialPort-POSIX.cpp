/**
POSIX implementation of SerialPort
by Matjaz Kovac

http://www.cmrr.umn.edu/~strupp/serial.html

*/

#include <SerialPort.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>


/**
    Allocate a SerialPort instance.
*/
SerialPort::SerialPort():
    mFile(-1),
    mSpeed(B9600),
    mParity(NO_PARITY),
    mDataBits(8),
    mStopBits(1),
    mFlow(0),
    mTimeout(0),
    mBlocking(false)
{
}


SerialPort::~SerialPort()
{
	Close();
}


/**
    Open a device.
    Use SetTimeout(0) for NDELAY option.
*/
bool SerialPort::Open(const char *device)
{
    int flags = O_RDWR | O_NOCTTY;
	mFile = open(device, flags);
    if (mFile >= 0)
    {
        Reconfigure();
        Flush();
    }
    return (mFile >= 0);
}


/**
	Close the device.
*/
void SerialPort::Close()
{
	close(mFile);
	mFile = -1;
}


/**
    Number of available characters.
 */
size_t SerialPort::Available ()
{
  if (mFile <= 0)
    return 0;

  int count = 0;
  if (ioctl (mFile, TIOCINQ, &count))
      return -1;
  return count;
}


/**
    Read max 'size' bytes into 'buffer'.
    \return Number of bytes read
*/
size_t SerialPort::Read(void *buffer, size_t size)
{
	return read(mFile, buffer, size);
}


/**
    Write 'size' bytes from 'buffer'.
    \return Number of bytes written
*/
size_t SerialPort::Write(const void *buffer, size_t size)
{
    return write(mFile, buffer, size);
}


/**
    Flush terminal input or output.
*/
void SerialPort::Flush()
{
    tcflush(mFile, TCIOFLUSH);
}


char SerialPort::ReadChar()
{
    char c = '\0';
    Read(&c, sizeof(c));
    return c;
}




bool SerialPort::SetMode(int baudRate, short dataBits, short stopBits, short parity)
{
    mSpeed = baudRate;
    mDataBits = dataBits;
    mStopBits = stopBits;
    mParity = parity;
    return Reconfigure();
}


/**
    Read/write timeout in ms.
    timeout == 0 means non-blocking (NDELAY) mode.
*/
bool SerialPort::SetTimeout(int timeout)
{
    mTimeout = timeout;
    SetBlocking(timeout != 0);
    return Reconfigure();
}

/**
    A combination of HW_FLOW and SW_FLOW.
*/
bool SerialPort::SetFlowControl(short flow)
{
    mFlow = flow;
    return Reconfigure();
}


/**
    Set/clear Request-to-Send line (if present).
*/
bool SerialPort::SetRTS(bool enabled)
{
    int status = TIOCM_RTS;
    return ioctl(mFile, enabled ? TIOCMBIS : TIOCMBIC, &status) == 0;
}


/**
    Set/clear Data Terminal Ready line (if present).
*/
bool SerialPort::SetDTR(bool enabled)
{
    int status = TIOCM_DTR;
    return ioctl(mFile, enabled ? TIOCMBIS : TIOCMBIC, &status) == 0;
}


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


/**
    \returns timeout in ms.
*/
int SerialPort::Timeout() const {
    return mTimeout;
}


bool SerialPort::IsBlocking() const {
    return mBlocking;
}


/**
    \returns a combination of HW_FLOW and SW_FLOW.
*/
short SerialPort::FlowControl() const
{
    return mFlow;
}


/**
    Carrier Detect.
    \returns true if this line is on
*/
bool SerialPort::IsCD()
{
    int status = 0;
    ioctl(mFile, TIOCMGET, &status);
    return status & TIOCM_CD;
}


/**
    Clear To Send.
    \returns true if this line is on
*/
bool SerialPort::IsCTS()
{
    int status = 0;
    ioctl(mFile, TIOCMGET, &status);
    return status & TIOCM_CTS;
}


/**
    Data Set Ready.
    \returns true if this line is on
*/
bool SerialPort::IsDSR()
{
    int status = 0;
    ioctl(mFile, TIOCMGET, &status);
    return status & TIOCM_DSR;
}


/**
    Ring Indicator.
    \returns true if this line is on
*/
bool SerialPort::IsRI()
{
    int status = 0;
    ioctl(mFile, TIOCMGET, &status);
    return status & TIOCM_RI;
}


/**
    Apply serial port control attributes.
    Can be called before Open().
*/
bool SerialPort::Reconfigure()
{
    struct termios options;
    int err = tcgetattr(mFile, &options);
    if (err)
        return false;

    // reset all flags
    options.c_iflag &= ~(IXON | IXOFF | IXANY | INPCK);
    options.c_cflag &= ~(CRTSCTS | CSIZE | CSTOPB | PARODD | PARENB);
    options.c_lflag &= ~(ECHO | ECHONL | ISIG | ICANON);
    options.c_cflag |= CLOCAL;

	if (mDataBits == 8)
        options.c_cflag |= CS8;
    else if (mDataBits == 7)
        options.c_cflag |= CS7;
    else if (mDataBits == 6)
        options.c_cflag |= CS6;
    else if (mDataBits == 5)
        options.c_cflag |= CS5;

	if (mStopBits == 2)
        options.c_cflag |= CSTOPB;

    if (mParity) {
        options.c_cflag |= PARENB;
        if (mParity == ODD_PARITY)
            options.c_cflag |= PARODD;
    }

    if (mFlow & HW_FLOW)
        options.c_cflag |= CRTSCTS;

    if (mFlow & SW_FLOW)
        options.c_iflag |= (IXON | IXOFF);

    // baud rate
    speed_t br = ConvertBaudRate(mSpeed);
    cfsetispeed(&options, br);
    cfsetospeed(&options, br);

	// time outs
    if (mTimeout  < 0)
    {
        // infinite
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 0;
    }
    else {
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = mTimeout / 100;
    }

    return tcsetattr(mFile, TCSANOW, &options) == 0;
}


/**
    Enable or disable blocking mode.
    When enabled, Read() and Write() are affected by timeouts and flow control.
    If disabled, SetTimeout() has no effect.
*/
bool SerialPort::SetBlocking(bool enable)
{
    mBlocking = enable;
    int flags = fcntl(mFile, F_GETFL );
    if (mBlocking)
        flags &= ~(O_NDELAY);
    else
        flags |= O_NDELAY;
    return fcntl(mFile, F_SETFL, flags) == 0;
}


/**
    Convert integer baud rate into termios B... constants.
*/
speed_t SerialPort::ConvertBaudRate(int baudrate)
{
    switch(baudrate)
      {
        case      50 : return B50;
        case      75 : return B75;
        case     110 : return B110;
        case     134 : return B134;
        case     150 : return B150;
        case     200 : return B200;
        case     300 : return B300;
        case     600 : return B600;
        case    1200 : return B1200;
        case    1800 : return B1800;
        case    2400 : return B2400;
        case    4800 : return B4800;
        case    9600 : return B9600;
        case   19200 : return B19200;
        case   38400 : return B38400;
        case   57600 : return B57600;
        case  115200 : return B115200;
        case  230400 : return B230400;
        case  460800 : return B460800;
        case  500000 : return B500000;
        case  576000 : return B576000;
        case  921600 : return B921600;
        case 1000000 : return B1000000;
        case 1152000 : return B1152000;
        case 1500000 : return B1500000;
        case 2000000 : return B2000000;
        case 2500000 : return B2500000;
        case 3000000 : return B3000000;
        case 3500000 : return B3500000;
        case 4000000 : return B4000000;
        default      : return B0;
      }
}
