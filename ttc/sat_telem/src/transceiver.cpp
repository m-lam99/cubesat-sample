
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

void Transceiver::SendCommand(std::vector<uint8_t> data)
{
    uint8_t *data_ptr = static_cast<uint8_t *>(data.data());

    write(_serial_port, data_ptr, 1000 * sizeof(data_ptr));
    // write(_serial_port, data_ptr, sizeof(data_ptr));

    sleep(1);
}

std::vector<uint8_t> Transceiver::ReceiveData(void)
{

    std::array<uint8_t, Transceiver::MAX_BYTES_AX25> buff;
    std::vector<uint8_t> output_buff;
    memset(&buff, '\0', sizeof(buff));
    read(_serial_port, &buff, sizeof(buff));
    for (unsigned int i = 0; i < MAX_BYTES_AX25; i++)
    {
        output_buff.push_back(buff[i]);
    }
    sleep(1);
    return output_buff;
}

bool Transceiver::TestTransceiver(void)
{
    SendCommand(CMD_FIRMWARE_VERSION);
    std::vector<uint8_t> buff;
    std::vector<uint8_t> version = ReceiveData();
    for (unsigned int i = 0; i < version.size(); i++)
    {
        std::cout << version[i];
    }
    std::cout << std::endl;
    sleep(1);
    return true;
}

void Transceiver::TransmitMessage(std::vector<uint8_t> message)
{

    std::vector<uint8_t> full_message;
    full_message.insert(full_message.end(), CMD_TRANSMIT_MODE_CONFIG.begin(), CMD_TRANSMIT_MODE_CONFIG.end());
    full_message.insert(full_message.end(), message.begin(), message.end());
    for (unsigned int i = 0; i < full_message.size(); i++)
    {
        std::cout << +full_message[i];
    }
    std::cout << std::endl;
    SendCommand(full_message);
}