from time import sleep
import serial

from transceiver import (
    setup_transceiver,
    test_transceiver,
    receive_data,
    transmit_message,
)

def main():
    i = 0
    while True:
        setup_transceiver(True)
        # test_transceiver()
        message = "Hello there,".encode()
        test_message = [int(hex(x), 16) for x in message]
        # received_data = receive_data(packet_size=15)
        # hash_confirm=False
        # r_confirm=False
        # expected_message_length = 18
        # message_length = 0
        # filtered_message = ""
        # for character in received_data:
        #     if character == 35:
        #         hash_confirm = True
        #     if character == 82 and hash_confirm==True:
        #         r_confirm = True
        #     if hash_confirm == True and r_confirm==True and message_length<expected_message_length:
        #         filtered_message += chr(character)
        #         message_length += 1
        # if filtered_message[3] == 'G':
        #     print(f"Received message: {filtered_message[3:]}")

        transmit_message(test_message)
        if i == 0:
            print(f"Sent message: {message}")

        sleep(5)

        i+=1

    # while True:
    #     send_command([0x41, 0x54, 0x56])
    #     print("Sent!")
    #     sleep(2)

    # while True:
    #     received_data: list = _ser.read()
    #     data_left = _ser.inWaiting()
    #     received_data += _ser.read(data_left)
    #     print(received_data)
    #     sleep(5)                                                                                 



if __name__ == "__main__":
    main()
