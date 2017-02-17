/**
 * http://www.cmrr.umn.edu/~strupp/serial.html
 */

#include <SerialPort.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>


/**
 * Default constructor.
 */
SerialPort::SerialPort():
    mFile(-1),
    mSpeed(B9600),
    mParity(NO_PARITY),
    mDataBits(0),
    mStopBits(0),
    mFlow(0),
    mTimeout(0),
    mBlocking(false)
{
}

SerialPort::SerialPort(const char *port, int speed, short parityMode, short dataBits):
    mPort(port),
    mFile(-1),
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


/**
    Opens a device.
    Use SetBlocking(false) for NDELAY option.
*/
bool SerialPort::Open()
{
    mFile = open(Port(), O_RDWR | O_NOCTTY);
    if (mFile > 0)
    {
        Reconfigure();
        Flush();
    }
    return (mFile > 0);
}


void SerialPort::Close()
{
	close(mFile);
	mFile = -1;
}


/**
    Reads max 'size' bytes into 'buffer'.
    \return Number of bytes read
*/
size_t SerialPort::Read(void *buffer, size_t size)
{
	return read(mFile, buffer, size);
}


/**
    Writes 'size' bytes from 'buffer'.
    \return Number of bytes written
*/
size_t SerialPort::Write(const void *buffer, size_t size)
{
    return write(mFile, buffer, size);
}


/**
 * Number of available characters.
 */
size_t SerialPort::Available ()
{
  int count = 0;
  if (ioctl (mFile, TIOCINQ, &count))
      return -1;
  return count;
}


void SerialPort::Flush()
{
    tcflush(mFile, TCIOFLUSH);
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


/*
    Sets read timeout in ms.
*/
bool SerialPort::SetTimeout(int timeout)
{
    mTimeout = timeout;
    return Reconfigure();
}


/**
    Enables or disables blocking mode.
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
    Sets/clears Request-to-Send line (if present).
*/
bool SerialPort::SetRTS(bool enabled)
{
    int status = TIOCM_RTS;
    return ioctl(mFile, enabled ? TIOCMBIS : TIOCMBIC, &status) == 0;
}


/**
    Sets/clears Data Terminal Ready line (if present).
*/
bool SerialPort::SetDTR(bool enabled)
{
    int status = TIOCM_DTR;
    return ioctl(mFile, enabled ? TIOCMBIS : TIOCMBIC, &status) == 0;
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
 * Applies serial port control attributes.
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
    cfsetispeed(&options, mSpeed);
    cfsetospeed(&options, mSpeed);

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

