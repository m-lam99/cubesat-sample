#include "transceiver.h"

#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int main()
{
    Transceiver comms;

    comms.TestTransceiver();
    std::vector<uint8_t> message = {'T', 'e', 's', 't'};
    std::cout << "Test1" << std::endl;
    comms.TransmitMessage(message);
}