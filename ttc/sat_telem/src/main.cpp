#include "transceiver.h"

// #include <iostream>
// #include <string.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <termios.h>
// #include <unistd.h>

int main()
{
    Transceiver comms;

    comms.TestTransceiver();
    while (1)
    {
        std::vector<uint8_t> message = {'T', 'e', 's', 't'};
        comms.TransmitMessage(message);
        // comms.ReceiveData();
        sleep(2);
    }
}