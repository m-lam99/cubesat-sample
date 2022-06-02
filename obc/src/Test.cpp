#include <unistd.h> //for usleep

#include <iostream>
#include <unistd.h> //for usleep
#include <iomanip>  // for setprecision

#include "Test.h"
#include "Computer.h"
#include "controller.h"
#include "Payload.h"

#include <vector>

using namespace exploringBB;
using namespace std;

void Test::runTests(){
    
    cout << "Running Testing Mode" << endl;
    while(1){
        string input; 
        cout << "Testing mode: "; 
        cin >> input; 

        if(input == "gpio"){
            testGPIO();
        }
        else if(input == "current"){
            testINA219(); 
        }
        else if(input == "adc"){
            testADS1015();
        }
        else if(input == "payload"){
            testPayload(); 
        }
        else if(input == "pay"){
            testAS7263();
        }
        else if(input == "pwm"){
            testPWM(PWM_1A);
        }
        else if(input == "imu"){
            testBNO055();
        }
        else if(input == "gps"){
            testGPS();
        }
        else if(input == "wod"){
            wodTest();
        }
        else if(input == "wodEn"){
            wodTestEncoded();
        }
        else if(input == "prop"){
            testPropulsion();
        }
        else if(input == "ttc"){
            testTransceiver();
        }
        else if(input == "24v"){
            test24V();
        }
        // else if (input == "x"){
        //     break;
        // }
                else if(input == "24v"){
            test24V();
        }
        else if (input == "x"){
            break;
        }
        else if(input == "burn"){
            testDeployment(); 
        }
        else if(input == "detum"){
            testDetumble();
        }
        else if(input == "point"){
            testPoint();
        }
        else if(input == "fuckthis"){
            break; 
        }
        else if(input == "wodtransmit"){
            transmitWOD();
        }
    }
}

void Test::testDetumble() {
    Satellite satellite;
    // CControl Controller(0, 1.57, 0);
    while (1) {
        satellite.detumbling();
    }

}

void Test::testPoint() {
    Satellite satellite;
    while(1) {
        satellite.pointSatellite(0.0, 1.57, 0.0);
        usleep(100000);
    }
}

void Test::testDeployment(){
    GPIO burn_GPIO_(59); 

    burn_GPIO_.setDirection(OUTPUT); 
    if (burn_GPIO_.setValue(HIGH) == -1)
    {
        cout << "deployment failed" << endl; 
    }
    else
    {
        usleep(6900000);
        burn_GPIO_.setValue(LOW);
        cout << "Finished Burning" << endl; 
    }

    return; 
}
void Test::testGPIO(){
    GPIO outGPIO(23), inGPIO(46);

    // Basic Output - Flash the LED 10 times, once per second
    outGPIO.setDirection(OUTPUT);
    for (int i = 0; i < 10; i++)
    {
        outGPIO.setValue(HIGH);
        usleep(500000); // micro-second sleep 0.5 seconds
        outGPIO.setValue(LOW);
        usleep(500000);
    }
    // Basic Input example
    inGPIO.setDirection(INPUT);
     cout << "The value of the input is: " << inGPIO.getValue()
              <<  endl;

    // Fast write to GPIO 1 million times
    outGPIO.streamOpen();
    for (int i = 0; i < 1000000; i++)
    {
        outGPIO.streamWrite(HIGH);
        outGPIO.streamWrite(LOW);
    }
    outGPIO.streamClose();
}

void Test::testINA219()
{
    INA219 sensor1(1, 0x45);
    // INA219 sensor2(1, INA219_ADDRESS2);
    // INA219 sensor3(1, INA219_ADDRESS3);
    // INA219 sensor4(1, INA219_ADDRESS4);
    sensor1.configure(INA219::VOLTAGE_RANGE::FSR_32,
                      INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    // sensor2.configure(INA219::VOLTAGE_RANGE::FSR_32,
    // INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    // sensor3.configure(INA219::VOLTAGE_RANGE::FSR_32,
    // INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    // sensor4.configure(INA219::VOLTAGE_RANGE::FSR_32,
    // INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    uint16_t value1 = sensor1.readRegisters(INA219::REGISTERS::CONFIG);
    // uint16_t value2 = sensor2.readRegister(INA219::REGISTERS::CONFIG);
    // uint16_t value3 = sensor3.readRegister(INA219::REGISTERS::CONFIG);
    // uint16_t value4 = sensor4.readRegister(INA219::REGISTERS::CONFIG);
    // sensor1.calibrate(26, 0.5, 3.2);
    sensor1.writeRegister(INA219::REGISTERS::CALIBRATION, 4096);
    //  cout << value1 <<  endl;
    //  cout << value2 <<  endl;
    //  cout << value3 <<  endl;
    //  cout << value4 <<  endl;
     cout << sensor1.busVoltage() <<  endl;
     cout << sensor1.current() <<  endl;
}

void Test::testADS1015(){
    ADS1015 adc1(2, ADC_ADDRESS1);
    ADS1015 adc2(2, ADC_ADDRESS2);

    // for (int i = 0; i < 10; i++)
    // {
    //      cout << (int)adc1.getVoltage(0) << ", " << (int)adc1.getVoltage(1) <<  endl;
    //     usleep(500000);
    // }

    /*
    adc1:
        0: +x
        1: +y
        2: +z
    adc2:
        0: -x
        1: -y
        2: -z
    */

    float voltages[6];
    for (int i = 0; i < 3; i++)
    {
        voltages[i] = adc1.getVoltage(i);
        voltages[i + 3] = adc2.getVoltage(i);
    }

    for (int i = 0; i < 6; i++)
    {
         cout << "Photodiode " << i << " - " << voltages[i] <<  endl;
    }
}

void Test::testAS7263()
{
    AS7263 sensor(2, AS7263_ADDRESS);
    sensor.Test();
    for (int i = 0; i < 10; i++)
    {
        sensor.takeMeasurements();
         cout << "calibrated val: " << sensor.getCalibratedR() <<  endl;
         cout << "meas val: " << sensor.getR() <<  endl;
        usleep(500000);
    }
}

void Test::testPWM(string pwm_channel){
   // 
    PWM pwm(pwm_channel);  // P9_42 MUST be loaded as a slot before use
    pwm.setPeriod(3345678);         // Set the period in ns
    pwm.setDutyCycle(50.0f);       // Set the duty cycle as a percentage

    pwm.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
    pwm.run();                     // start the PWM output
     cout << "PWM active" <<  endl; 
    //pwm.stop();  // to discontinue the pwm signal 
    return; 
}

void Test::testPayload(){
    GPS gps;
    // AS7263 sensor(2, AS7263_ADDRESS);
    Payload payload(&gps);
    while (1){
        Payload::payload_data_t data = payload.getData();
        cout << "R: " << data.R << endl;
        cout << "S: " << data.S << endl;
        cout << "T: " << data.T << endl;
        cout << "U: " << data.U << endl;
        cout << "V: " << data.V << endl;
        cout << "W: " << data.W << endl;
        cout << "lat: " << data.lat << endl;
        cout << "long: " << data.lon << endl;
        cout << "alt: " << data.alt << endl;
    }
    
}


void Test::testGPS()
{

    GPS gps;
    gps.gps_on();
    for (int i = 0; i < 1000; i++)
    {
        gps.print_GPS();
        usleep(200);
    }
    gps.gps_off();
    return;
}

void Test::wodTest() {
    GPS gps;
    INA219 current_sensor(1, INA219_ADDRESS_BATT);
    WholeOrbit wod(&gps, 0, &current_sensor);

    while(1){
        WholeOrbit::wod_float_t data = wod.GetDataFloat();
        cout << "Time: " << (int)data.time << endl;
        cout << "Mode: " << data.mode << endl;
        cout << "Voltage batt: " << data.voltage_batt << endl;
        cout << "Current batt: " << data.current_batt << endl;
        cout << "Current 3v3: " << data.current_3v3 << endl;
        cout << "Current 5v: " << data.current_5v << endl;
        cout << "Temp comms: " << data.temp_comms << endl;
        cout << "Temp batt: " << data.temp_batt << endl;
        cout << "Temp eps: " << data.temp_eps << endl;
    }
    
    
}

void Test::wodTestEncoded() {
    GPS gps;
    INA219 current_sensor(1, INA219_ADDRESS_BATT);
    WholeOrbit wod(&gps, 0, &current_sensor);

    for (int i = 0; i < 10; ++i){
        WholeOrbit::wod_t data = wod.GetData();
        // cout << "Time: " << (int)data.time << endl;
        cout << "Mode: " << (int)data.mode << endl;
        cout << "Voltage batt: " << (int)data.voltage_batt << endl;
        cout << "Current batt: " << (int)data.current_batt << endl;
        cout << "Current 3v3: " << (int)data.current_3v3 << endl;
        cout << "Current 5v: " << (int)data.current_5v << endl;
        cout << "Temp comms: " << (int)data.temp_comms << endl;
        cout << "Temp batt: " << (int)data.temp_batt << endl;
        cout << "Temp eps: " << (int)data.temp_eps << endl;
    }
    
    
}

void Test::transmitWOD() {

    Satellite nice;
    while(1){
        nice.wodCollection();
        nice.wodTransmission();
        usleep(5000000);
    }
    

}

void Test::testPropulsion(){
    Satellite NICE; 
     vector<int> prop_vals{1, 3, 9, 5, 6, 2};
    NICE.propulsion(prop_vals);
}

void Test::testTransceiver(){
    Transceiver comms;

    comms.TestTransceiver();
    while(1){
         vector<uint8_t> message = {'T', 'e', 's', 't'};
        comms.TransmitMessage(message);
        usleep(1000000);
    }
     vector<uint8_t> message = {'T', 'e', 's', 't'};
    comms.TransmitMessage(message);
    usleep(1000000);

    while(1){
        comms.SendCommand(comms.CMD_RECEIVE_MODE_CONFIG);
         vector<uint8_t> message = comms.ReceiveData();
         cout << "MESSAGE: "; 

       for (unsigned int i = 0; i < comms.MAX_BYTES_AX25; i++)
        {
                if(message[0] == '#' && message[1] == 'R' && i>3 && i<18){
                     cout << message[i]; 

            }
        }
         cout <<  endl; 
        usleep(1000000);
    }
    
}

void Test::testBNO055()
{
    BNO055 bno(2, BNO055_ADDRESS_A);
     cout << "Orientation Sensor Raw Data Test" <<  endl;
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
         cout << "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!" <<  endl;
        while (1);
    }

    usleep(1000);

    /* System Status */
    int8_t status = bno.getSystemStatus();
     cout << "System status: " << (int)status <<  endl;

    /* Display the current temperature */
    int8_t temp = bno.getTemp();
     cout << "Current Temperature: " << (int)temp << " C" <<  endl;

     cout << "Calibration status values: 0=uncalibrated, 3=fully calibrated" <<  endl;

    bno.setExtCrystalUse(true);


    // SETUP: Initiate the Controller Object, and hardcode desired atittude
    imu::Vector<3> signal;
    CControl Controller(0, 1.57, 0);

    while(1)
    {

        // Display Quaternions
        imu::Quaternion quat = bno.getQuat();
         cout << "qW: " << quat.w() << " qX: " << quat.x() << " qY: " << quat.y() << " qZ: " << quat.z() << "\t\t";

	    /* Display Euler Angles deg */
        imu::Vector<3> euler = bno.getVector(BNO055::VECTOR_EULER);
	     cout << "X: " << euler.x() <<  " Y: " << euler.y() << " Z: "
	  	<< euler.z() << "\t\t";

        /* Display Angular Velocities rad/s */
        imu::Vector<3> rps = bno.getRPS();
         cout << "X: " << rps.x() <<  " Y: " << rps.y() << " Z: "
	  	<< rps.z() << "\t\t";

        /* Display Magnetometer Reading uT */
        imu::Vector<3> magfield = bno.getVector(BNO055::VECTOR_MAGNETOMETER);
         cout << "X: " << magfield.x() <<  " Y: " << magfield.y() << " Z: "
	  	<< magfield.z() << "\t\t";
 
        /* Display calibration status for each sensor. */
        uint8_t system, gyro, accel, mag = 0;
        bno.getCalibration(&system, &gyro, &accel, &mag);
         cout << "CALIBRATION: Sys=" << (int)system << " Gyro=" << (int)gyro
                  << " Accel=" << (int)accel << " Mag=" << (int)mag <<  endl;

        // signal = Controller.runControlAlgorithm(quat, rps);
        // if (Controller.getTolerance()) {
        //     break;
        // }
        // double out[3] = convertToCurrent(signal)
        // testPWM(PWM_0A);
        // testPWM(PWM_1A);
        // testPWM(PWM_2B);
        
        usleep(10000 * BNO055_SAMPLERATE_DELAY_MS);
    }
}

void Test::test24V(){
    INA219 sensor1(1, INA219_ADDRESS_24V);
    sensor1.configure(INA219::VOLTAGE_RANGE::FSR_32,
                      INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    uint16_t value1 = sensor1.readRegisters(INA219::REGISTERS::CONFIG);
    sensor1.writeRegister(INA219::REGISTERS::CALIBRATION, 4096);
     cout << sensor1.current() <<  endl;

}