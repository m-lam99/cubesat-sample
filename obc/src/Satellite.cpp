#include "Satellite.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <math.h>

#include <iostream>


Satellite::Satellite()
    : current_sensor_batt_(1, INA219_ADDRESS_BATT),
      gps_(),
      wod_(&gps_, mode_, &current_sensor_batt_),
      // adc1_(2, ADC_ADDRESS1),
      // adc2_(2, ADC_ADDRESS2),
      imu_(2, BNO055_ADDRESS_A),
      payload_(&gps_),
      prop_GPIO_(23),
      burn_GPIO_(59),
      transceiver_(),
      Controller(0, 0, 0),
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

    imu_.setExtCrystalUse(true);

    std::cout << "SATELLITE initialisied" << std::endl;
}

Satellite::~Satellite() {}

bool Satellite::pointSatellite(double phi, double theta, double psi)
{

    /* Display calibration status for each sensor. */
    uint8_t system, gyro, accel, mag = 0;
    imu_.getCalibration(&system, &gyro, &accel, &mag);
    std::cout << "CALIBRATION: Sys=" << (int)system << " Gyro=" << (int)gyro
            << " Accel=" << (int)accel << " Mag=" << (int)mag << std::endl;

    // change pointing direction
    Controller.changeTarget(phi, theta, psi);

    imu::Vector<3> signal;

    // Display Quaternions
    imu::Quaternion quat = imu_.getQuat();
    imu::Vector eulers = imu_.getVector(BNO055::VECTOR_EULER);
    std::cout << " eX: " << (int)eulers.x() << " eY: " << (int)eulers.y() << " eZ: " << (int)eulers.z() << "\t";
    // std::cout << "qW: " << quat.w() << " qX: " << quat.x() << " qY: " << quat.y() << " qZ: " << quat.z() << "\t\t";

    // /* Display Angular Velocities rad/s */
    imu::Vector<3> rps = imu_.getRPS();
    // std::cout << "X: " << rps.x() << " Y: " << rps.y() << " Z: "
    //           << rps.z() << "\t\t";

    signal = Controller.runControlAlgorithm(quat, rps);

    for (int i; i < 3; i++) {
        signal[i] = 100*abs(signal[i])/3.1415;
    }

    std::cout << "DUTY CYCLE SIGNALS: " << (int)signal[0] << " " << (int)signal[1] << " " <<(int)signal[2] << std::endl;


    // ACTUATE THE SIGNAL
    mag_x.setDutyCycle((unsigned int)(signal.x()));
    mag_y.setDutyCycle((unsigned int)(signal.y()));
    mag_z.setDutyCycle((unsigned int)(signal.z()));

    mag_x.run();
    mag_y.run();
    mag_z.run();

    imu::Vector<3> mags = imu_.getVector(BNO055::VECTOR_MAGNETOMETER);
    std::cout << "Xmag: " << (int)mags.x() <<  " Ymag: " << (int)mags.y() << " Zmag: "
        << (int)mags.z() << "\t" << std::endl;

    return Controller.getTolerance();
}

int Satellite::detumbling()
{
    /* Display calibration status for each sensor. */
    uint8_t system, gyro, accel, mag = 0;
    imu_.getCalibration(&system, &gyro, &accel, &mag);
    std::cout << "CALIBRATION: Sys=" << (int)system << " Gyro=" << (int)gyro
            << " Accel=" << (int)accel << " Mag=" << (int)mag << std::endl;

    imu::Vector<3> mags = imu_.getVector(BNO055::VECTOR_MAGNETOMETER);
    imu::Vector<3> rps = imu_.getRPS();


    std::cout << "Rad x: " << rps.x() << " Rad y: " << rps.y() << " Rad z: "
        << rps.z() << "\t";

    // Detumble controller
    imu::Vector<3> directions = Controller.detumble(rps, mags);

    // std::cout << "directions: " << directions[2] << std::endl;

    if (directions[0] == 0 && directions[1] == 0 && directions[2] == 0) {
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
    /* Display Magnetometer Reading uT */
    std::cout << "Xmag: " << (int)mags.x() <<  " Ymag: " << (int)mags.y() << " Zmag: "
        << (int)mags.z() << "\t";

    usleep(1000000);

    return 1;
}

int Satellite::runmagtorquer(PWM mag) {
    unsigned int dc = 75;
    mag.setDutyCycle(dc);
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

    return 1;
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

    return 1;
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
    bool start_flag = false; 
    std::vector<uint8_t> filtered{}; 
    // Assume nothing received 
    receive = 0; 

    for (unsigned int i = 0; i < transceiver_.MAX_BYTES_AX25; i++)
    {
        std::cout << message[i];
        if (message[i] == '#')
        {
            start_flag = true;
        }
        else if (message[i] == 'R' && start_flag){
            receive = 1;
        }
        else{
            start_flag = false; 
        }
    }
    std::cout << std::endl;

    // UNPACK message RICKYS HAX
    for (unsigned int i = 0; i < message.size();i++)
    {
        if (message[i] == 'M'){
            filtered.push_back(message[i]);
            filtered.push_back(message[i+1]);
            mode_ = message[i+1];
            std::cout << "mode is" << mode_ << std::endl;

            break;
        }
        else if(message[i] == 0x54 && i+2 < message.size()){
            if(message[i+1] == 0x58){
                filtered.push_back(message[i]);
                filtered.push_back(message[i+1]);
                filtered.push_back(message[i+2]);

                break; 
            }
        }
        else if(i+2 < message.size() ){
            if(message[i] == 0x53 && message[i+1] == 0x4F && message[i+2] == 0x53){
                filtered.push_back(message[i]);
                filtered.push_back(message[i+1]);
                filtered.push_back(message[i+2]);
            }
            
        }
    }

    return filtered;
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
