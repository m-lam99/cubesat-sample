/*
 * SPIDevice.cpp  Created on: 22 May 2014
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

#include "SPIDevice.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
using namespace std;

#define HEX(x) setw(2) << setfill('0') << hex << (int)(x)  //!< Macro for filling in leading 0 on HEX outputs

namespace exploringBB {

/**
 * The constructor for the SPIDevice that sets up and opens the SPI connection.
 * The destructor will close the SPI file connection.
 * @param bus The SPI bus number X (first digit after spidevX.Y)
 * @param device The device on the bus Y (second digit after spidevX.Y)
 */
SPIDevice::SPIDevice(unsigned int bus, unsigned int device):
	BusDevice(bus,device) {
	stringstream s;
	s << SPI_PATH << bus << "." << device;
	this->filename = string(s.str());
	this->mode = SPIDevice::MODE3;
	this->bits = 8;
	this->speed = 500000;
	this->delay = 0;
	this->open();
}

/**
 * This method opens the file connection to the SPI device.
 * @return 0 on a successful open of the file
 */
int SPIDevice::open(){
	//cout << "Opening the file: " << filename.c_str() << endl;
	if ((this->file = ::open(filename.c_str(), O_RDWR))<0){
		perror("SPI: Can't open device.");
		return -1;
	}
	if (this->setMode(this->mode)==-1) return -1;
	if (this->setSpeed(this->speed)==-1) return -1;
	if (this->setBitsPerWord(this->bits)==-1) return -1;
	return 0;
}

/**
 * Generic method to transfer data to and from the SPI device. It is used by the
 * following methods to read and write registers.
 * @param send The array of data to send to the SPI device
 * @param receive The array of data to receive from the SPI device
 * @param length The length of the array to send
 * @return -1 on failure
 */
int SPIDevice::transfer(unsigned char send[], unsigned char receive[], int length){
	struct spi_ioc_transfer	transfer;
	memset(&transfer,0,sizeof(transfer));
	transfer.tx_buf = (unsigned long) send;
	transfer.rx_buf = (unsigned long) receive;
	transfer.len = length;
	transfer.speed_hz = this->speed;
	transfer.bits_per_word = this->bits;
	transfer.delay_usecs = this->delay;
	int status = ioctl(this->file, SPI_IOC_MESSAGE(1), &transfer);
	if (status < 0) {
		perror("SPI: SPI_IOC_MESSAGE Failed");
		return -1;
	}
	return status;
}

/**
 * A method to read a single register at the SPI address
 * @param registerAddress the address of the register from the device datasheet
 * @return the character that is returned from the address
 */
unsigned char SPIDevice::readRegister(unsigned int registerAddress){
	unsigned char send[2], receive[2];
	memset(send, 0, sizeof send);
	memset(receive, 0, sizeof receive);
	send[0] = (unsigned char) (0x80 | registerAddress);
	this->transfer(send, receive, 2);
	//cout << "The value that was received is: " << (int) receive[1] << endl;
	return receive[1];
}

/**
 * A method to read a number of registers as a data array
 * @param number the number of registers to read
 * @param fromAddress the starting address of the block of data
 * @return the data array that is returned (memory allocated by the method)
 */
unsigned char* SPIDevice::readRegisters(unsigned int number, unsigned int fromAddress){
	unsigned char* data = new unsigned char[number];
	unsigned char send[number+1], receive[number+1];
	memset(send, 0, sizeof send);
	send[0] =  (unsigned char) (0x80 | 0x40 | fromAddress); //set read bit and MB bit
	this->transfer(send, receive, number+1);
	memcpy(data, receive+1, number);  //ignore the first (address) byte in the array returned
	return data;
}

/**
 *  A write method that writes a single character to the SPI bus
 *  @param value the value to write to the bus
 *  @return returns 0 if successful
 */
int SPIDevice::write(unsigned char value){
	unsigned char null_return = 0x00;
	//printf("[%02x]", value);
	this->transfer(&value, &null_return, 1);
	return 0;
}

/**
 *  A write method that writes a block of data of the length to the bus.
 *  @param value the array of data to write to the device
 *  @param length the length of the data array
 *  @return returns 0 if successful
 */
int SPIDevice::write(unsigned char value[], int length){
	unsigned char null_return = 0x00;
	this->transfer(value, &null_return, length);
	return 0;
}

/**
 *  Writes a value to a defined register address (check the datasheet for the device)
 *  @param registerAddress the address of the register to write to
 *  @param value the value to write to the register
 *  @return returns 0 if successful
 */
int SPIDevice::writeRegister(unsigned int registerAddress, unsigned char value){
	unsigned char send[2], receive[2];
	memset(receive, 0, sizeof receive);
	send[0] = (unsigned char) registerAddress;
	send[1] = value;
	//cout << "The value that was written is: " << (int) send[1] << endl;
	this->transfer(send, receive, 2);
	return 0;
}

/**
 *  A simple method to dump the registers to the standard output -- useful for debugging
 *  @param number the number of registers to dump
 */
void SPIDevice::debugDumpRegisters(unsigned int number){
	cout << "SPI Mode: " << this->mode << endl;
	cout << "Bits per word: " << (int)this->bits << endl;
	cout << "Max speed: " << this->speed << endl;
	cout << "Dumping Registers for Debug Purposes:" << endl;
	unsigned char *registers = this->readRegisters(number);
	for(int i=0; i<(int)number; i++){
		cout << HEX(*(registers+i)) << " ";
		if (i%16==15) cout << endl;
	}
	cout << dec;
}

/**
 *   Set the speed of the SPI bus
 *   @param speed the speed in Hz
 */
int SPIDevice::setSpeed(uint32_t speed){
	this->speed = speed;
	if (ioctl(this->file, SPI_IOC_WR_MAX_SPEED_HZ, &this->speed)==-1){
		perror("SPI: Can't set max speed HZ");
		return -1;
	}
	if (ioctl(this->file, SPI_IOC_RD_MAX_SPEED_HZ, &this->speed)==-1){
		perror("SPI: Can't get max speed HZ.");
		return -1;
	}
	return 0;
}

/**
 *   Set the mode of the SPI bus
 *   @param mode the enumerated SPI mode
 */
int SPIDevice::setMode(SPIDevice::SPIMODE mode){
	this->mode = mode;
	if (ioctl(this->file, SPI_IOC_WR_MODE, &this->mode)==-1){
		perror("SPI: Can't set SPI mode.");
		return -1;
	}
	if (ioctl(this->file, SPI_IOC_RD_MODE, &this->mode)==-1){
		perror("SPI: Can't get SPI mode.");
		return -1;
	}
	return 0;
}

/**
 *   Set the number of bits per word of the SPI bus
 *   @param bits the number of bits per word
 */
int SPIDevice::setBitsPerWord(uint8_t bits){
	this->bits = bits;
	if (ioctl(this->file, SPI_IOC_WR_BITS_PER_WORD, &this->bits)==-1){
		perror("SPI: Can't set bits per word.");
		return -1;
	}
	if (ioctl(this->file, SPI_IOC_RD_BITS_PER_WORD, &this->bits)==-1){
		perror("SPI: Can't get bits per word.");
		return -1;
	}
	return 0;
}

/**
 *   Close the SPI device
 */
void SPIDevice::close(){
	::close(this->file);
	this->file = -1;
}

/**
 *   The destructor closes the SPI bus device
 */
SPIDevice::~SPIDevice() {
	this->close();
}

} /* namespace exploringBB */







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
// * SPI.c
// *
// *  Created on  : September 5, 2017
// *  Author      : Vinay Divakar
// *  Description : This is an SPI Library for the BeagleBone that consists of the API's to enable
//                   full duplex SPI transactions.
// *  Note        : This Library has been tested to work in all the modes i.e. SPI_MODE0, SPI_MODE1,
// *                SPI_MODE2 and SPI_MODE3. At present, this Library only supports spidev1.0, however
// *                it can be extended to support other spidev1.x or spidev2.x as well. 
// */

// /*Custom Libs Includes*/
// #include "SPI.h"

// /*Lib Includes*/
// #include<stdio.h>
// #include<fcntl.h>
// #include<string.h>
// #include<stdint.h>
// #include<unistd.h>
// #include<sys/ioctl.h>
// #include<linux/spi/spidev.h>

// /* Static objects for spidev1.0 */
// static SPI_DeviceT SPI_device1;
// static struct spi_ioc_transfer	transfer_spidev1;

// /* Globals */
// unsigned char Tx_spi[SPIDEV_BYTES_NUM];
// unsigned char RX_spi[SPIDEV_BYTES_NUM];

// /****************************************************************
//  * Function Name : Open_device
//  * Description   : Opens the SPI device to use
//  * Returns       : 0 on success, -1 on failure
//  * Params        @spi_dev_path: Path to the SPI device
//  *               @fd: Variable to store the file handler
//  ****************************************************************/
// int Open_device(char *spi_dev_path, int *fd)
// {
// 	if((*fd = open(spi_dev_path, O_RDWR))<0)
// 		return -1;
// 	else
// 		return 0;
// }

// /****************************************************************
//  * Function Name : Set_SPI_mode
//  * Description   : Set the SPI mode of operation
//  * Returns       : 0 on success, -1 on failure
//  * Params        @fd: File handler
//  *               @spi_mode: SPI Mode
//  ****************************************************************/
// int Set_SPI_mode(int fd, unsigned char spi_mode)
// {
// 	int ret = 0;
// 	if(ioctl(fd, SPI_IOC_WR_MODE, &spi_mode)==-1)
// 		ret = -1;
// 	if(ioctl(fd, SPI_IOC_RD_MODE, &spi_mode)==-1)
// 		ret = -1;
// 	return (ret);
// }

// /****************************************************************
//  * Function Name : Set_SPI_bits
//  * Description   : Set the No. of bits/transaction
//  * Returns       : 0 on success, -1 on failure
//  * Params        @fd: File handler
//  *               @bits_per_word: No. of bits
//  ****************************************************************/
// int Set_SPI_bits(int fd, unsigned char bits_per_word)
// {
// 	int ret = 0;
// 	if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word)==-1)
// 		ret = -1;
// 	if(ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word)==-1)
// 		ret = -1;
// 	return (ret);
// }

// /****************************************************************
//  * Function Name : Set_SPI_speed
//  * Description   : Set the SPI bus frequency in Hertz(Hz)
//  * Returns       : 0 on success, -1 on failure
//  * Params        @fd: File handler
//  *               @bus_speed_HZ: Bus speed
//  ****************************************************************/
// int Set_SPI_speed(int fd, unsigned long bus_speed_HZ)
// {
// 	int ret = 0;
// 	if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &bus_speed_HZ)==-1)
// 		ret = -1;
// 	if(ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &bus_speed_HZ)==-1)
// 		ret = -1;
// 	return (ret);
// }

// /****************************************************************
//  * Function Name : SPI_Config_init
//  * Description   : Initialization configuration for the SPI
//  *                 device
//  * Returns       : None
//  * Params        @spi_bytes_no: File handler
//  *               @spi_bus_speed: Bus speed
//  *               @chip_select: chip select line
//  *               @spi_delay: delay in us
//  *               @spi_bits_No: No. of bits/transaction
//  *               @mode_spi: SPI Mode
//  *               @SPI_devicePtr: Points to the SPI device
//  *               configuration structure.
//  ****************************************************************/
// void SPI_Config_init(unsigned long spi_bytes_no, unsigned long spi_bus_speed,
// 		unsigned char chip_select, unsigned short spi_delay,
// 		unsigned char spi_bits_No, unsigned char mode_spi, SPI_DevicePtr SPI_devicePtr)
// {
// 	SPI_devicePtr->spi_bytes_num = spi_bytes_no;
// 	SPI_devicePtr->spi_bus_speedHZ = spi_bus_speed;
// 	SPI_devicePtr->ss_change = chip_select;
// 	SPI_devicePtr->spi_delay_us = spi_delay;
// 	SPI_devicePtr->spi_data_bits_No = spi_bits_No;
// 	SPI_devicePtr->spi_mode = mode_spi;
// 	SPI_devicePtr->fd_spi = 0;
// 	SPI_devicePtr->spi_dev_path = NULL;
// }

// /****************************************************************
//  * Function Name : SPI_DEV1_init
//  * Description   : Initialize and set up spidev1.0
//  * Returns       : 0 on success, -1 on failure
//  * Params        : None
//  ****************************************************************/
// int SPI_DEV1_init(unsigned long spi_bytes_no, unsigned long spi_bus_speed,
//                                            unsigned char chip_select, unsigned short spi_delay,
//                                            unsigned char spi_bits_No, unsigned char mode_spi)
// {
// 	/* Initialize the parameters for spidev1.0 structure */
// 	SPI_Config_init(spi_bytes_no, spi_bus_speed, chip_select,
// 			spi_delay, spi_bits_No, mode_spi, &SPI_device1);

// 	/* Assign the path to the spidev1.0 for use */
// 	SPI_device1.spi_dev_path = SPIDEV1_PATH;

// 	/* Open the spidev1.0 device */
// 	if(Open_device(SPI_device1.spi_dev_path, &SPI_device1.fd_spi) == -1)
// 	{
// 		perror("SPI: Failed to open spidev1.0 |");
// 		return -1;
// 	}

// 	/* Set the SPI mode for RD and WR operations */
// 	if(Set_SPI_mode(SPI_device1.fd_spi, SPI_device1.spi_mode) == -1)
// 	{
// 		perror("SPI: Failed to set SPIMODE |");
// 		return -1;
// 	}

// 	/* Set the No. of bits per transaction */
// 	if(Set_SPI_bits(SPI_device1.fd_spi, SPI_device1.spi_data_bits_No) == -1)
// 	{
// 		perror("SPI: Failed to set No. of bits per word |");
// 		return -1;
// 	}

// 	/* Set the SPI bus speed in Hz */
// 	if(Set_SPI_speed(SPI_device1.fd_spi, SPI_device1.spi_bus_speedHZ) == -1)
// 	{
// 		perror("SPI: Failed to set SPI bus frequency |");
// 		return -1;
// 	}

// 	/* Initialize the spi_ioc_transfer structure that will be passed to the
// 	 * KERNEL to define/configure each SPI Transactions*/
// 	transfer_spidev1.tx_buf = 0;
// 	transfer_spidev1.rx_buf = 0;
// 	transfer_spidev1.pad = 0;
// 	transfer_spidev1.tx_nbits = 0;
// 	transfer_spidev1.rx_nbits = 0;
// 	transfer_spidev1.len = SPI_device1.spi_bytes_num;
// 	transfer_spidev1.speed_hz = SPI_device1.spi_bus_speedHZ;
// 	transfer_spidev1.delay_usecs = SPI_device1.spi_delay_us;
// 	transfer_spidev1.bits_per_word = SPI_device1.spi_data_bits_No;
// 	transfer_spidev1.cs_change = SPI_device1.ss_change;

// 	return 0;
// }

// /****************************************************************
//  * Function Name : SPIDEV1_transfer
//  * Description   : Performs a SPI transaction
//  * Returns       : 0 on success, -1 on failure
//  * Params        @send: Points to the buffer containing the data
//  *               to be sent
//  *               @receive: Points to the buffer into which the
//  *               received bytes are stored
//  * NOTE          : Good for multiple transactions
//  ****************************************************************/
// int SPIDEV1_transfer(unsigned char *send, unsigned char *receive,
// 		unsigned char bytes_num)
// {
// 	/* Points to the Tx and Rx buffer */
// 	transfer_spidev1.tx_buf = (unsigned long)send;
// 	transfer_spidev1.rx_buf = (unsigned long)receive;

// 	/* Override No. of bytes per transaction */
// 	transfer_spidev1.len = bytes_num;

// 	/* Perform a SPI Transaction */
// 	if (ioctl(SPI_device1.fd_spi, SPI_IOC_MESSAGE(1), &transfer_spidev1)<0)
// 	{
// 		perror("SPI: SPI_IOC_MESSAGE Failed |");
// 		return -1;
// 	}
// 	return 0;
// }

// /****************************************************************
//  * Function Name : SPIDEV1_single_transfer
//  * Description   : Performs a single full duplex SPI transaction
//  * Returns       : 0 or data on success, -1 on failure
//  * Params        @data_byte: Points to the address of the variable
//  *               containing the data to be sent.
//  * NOTE          : Good for single transactions
//  ****************************************************************/
// unsigned char SPIDEV1_single_transfer(unsigned char data_byte)
// {
// 	unsigned char rec_byte = 0;

// 	/* Override No. of bytes to send and receive one byte */
// 	transfer_spidev1.len = SPI_ONE_BYTE;

// 	/* Points to the address of Tx and Rx variable  */
// 	transfer_spidev1.tx_buf = (unsigned long)&data_byte;
// 	transfer_spidev1.rx_buf = (unsigned long)&rec_byte;

// 	/* Perform an SPI Transaction */
// 	if (ioctl(SPI_device1.fd_spi, SPI_IOC_MESSAGE(1), &transfer_spidev1)<0)
// 	{
// 		perror("SPI: SPI_IOC_MESSAGE Failed |");
// 		rec_byte = -1;
// 	}

// 	return (rec_byte);
// }