/*
 * 
 * For more details, see http://www.derekmolloy.ie/
 */

#include "UART.h"
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

using namespace std;


/**
 * Constructor for the I2CDevice class. It requires the bus number and device number. The constructor
 * opens a file handle to the I2C device, which is destroyed when the destructor is called
 * @param bus The bus number. Usually 0 or 1 on the BBB
 * @param device The device ID on the bus.
 */
UARTDevice::UARTDevice(unsigned int channel, unsigned int baud) {
	this->channel = channel;
    this->baud = baud; 
	this->init();
    this->config();
}

/**
 * Open a connection to an I2C device
 * @return 1 on failure to open to the bus or device, 0 on success.
 */
int UARTDevice::init(){
    const char* name;

    // Device channel based on input 
    switch(this->channel) {
        case 0: 
            name = BBB_UART_0;
            break; 
        case 1: 
            name = BBB_UART_1;
            break; 
        case 2:
            name = BBB_UART_2; 
            break;
        case 4:
            name = BBB_UART_4;
            break; 
        default: 
            cout << "Invalid channel UART" << this->channel << endl;
            return 1;
    }

    this->serial_port = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
    
    return 0;
}

int UARTDevice::config(){
    
    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }
    
    // Device channel based on baud 
    switch(this->baud) {
        case 9600: 
            tty.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
            break; 
        case 19200: 
            tty.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
            break; 
        default: 
            cout << "Invalid Baud Selection" << this->baud << endl;
            return 1;
    }
    
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    tcflush(serial_port, TCIFLUSH);
    tcsetattr(serial_port, TCSANOW, &tty);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }   

}

/**
 * Reads a line from  UART
 */
void UARTDevice::readln(char* buffer){
	char c;
    char *b = buffer;
    int rx_length = -1;
    cout << "UART Print";
    while(1) {
        rx_length = read(serial_port, (void*)(&c), 1);

        if (rx_length <= 0) {
            //wait for messages
            sleep(1);
        } else {
            if (c == '\n') {
                *b++ = '\0';
                cout << c;
                break;
            }
            *b++ = c;
            cout << c;
        }
    }

    return;
}


/**
 * Close the file handles and sets a temporary state to -1.
 */
void UARTDevice::close(){
	::close(this->serial_port);
}

/**
 * Closes the file on destruction, provided that it has not already been closed.
 */
UARTDevice::~UARTDevice() {
	if(this->serial_port!=-1) this->close();
}
