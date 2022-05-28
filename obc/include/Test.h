#ifndef TEST_H
#define TEST_H
#include <string>

class Test{
    private:
    public:
        void runTests();
        void testGPIO();
        void testINA219();
        void testADS1015();
        void testAS7263();
        void testPWM(std::string pwm_channel); 
        void testBNO055(); 
        void testGPS();
        void wodTest();
        void testPropulsion();
        void testTransceiver();

};

#endif