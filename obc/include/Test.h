#ifndef TEST_H
#define TEST_H
#include <string>

class Test{
    private:
    public:
        void runTests();
        void testGPIO();                        // "gpio"
        void testINA219();                      // "current"
        void testADS1015();                     // "adc"
        void testAS7263();                      // "pay"
        void testPWM();  // "pwm"
        void testBNO055();                      // "imu"
        void testGPS();                         // "gps"
        void wodTest();                         // "wod"
        void wodTestEncoded();                  // "wodEn"
        void testPropulsion();                  // "prop"
        void testTransceiver();                 // "ttc"
        void test24V();

        void testDeployment();                  // "burn"
        void testDetumble();                        // "aocs"
        void testPoint();
        void transmitWOD();
        void testPayload();

        enum TEST_FUNCTIONS
        {
            T_EXIT = 0x60, 

            T_GPIO = 0x11,
            T_CURRENT = 0x12, 
            T_ADC = 0x13, 
            T_IR = 0x14,
            T_PWM = 0x15, 
            T_IMU = 0x16, 
            T_WOD = 0x17, 
            T_WOD_ENCODE = 0x18, 
            T_PROP = 0x19, 
            T_TTC = 0x20, 
            T_24V = 0x21, 
            T_BURN = 0x22, 
            T_DETUM = 0x23, 
            T_POINT = 0x24,
            T_WOD_TRANSMIT = 0x25,
            T_PAYLOAD = 0x26
        };

};

#endif