
#ifndef UART_H_
#define UART_H_

#include <cstdint>

#define BBB_UART_0 "/dev/ttyS0"
#define BBB_UART_1 "/dev/ttyS1"
#define BBB_UART_2 "/dev/ttyS2"
#define BBB_UART_4 "/dev/ttyS4"

class UARTDevice{
	private:

	protected:
		unsigned int channel; 
		unsigned int baud; 
		int serial_port; 
		virtual int init(); // Open serial connection 
		virtual int config(); // Config settigs 

	public:
		UARTDevice(unsigned int channel, unsigned int baud);
		virtual void close();
		void readln(char* buffer);
		virtual ~UARTDevice();
};

#endif /* UART_H */