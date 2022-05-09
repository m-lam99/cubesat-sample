
#include "transceiver.h"

Transceiver::Transceiver()
{
    _serial_port = open("/dev/ttyS0", O_RDWR);

    if (tcgetattr(_serial_port, &_tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

    _tty.c_cflag &= ~PARENB;
    _tty.c_cflag &= ~CSTOPB;
    _tty.c_cflag &= ~CSIZE;
    _tty.c_cflag |= CS8;
    _tty.c_cflag &= ~CRTSCTS;
    _tty.c_cflag |= CREAD | CLOCAL;
    _tty.c_lflag &= ~ICANON;
    _tty.c_lflag &= ~ECHO;
    _tty.c_lflag &= ~ECHOE;
    _tty.c_lflag &= ~ECHONL;
    _tty.c_lflag &= ~ISIG;
    _tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    _tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    _tty.c_oflag &= ~OPOST;
    _tty.c_oflag &= ~ONLCR;
    _tty.c_cc[VTIME] = 10;
    _tty.c_cc[VMIN] = 0;
    cfsetispeed(&_tty, B19200);
    cfsetospeed(&_tty, B19200);
    if (tcsetattr(_serial_port, TCSANOW, &_tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
}
Transceiver::~Transceiver()
{
    close(_serial_port);
}

void Transceiver::SendCommand(const char *data)
{

    write(_serial_port, data, sizeof(data));
    sleep(1);
}

std::array<char, Transceiver::MAX_BYTES_AX25> Transceiver::ReceiveData(std::array<char, MAX_BYTES_AX25> buff)
{

    memset(&buff, '\0', sizeof(buff));
    read(_serial_port, &buff, sizeof(buff));
    return buff;
}

bool Transceiver::TestTransceiver(void)
{
    SendCommand(CMD_FIRMWARE_VERSION);
    std::array<char, MAX_BYTES_AX25> buff;
    std::array<char, MAX_BYTES_AX25> version = ReceiveData(buff);

    for (int i = 0; i < 6; i++)
    {
        std::cout << version[i];
    }

    return true;
}

void Transceiver::TransmitMessage(void)
{
}