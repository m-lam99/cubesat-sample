/* UART.h
//  */

// #ifndef UART_H_
// #define UART_H_

// #include <cstdint>
// #include <termios.h>
// #include <string.h> 

// #define BBB_UART_0 "/dev/ttyS0" // not used

// class UARTDevice{
// private:
// 	int serial_port;
//     std::string port_name;
//     struct termios tty; 
// protected:
// 	int file;
// public:
// 	UARTDevice(std::string port_name);
// 	virtual int open();
// 	virtual int write(unsigned char msg[]);
//     virtual void read(char read_buf[], int buf_size);
// 	virtual void close();
// 	virtual ~I2CDevice();
// };

// #endif /* UART_H_ */