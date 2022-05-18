# RPi_BNO055
BNO055 IMU Library for Raspberry Pi Zero based on the Adafruit Python libraries.

1. Connect Raspberry Pi to BNO055 via i2c (careful of clock stretching for your device)
2. Run `make` to compile and `sudo ./run`. If pigpio error comes up, `sudo killall pigpiod`.
3. Calibrate IMU by moving it around.

