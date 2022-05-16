/*
 * UART.cpp  Created on: 17 May 2014
 */

#include"UART.h"
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
using namespace std;

#define HEX(x) setw(2) << setfill('0') << hex << (int)(x)

/**
 * Constructor for the I2CDevice class. It requires the bus number and device number. The constructor
 * opens a file handle to the I2C device, which is destroyed when the destructor is called
 * @param port Usually in the form "/dev/ttySX"
 * @param device The device ID on the bus.
 */
UARTDevice::UARTDevice(string port_name) {
	this->port_name = port_name;
	this->open();
}

/**
 * Open a connection to an I2C device
 * @return 1 on failure to open to the bus or device, 0 on success.
 */
int UARTDevice::open(){
    int serial_port = open(port_name, O_RDWR);
    if (tcgetattr(serial_port, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }
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
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }
}

/**
 * Write a single value to the I2C device. Used to set up the device to read from a
 * particular address.
 * @param value[] message array to pass in
 * @return 1 on failure to write, 0 on success.
 */
int UARTDevice::write(unsigned char msg[]){
    write(serial_port, msg, sizeof(msg));
   return 0;
}

/**
 * BUFFER SIZE SET TO 6
 * @param buf_size size of read buffer
 * pointer to buffer --> siz
 * @param read_buf[] pointer to buffer array 
 * @return a pointer of type unsigned char* that points to the first element in the block of registers
 */
void UARTDevice::read(char read_buf[], int buf_size){

    memset(&read_buf, '\0', buf_size);
    int num_bytes = read(serial_port, &read_buf, buf_size);
    if (num_bytes < 0)
    {
        printf("Error reading: %s", strerror(errno));
        return 1;
    }
    printf("Read %i bytes. Received message: %s", num_bytes);

	return;
}


/**
 * Close the file handles and sets a temporary state to -1.
 */
void UARTDevice::close(){
	close(serial_port);

}

/**
 * Closes the file on destruction, provided that it has not already been closed.
 */
UARTDevice::~I2CDevice() {
	this->close();
}

