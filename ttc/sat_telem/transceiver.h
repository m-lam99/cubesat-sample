#include <string.h>

class Transceiver
{

public:
    Transceiver();
    void SendCommand(int data[]);
    void ReceiveData(int packet_size, int channel);
    bool TestTransceiver(void);
    void TransmitMessage(void);

private:
    const int CMD_OPERATING_MODE[3] = {0x41, 0x54, 0x4D};
    const int CMD_RECEIVE_MODE_CONFIG[3] = {0x41, 0x54, 0x52};
    const int CMD_TRANSMIT_MODE_CONFIG[3] = {0x41, 0x54, 0x53};
    const int CMD_SYNC_BYTES[3] = {0x41, 0x54, 0x41};
    const int CMD_HOST_BAUD_RATE[3] = {0x41, 0x54, 0x48};
    const int CMD_RF_BAUD_RATE[3] = {0x41, 0x54, 0x42};
    const int CMD_RF_OUTPUT_POWER[3] = {0x41, 0x54, 0x50};
    const int CMD_DATA_ERROR_CHECKING[3] = {0x41, 0x54, 0x45};
    const int CMD_ENABLE_RESPONSE[3] = {0x41, 0x54, 0x43};
    const int CMD_RSSI[3] = {0x41, 0x54, 0x51};
    const int CMD_CONFIGURATION_SETTINGS[3] = {0x41, 0x54, 0x3F};
    const int CMD_FIRMWARE_VERSION[3] = {0x41, 0x54, 0x56};
    const int CMD_RESET_DEFAULT[3] = {0x41, 0x54, 0x44};
    const int MODE_READY = 0x02;
    const int MODE_HOST_BAUD_19200 = 0x01;
    const int MODE_RF_BAUD_9600 = 0x02;
    const int DEFAULT_HOST_BAUD = 19200; // bps
    const int DEFAULT_RF_BAUD = 9600;    // bps
    static const float TIMEOUT = 2;      // s
    char *PORT = "/dev/ttyS0";
    static const float WRITE_WAIT_TIME = 0.2;   // s
    static const float RECEIVE_WAIT_TIME = 0.2; // s
    char *FIRMWARE = "#V1.01";
};