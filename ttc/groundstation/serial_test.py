from time import sleep

from transceiver import (
    setup_transceiver,
    test_transceiver,
    receive_data,
    transmit_message,
)


def main():

    while True:
        setup_transceiver(True)
        if test_transceiver():
            message = "General Kenobi!".encode()
            test_message = [int(hex(x), 16) for x in message]
            print(receive_data(packet_size=15))
            transmit_message(test_message)
            sleep(5)

    # while True:
    #     send_command([0x41, 0x54, 0x56])
    #     print("Sent!")
    #     sleep(2)
    # while True:
    #     received_data: list = _ser.read()
    #     print(received_data)


if __name__ == "__main__":
    main()
