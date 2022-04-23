# import serial
# from time import sleep

from transceiver import setup_transceiver, test_transceiver


def main():

    setup_transceiver()
    test_transceiver()


if __name__ == "__main__":
    main()
