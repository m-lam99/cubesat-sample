import serial
from time import sleep
from transceiver.ax25 import bindings

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

DEFAULT_HOST_BAUD = 19200  # bps
DEFAULT_RF_BAUD = 9600  # bps
TIMEOUT = 2  # s
PORT = "/dev/ttyS0"
WRITE_WAIT_TIME = 0.2  # s
RECEIVE_WAIT_TIME = 0.2  # s
FIRMWARE = b"#V1.01"

_ser = serial.Serial(
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
        _ser.write(data)
        sleep(WRITE_WAIT_TIME)
    except RuntimeError:
        print(f"Data ({data}) did not send")


def receive_data(packet_size: int, channel: int = 0) -> list:
    """Receive data from the transceiver

    Returns:
        Data from the transceiver
    """
    while True:
        send_command(CMD_RECEIVE_MODE_CONFIG + [channel] + [packet_size])
        received_data: list = _ser.read()
        sleep(RECEIVE_WAIT_TIME)
        if len(received_data) == 0:
            print("Timeout!")
        else:
            break

    data_left = _ser.inWaiting()
    received_data += _ser.read(data_left)
    return received_data


def setup_transceiver(error_checking: bool = False):
    """Setup the transceiver (ground station)

    Args:
        error_checking: Option to enable error checking
    """

    # Setting mode to send/receive
    send_command(CMD_OPERATING_MODE + [MODE_READY])

    # Setting host baud rate
    send_command(CMD_HOST_BAUD_RATE + [MODE_HOST_BAUD_19200])

    # Setting RF baud rate
    send_command(CMD_RF_BAUD_RATE + [MODE_RF_BAUD_9600])

    if error_checking:
        send_command(CMD_DATA_ERROR_CHECKING + [0x01])


def test_transceiver() -> bool:
    """Tests the transciever setup by checking the firmware verison

    Returns:
        Boolean to verify whether the transceiver is working or not
    """
    result = False

    send_command(CMD_FIRMWARE_VERSION)
    firmware_version = receive_data(6)
    print(firmware_version)

    if firmware_version == FIRMWARE:
        result = True

    return result


def transmit_message(data: list, channel: int = 0):
    """Transmit message using the transceiver

    Args:
        data: Data to be sent via the transceiver
    """

    if channel < 0 or channel > 15:
        ValueError(f"Invalid channel for transmission: {channel}")

    full_message = CMD_TRANSMIT_MODE_CONFIG + [channel] + [len(data)] + data

    send_command(full_message)

def send_mode_command(mode: int):
    """Encodes mode command and invokes message transmitter"""
    
    m = bindings.Message(
        [0x4D, 0x30+mode], # "M<mode>"
        "USYDGS",
        "NICE",
        0,
        1,
        2,
        -1,
        -1
    )
    
    b = bindings.ax25._encode(m.obj)
    nbytes = bindings.ax25.ByteArray_getnbytes(b)
    _bytes = bindings.ax25.ByteArray_getbytes(b)
    encoded_msg = [_bytes[i] for i in range(nbytes)]
    transmit_message(encoded_msg)