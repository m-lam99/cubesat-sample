#include "Satellite.h"
#include "stationkeeping.h"

#include <unistd.h>
#include <iostream>
#include <string>

#include <iostream>

Satellite::Satellite()
    : current_sensor_batt_(1, INA219_ADDRESS_BATT),
      gps_(),
      wod_(&gps_, mode_, &current_sensor_batt_),
      adc1_(2, ADC_ADDRESS1),
      adc2_(2, ADC_ADDRESS2),
      imu_(2, BNO055_ADDRESS_A),
      payload_(&gps_),
      prop_GPIO_(23),
      burn_GPIO_(59),
      transceiver_(),
      Controller(0, 1.57, 0) // initial pointing direction
{
    if (prop_GPIO_.setDirection(OUTPUT) == -1)
    {
        prop_valid_ = 0;
    }
    else
    {
        prop_valid_ = 1;
    }

    if (burn_GPIO_.setDirection(OUTPUT) == -1)
    {
        burn_valid_ = 0;
    }
    else
    {
        burn_valid_ = 1;
        burn_GPIO_.setValue(LOW);
    }

    // Wait for imu to initialise
    std::cout << "Orientation Sensor Raw Data Test" << std::endl;
    if (!imu_.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        std::cout << "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!" << std::endl;
        while (1)
            ;
    }

    usleep(1000);

    /* System Status */
    int8_t status = imu_.getSystemStatus();
    std::cout << "System status: " << (int)status << std::endl;

    /* Display the current temperature */
    int8_t temp = imu_.getTemp();
    std::cout << "Current Temperature: " << (int)temp << " C" << std::endl;

    std::cout << "Calibration status values: 0=uncalibrated, 3=fully calibrated" << std::endl;

    imu_.setExtCrystalUse(true);

    std::cout << "SATELLITE initialisied" << std::endl;
}

Satellite::~Satellite() {}

bool Satellite::pointSatellite(double phi, double theta, double psi)
{

    // change pointing direction
    Controller.changeTarget(phi, theta, psi);

    imu::Vector<3> signal;

    // Display Quaternions
    imu::Quaternion quat = imu_.getQuat();
    std::cout << "qW: " << quat.w() << " qX: " << quat.x() << " qY: " << quat.y() << " qZ: " << quat.z() << "\t\t";

    /* Display Angular Velocities rad/s */
    imu::Vector<3> rps = imu_.getRPS();
    std::cout << "X: " << rps.x() << " Y: " << rps.y() << " Z: "
              << rps.z() << "\t\t";

    signal = Controller.runControlAlgorithm(quat, rps);

    // ACRTUATE THE SIGNAL?
    std::cout << "SIGNAL NOT ACTUATED" << std::endl;

    return Controller.getTolerance();
}

int Satellite::detumbling()
{

    // return true if finished
    return true;
}

int Satellite::payloadDataCollection()
{
    payload_data_.push(payload_.getData());
    return 1;
}

int Satellite::payloadDataTransmission()
{
    Payload::payload_data_t data_packet = payload_data_.front();

    unsigned char data_transmit[sizeof(data_packet)];
    memcpy(data_transmit, &data_packet, sizeof(data_packet));

    message_.payload = data_transmit;
    message_.npayload = 18;
    message_.source = srcaddr;
    message_.destination = destaddr;
    message_.dataType = 1;
    message_.commandResponse = 0;
    message_.controlType = 0;
    message_.sendSequence = 0;
    message_.receiveSequence = 0;

    encodedMsg_ = ax25::encode(&message_);
    if (encodedMsg_ != NULL)
    {
        // int success = sendMessage(encodedMsg);

        wod_data_.pop();
        return 1;
    }
    else
    {
        return 0;
    }
}

int Satellite::checkBattery()
{
    if (current_sensor_batt_.busVoltage() < BATTERY_THRESHOLD)
    {
        // GO INTO IDLE MODE
        return 0;
    }
    return 1;
}

int Satellite::checkOrbit()
{
}

int Satellite::orbitCorrection()
{
    Vec3 pointVec;
    GPS::loc_t llh1;
    GPS::loc_t llh2;

    gps_.get_location(&llh1);
    usleep(1000000);
    gps_.get_location(&llh2);
    // Get prograde vector and point to it
    getPrograde(pointVec);
    vec2Euler(pointVec);

    pointSatellite(pointVec.x, pointVec.y, pointVec.z);

    // Not sure how to trigger propulsion
    vector<0> burn;
    propulsion(burn);
}

int Satellite::wodCollection()
{
    wod_data_.push(wod_.GetData());
    std::cout << (int)wod_data_.back().current_3v3 << std::endl;
    return 1;
}

uint32_t Satellite::getTime()
{ // from gps
    GPS::loc_t *location_data;
    gps_.get_location(location_data);
    return location_data->epoch;
}

int Satellite::wodTransmission()
{
    WholeOrbit::wod_t data_packet = wod_data_.front();

    unsigned char data_transmit[sizeof(data_packet)];
    memcpy(data_transmit, &data_packet, sizeof(data_packet));

    message_.payload = data_transmit; // double check
    message_.npayload = 12;
    message_.source = srcaddr;
    message_.destination = destaddr;
    message_.dataType = 0;
    message_.commandResponse = 0;
    message_.controlType = 0;
    message_.sendSequence = 0;
    message_.receiveSequence = 0;

    encodedMsg_ = ax25::encode(&message_);

    std::cout << "WOD transmit" << std::endl;
    if (encodedMsg_ != NULL)
    {
        // Transmit message
        // transceiver_.TransmitMessage(encodedMsg_);

        // int success = sendMessage(encodedMsg);
        wod_data_.pop();
        return 1;
    }
    else
    {
        return 0;
    }
}

int Satellite::deployment()
{
    if (burn_GPIO_.setValue(HIGH) == -1 || !burn_valid_)
    {
        return 0;
    }
    else
    {
        usleep(1000000);
        burn_GPIO_.setValue(LOW);
        return 1;
    }
}

int Satellite::checkDayTime()
{
    int isDaytime = 1;

    // All channels
    float max_voltage = 0.0;

    for (int i = 0; i < 3; i++)
    {
        if (adc1_.getVoltage(i) > max_voltage)
        {
            max_voltage = adc1_.getVoltage(i);
        }

        if (adc2_.getVoltage(i) > max_voltage)
        {
            max_voltage = adc2_.getVoltage(i);
        }
    }

    if (max_voltage < SUN_SENOR_THRESHOLD)
    {
        isDaytime = 0;
    }

    return isDaytime;
}

void Satellite::transmitMessage(std::vector<uint8_t> message)
{
    transceiver_.TransmitMessage(message);
    usleep(1000000);
}

std::vector<uint8_t> Satellite::checkTransceiver()
{
    // checks for messages
    transceiver_.SendCommand(transceiver_.CMD_RECEIVE_MODE_CONFIG);
    std::vector<uint8_t> message = transceiver_.ReceiveData();
    std::cout << "MESSAGE: ";

    for (unsigned int i = 0; i < transceiver_.MAX_BYTES_AX25; i++)
    {
        if (message[0] == '#' && message[1] == 'R' && message.size() >= 3 && i > 3 && i < 18)
        {
            std::cout << message[i];
            receive = 1;
        }
    }
    std::cout << std::endl;

    // UNPACK message

    return message;
}

int Satellite::propulsion(std::vector<int> array)
{
    int n = array.size();
    for (int i = 0; i < n; ++i)
    {
        if (i % 2 == 0)
        {
            // Turn on
            if (prop_GPIO_.setValue(HIGH) == -1 || !prop_valid_)
            {
                return 0;
            }
        }
        else
        {
            // Turn off
            if (prop_GPIO_.setValue(LOW) == -1 || !prop_valid_)
            {
                return 0;
            }
        }
        usleep(array[i] * 1000000);

        // std::cout << array[i] << std::endl;
    }
    return 1;
}
