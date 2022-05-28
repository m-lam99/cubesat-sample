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
        void testPWM(std::string pwm_channel);  // "pwm"
        void testBNO055(); 
        void testGPS();
        void wodTest();
        void testPropulsion();
        void testTransceiver();

};

#endif