# CubeSat Code Sample

Coding sample for NASA/GSFC Postbaccalaureate Program.

This was a capstone project in the final year of my Bachelor's degree for my major in Space Engineering. We were required to design and implement a prototype for a CubeSat with both software and hardware. The purpose of this project was to demonstrate the basic functionality required for a typical CubeSat and implement our own science mission with a payload. In this, we decided to include a spectral sensor as our payload object intended to measure the absorption of molecular species in Earth's atmosphere. We decided to write all the software in C++. I have included files that were used for the On-Board Computer (OBC) that were primarily written by myself. These files include overall functionality and interfacing with the various systems involved in the CubeSat, so many of the included files have not been included in this repository.

A brief description of each class is as follows:

AS7263: Hardware interfacing with the payload device, a spectral sensor, and obtain its measurements  
BusDevice: Parent class for I2C and SPI devices, which were used for communicating between various hardware systems involved in the CubeSat  
INA219: Hardware interfacing with the current sensor to measure the current flowing to each device  
Payload: Collect and transmit payload data to the device  
Satellite: Overall class to handle various modes of operation  
WholeOrbit: Encode Whole Orbit Data (WOD) intended for transmission  
