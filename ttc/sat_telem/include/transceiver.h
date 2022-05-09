#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <array>

class Transceiver
{

public:
    static const int MAX_BYTES_AX25 = 42;

    Transceiver();
    ~Transceiver();
    void SendCommand(const char *data);
    std::array<char, MAX_BYTES_AX25> ReceiveData(std::array<char, MAX_BYTES_AX25> buff);
    bool TestTransceiver(void);
    void TransmitMessage(void);

private:
    const char CMD_OPERATING_MODE[3] = {0x41, 0x54, 0x4D};
    const char CMD_RECEIVE_MODE_CONFIG[3] = {0x41, 0x54, 0x52};
    const char CMD_TRANSMIT_MODE_CONFIG[3] = {0x41, 0x54, 0x53};
    const char CMD_SYNC_BYTES[3] = {0x41, 0x54, 0x41};
    const char CMD_HOST_BAUD_RATE[3] = {0x41, 0x54, 0x48};
    const char CMD_RF_BAUD_RATE[3] = {0x41, 0x54, 0x42};
    const char CMD_RF_OUTPUT_POWER[3] = {0x41, 0x54, 0x50};
    const char CMD_DATA_ERROR_CHECKING[3] = {0x41, 0x54, 0x45};
    const char CMD_ENABLE_RESPONSE[3] = {0x41, 0x54, 0x43};
    const char CMD_RSSI[3] = {0x41, 0x54, 0x51};
    const char CMD_CONFIGURATION_SETTINGS[3] = {0x41, 0x54, 0x3F};
    const char CMD_FIRMWARE_VERSION[3] = {0x41, 0x54, 0x56};
    const char CMD_RESET_DEFAULT[3] = {0x41, 0x54, 0x44};
    const char MODE_READY = 0x02;
    const char MODE_HOST_BAUD_19200 = 0x01;
    const char MODE_RF_BAUD_9600 = 0x02;
    std::string PORT = "/dev/ttyS0";
    std::string FIRMWARE = "#V1.01";
    struct termios _tty;
    int _serial_port;
};