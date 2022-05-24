
#include "transceiver.h"

Transceiver::Transceiver():
    UARTDevice(TTC_CHANNEL, TTC_BAUD)
{
    config();
}

Transceiver::~Transceiver()
{
	if(this->serial_port!=-1) this->close();
}

int Transceiver::config(){

    std::cout << "Configuring transceiver" << std::endl;
    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

     // FOR TTC 
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;
    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;
    cfsetispeed(&tty, B19200);
    cfsetospeed(&tty, B19200);

    tcflush(serial_port, TCIFLUSH);
    tcsetattr(serial_port, TCSANOW, &tty);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }   
    return 0; 

}

void Transceiver::SendCommand(std::vector<uint8_t> data)
{
    uint8_t *data_ptr = static_cast<uint8_t *>(data.data());
    write(serial_port,
          data_ptr, 1000* sizeof(data_ptr));
    sleep(1);
}

std::vector<uint8_t> Transceiver::ReceiveData(void)
{

    std::array<uint8_t, Transceiver::MAX_BYTES_AX25> buff;
    std::vector<uint8_t> output_buff;
    memset(&buff, '\0', sizeof(buff));
    usleep(50000);

    read(serial_port, &buff, 100*sizeof(buff));
    for (unsigned int i = 0; i < MAX_BYTES_AX25; i++)
    {
        output_buff.push_back(buff[i]);
    }
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
    return true;
}

void Transceiver::TransmitMessage(std::vector<uint8_t> message)
{
    std::vector<uint8_t> full_message;
    full_message.insert(full_message.end(), CMD_TRANSMIT_MODE_CONFIG.begin(), CMD_TRANSMIT_MODE_CONFIG.end());
    full_message.insert(full_message.end(), message.begin(), message.end());
    for (unsigned int i = 0; i < full_message.size(); i++)
    {
        std::cout << full_message[i];
    }
    std::cout << std::endl;
    SendCommand(full_message);
}