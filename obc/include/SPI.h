/*
 * SPIDevice.h  Created on: 22 May 2014
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring BeagleBone"
 * See: www.exploringbeaglebone.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European
 * Union Public License (the "EUPL") version 1.1 as published by the
 * European Union. Any use of this Software, other than as authorized
 * under this License is strictly prohibited (to the extent such use
 * is covered by a right of the copyright holder of this Software).
 *
 * This Software is provided under the License on an "AS IS" basis and
 * without warranties of any kind concerning the Software, including
 * without limitation merchantability, fitness for a particular purpose,
 * absence of defects or errors, accuracy, and non-infringement of
 * intellectual property rights other than copyright. This disclaimer
 * of warranty is an essential part of the License and a condition for
 * the grant of any rights to this Software.
 *
 * For more details, see http://www.derekmolloy.ie/
 */

#ifndef SPIDEVICE_H_
#define SPIDEVICE_H_
#include<string>
#include<stdint.h>
#include"BusDevice.h"

#define SPI_PATH "/dev/spidev" /**< The general path to an SPI device **/

namespace exploringBB{

/**
 * @class SPIDevice
 * @brief Generic SPI Device class that can be used to connect to any type of SPI device and read or write to its registers
 */
class SPIDevice:public BusDevice {
public:
	/// The SPI Mode
	enum SPIMODE{
		MODE0 = 0,   //!< Low at idle, capture on rising clock edge
		MODE1 = 1,   //!< Low at idle, capture on falling clock edge
		MODE2 = 2,   //!< High at idle, capture on falling clock edge
		MODE3 = 3    //!< High at idle, capture on rising clock edge
	};

private:
	std::string filename;  //!< The precise filename for the SPI device
public:
	SPIDevice(unsigned int bus, unsigned int device);
	virtual int open();
	virtual unsigned char readRegister(unsigned int registerAddress);
	virtual unsigned char* readRegisters(unsigned int number, unsigned int fromAddress=0);
	virtual int writeRegister(unsigned int registerAddress, unsigned char value);
	virtual void debugDumpRegisters(unsigned int number = 0xff);
	virtual int write(unsigned char value);
	virtual int write(unsigned char value[], int length);
	virtual int setSpeed(uint32_t speed);
	virtual int setMode(SPIDevice::SPIMODE mode);
	virtual int setBitsPerWord(uint8_t bits);
	virtual void close();
	virtual ~SPIDevice();
	virtual int transfer(unsigned char read[], unsigned char write[], int length);
private:
	SPIMODE mode;     //!< The SPI mode as per the SPIMODE enumeration
	uint8_t bits;     //!< The number of bits per word
	uint32_t speed;   //!< The speed of transfer in Hz
	uint16_t delay;   //!< The transfer delay in usecs
};

} /* namespace exploringBB */

#endif /* SPIDEVICE_H_ */













// /*
// MIT License
// Copyright (c) 2017 DeeplyEmbedded
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// * SPI.h
// *
// *  Created on: Sep 4, 2017
// *  Author: Vinay Divakar
// */ 

// #ifndef SPI_H_
// #define SPI_H_

// #include<stdint.h>

// /* Generic Definitions */
// #define SPIDEV_BYTES_NUM                 8
// #define SPIDEV_DATA_BITS_NUM             8
// #define SPIDEV_DELAY_US	                 0
// #define SPI_SS_HIGH                      1
// #define SPI_SS_LOW                       0
// #define SPI_ONE_BYTE                     1

// /* No. of bytes per transaction */
// #define NO_OF_BYTES                      2

// /*Definitions specific to spidev1.0 */
// #define SPIDEV1_PATH                     "/dev/spidev1.0"
// #define SPIDEV1_BUS_SPEED_HZ             50000


// /* Enum SPI Modes*/
// typedef enum{
// 	SPI_MODE0 = 0,
// 	SPI_MODE1 = 1,
// 	SPI_MODE2 = 2,
// 	SPI_MODE3 = 3
// }SPI_MODE;

// /*SPI device configuration structure*/
// typedef struct{
// 	char* spi_dev_path;
// 	int fd_spi;
// 	unsigned long spi_bytes_num;
// 	unsigned long spi_bus_speedHZ;
// 	unsigned char ss_change;
// 	unsigned short spi_delay_us;
// 	unsigned char spi_data_bits_No;
// 	unsigned char spi_mode;
// }SPI_DeviceT, *SPI_DevicePtr;

// /* SPI API's*/
// extern int Open_device(char *spi_dev_path, int *fd);
// extern int Set_SPI_mode(int fd, unsigned char spi_mode);
// extern int Set_SPI_bits(int fd, unsigned char bits_per_word);
// extern int Set_SPI_speed(int fd, unsigned long bus_speed_HZ);
// extern void SPI_Config_init(unsigned long spi_bytes_no, unsigned long spi_bus_speed,
//                 unsigned char chip_select, unsigned short spi_delay,
//                 unsigned char spi_bits_No, unsigned char mode_spi, SPI_DevicePtr SPI_devicePtr);

// /* API's to initialize and use spidev1.0 - Configure the Bus as you like*/
// extern int SPI_DEV1_init(unsigned long spi_bytes_no, unsigned long spi_bus_speed,
//                                            unsigned char chip_select, unsigned short spi_delay,
//                                            unsigned char spi_bits_No, unsigned char mode_spi);
// extern int SPIDEV1_transfer(unsigned char *send, unsigned char *receive,
//                 unsigned char bytes_num);
// extern unsigned char SPIDEV1_single_transfer(unsigned char data_byte);

// #endif /* SPI_H_ */