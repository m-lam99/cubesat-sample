import serial
from time import sleep

ser = serial.Serial("/dev/ttyS0", baudrate=19200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=5)
print("Sending Message")

if not ser.is_open:
    ser.open()

#ser.write([0x41,0x54,0x4d,0x01])
ser.write([0x41,0x54,0x3F])
print("Message Sent!")
received_data = ser.read()
if received_data:
    print("Message Received!")
else:
    print("Timeout!")
sleep(0.3)
data_left = ser.inWaiting()
received_data += ser.read(data_left)
print(received_data)
sleep(3)

print("Sending Message")
ser.write([0x41,0x54,0x3F])
print("Message Sent!")
received_data = ser.read()
if received_data:
    print("Message Received!")
else:
    print("Timeout!")
sleep(0.3)
data_left = ser.inWaiting()
print(data_left)
received_data += ser.read(data_left)
print(received_data)
sleep(3)
