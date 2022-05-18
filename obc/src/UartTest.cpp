#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main()
{
    int serial_port = open("/dev/ttyS4", O_RDWR | O_NOCTTY | O_NDELAY);

    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }
    // tty.c_cflag &= ~PARENB;
    // tty.c_cflag &= ~CSTOPB;
    // tty.c_cflag &= ~CSIZE;
    // tty.c_cflag |= CS8;
    // tty.c_cflag &= ~CRTSCTS;
    // tty.c_cflag |= CREAD | CLOCAL;
    // tty.c_lflag &= ~ICANON;
    // tty.c_lflag &= ~ECHO;
    // tty.c_lflag &= ~ECHOE;
    // tty.c_lflag &= ~ECHONL;
    // tty.c_lflag &= ~ISIG;
    // tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    // tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    // tty.c_oflag &= ~OPOST;
    // tty.c_oflag &= ~ONLCR;
    // tty.c_cc[VTIME] = 10;
    // tty.c_cc[VMIN] = 0;
    // cfsetispeed(&tty, B9600);
    // cfsetospeed(&tty, B9600);

    tty.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
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
    // unsigned char msg[] = {0x41, 0x54, 0x56};
    // write(serial_port, msg, sizeof(msg));
    sleep(1);
    // char read_buf[75];
    // memset(&read_buf, '\0', sizeof(read_buf));
    // while(1){
    // int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
    // // if (num_bytes < 0)
    // // {
    // //     printf("Error reading: %s", strerror(errno));
    // //     return 1;
    // // }
    // printf("Read %i bytes. Received message: %s\n", num_bytes, read_buf);
    // }
    while(1) {
    char c;
    char buffer[256];
    char *b = buffer; 
    int rx_length = -1;
    while(1) {
        rx_length = read(serial_port, (void*)(&c), 1);

        if (rx_length <= 0) {
            //wait for messages
            sleep(1);
        } else {
            if (c == '\n') {
                std::cout << c;
                *b++ = '\0';
                break;
            }
            *b++ = c;
            std::cout << c;
        }
    }

    }

    close(serial_port);
    return 0;
}