
#include "transceiver.h"

Transceiver::Transceiver():
    UARTDevice(TTC_CHANNEL, TTC_BAUD)
{

}

Transceiver::~Transceiver():
    UARTDevice(TTC_CHANNEL, TTC_BAUD)
{
	if(this->serial_port!=-1) this->close();
}


void Transceiver::SendCommand(std::vector<uint8_t> data)
{
    uint8_t *data_ptr = static_cast<uint8_t *>(data.data());
    write(serial_port,
          data_ptr, sizeof(data_ptr));
    sleep(1);
}

std::vector<uint8_t> Transceiver::ReceiveData(void)
{

    std::array<uint8_t, Transceiver::MAX_BYTES_AX25> buff;
    std::vector<uint8_t> output_buff;
    memset(&buff, '\0', sizeof(buff));
    usleep(50000);

    read(serial_port, &buff, sizeof(buff));
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