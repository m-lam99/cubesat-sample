#include "Satellite.h"

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
      Controller(0, 1.57, 0),
      mag_x(PWM_0A),
      mag_y(PWM_1A),
      mag_z(PWM_0B) // initial pointing direction
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

    // ACTUATE THE SIGNAL
    mag_x.setDutyCycle((float)(signal.x()/1.57)*100);
    mag_y.setDutyCycle((float)(signal.y()/1.57)*100);
    mag_z.setDutyCycle((float)(signal.y()/1.57)*100);

    mag_x.run();
    mag_y.run();
    mag_z.run();
    std::cout << "SIGNAL ACTUATED" << std::endl;

    return Controller.getTolerance();
}

int Satellite::detumbling()
{
    imu::Vector<3> mags = imu_.getVector(BNO055::VECTOR_MAGNETOMETER);
    
    // std::cout << "X: " << mags.x() << " Y: " << mags.y() << " Z: "
    //     << mags.z() << "\t\t";
    imu::Vector<3> rps = imu_.getRPS();
    std::cout << "Rad x: " << rps.x() << " Rad y: " << rps.y() << " Rad z: "
        << rps.z() << "\t\t";

    // Detumble controller
    imu::Vector<3> directions = Controller.detumble(rps, mags);

    if (directions[0] == 0 & directions[1] == 0 & directions[2] == 0) {
        mag_x.stop();
        mag_y.stop();
        mag_z.stop();
        return 0;
    }
    
    if (directions[0] == 1) {
        runmagtorquer(mag_x);
    }
    if (directions[1] == 1) {
        runmagtorquer(mag_y);
    }
    if (directions[2] == 1) {
        runmagtorquer(mag_z);
    }

    std::cout << "Mag Duty Cycle: " << mag_z.getDutyCycle() << std::endl;

    return 1;
}

int Satellite::runmagtorquer(PWM mag) {
    mag.setDutyCycle(75.0f);
    mag.setPolarity(PWM::ACTIVE_HIGH);
    mag.run();

    return 1;
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
    gps_.get_location(&sat_pos);
    if (sat_pos.altitude < 230000) {
        orbitCorrection();
    }
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
    // getPrograde(&pointVec, &llh1, &llh2);
    // vec2Euler(&pointVec);

    pointSatellite(pointVec.x, pointVec.y, pointVec.z);

    // Not sure how to trigger propulsion
    std::vector<int> prop_vals;
    prop_vals.push_back(10);
    propulsion(prop_vals);
}

int Satellite::wodCollection()
{
    wod_data_.push(wod_.GetData());
    std::cout << (int)wod_data_.back().current_batt << std::endl;
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

    for(int i = 0; i <sizeof(data_packet); i++){
        std::cout<< data_transmit[i];
    }
    std::cout<< std::endl; 

    encodedMsg_ = ax25::encode(&message_);
    //std::vector<uint8_t> test = {0x7e, 0x39, 0x49, 0x61, 0x51, 0x2, 0x2, 0x3e, 0xaa, 0xb2, 0xa6, 0x88, 0xb8, 0xb2, 0xc3, 0x60, 0x0, 0x15, 0xb, 0x53, 0x27, 0x53, 0x72, 0x67, 0x2, 0x43, 0x2, 0xb, 0x7b, 0x1b, 0x53, 0x2, 0x4b, 0x3b, 0x2, 0x4f, 0x7b, 0x57, 0x27, 0x2, 0x23, 0x57, 0x63, 0x6b, 0x53, 0x17, 0x79, 0x22, 0x80, 0x7e};

    // encodedMsg_ = ax25::encode(&test);

    std::cout << "WOD transmit" << std::endl;
    if (encodedMsg_ != NULL)
    {
        std::vector<uint8_t> send_vector;
        // Transmit message
        for(int i = 0; i < encodedMsg_->nbytes; ++i){
           send_vector.push_back(encodedMsg_->bytes[i]);
           std::cout << (int)send_vector.back() << std::endl;
        }
        //transceiver_.TransmitMessage(test);
        //std::cout << test.data() << std::endl;
        transceiver_.TransmitMessage(send_vector);
        
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
        usleep(2000000);
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
        std::cout << message[i];
        if (message[0] == '#' && message[1] == 'R' && message.size() >= 3 && i > 3 && i < 18)
        {
            std::cout << message[i];
            receive = 1;
        }
    }
    std::cout << std::endl;

    // UNPACK message RICKYS HAX
    for (unsigned int i = 0; i < message.size();i++)
    {
        if (message[i] == 'M'){
            mode_ = message[i+1];
            std::cout << "mode is" << mode_ << std::endl;
        }
    }

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