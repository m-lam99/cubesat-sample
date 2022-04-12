import serial
from time import sleep

CMD_OPERATING_MODE = [0x41, 0x54, 0x4D]
CMD_RECEIVE_MODE_CONFIG = [0x41, 0x54, 0x52]
CMD_TRANSMIT_MODE_CONFIG = [0x41, 0x54, 0x53]
CMD_SYNC_BYTES = [0x41, 0x54, 0x41]
CMD_HOST_BAUD_RATE = [0x41, 0x54, 0x48]
CMD_RF_BAUD_RATE = [0x41, 0x54, 0x42]
CMD_RF_OUTPUT_POWER = [0x41, 0x54, 0x50]
CMD_DATA_ERROR_CHECKING = [0x41, 0x54, 0x45]
CMD_ENABLE_RESPONSE = [0x41, 0x54, 0x43]
CMD_RSSI = [0x41, 0x54, 0x51]
CMD_CONFIGURATION_SETTINGS = [0x41, 0x54, 0x3F]
CMD_FIRMWARE_VERSION = [0x41, 0x54, 0x56]
CMD_RESET_DEFAULT = [0x41, 0x54, 0x44]

MODE_READY = 0x02
MODE_HOST_BAUD_19200 = 0x01
MODE_RF_BAUD_9600 = 0x02

DEFAULT_HOST_BAUD = 19200
TIMEOUT = 2
PORT = "/dev/ttyS0"
WRITE_WAIT_TIME = 0.05

ser = serial.Serial(
    PORT,
    baudrate=DEFAULT_HOST_BAUD,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    timeout=TIMEOUT,
)


def send_command(data: list):
    """Send a command to the transceiver

    Args:
        data: Command and data to be sent to the transceiver
    """
    try:
        ser.write(data)
        sleep(WRITE_WAIT_TIME)
    except RuntimeError:
        print(f"Data ({data}) did not send")


def setup_transceiver(error_checking: bool = False):
    """Setup the transceiver (ground station)

    Args:
        error_checking: Option to enable error checking
    """

    # Setting mode to send/receive
    send_command(CMD_OPERATING_MODE + MODE_READY)

    # Setting host baud rate
    send_command(CMD_HOST_BAUD_RATE + MODE_HOST_BAUD_19200)

    # Setting RF baud rate
    send_command(CMD_RF_BAUD_RATE + MODE_RF_BAUD_9600)

    if error_checking:
        send_command(CMD_DATA_ERROR_CHECKING + [0x01])
