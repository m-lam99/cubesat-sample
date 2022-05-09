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
}