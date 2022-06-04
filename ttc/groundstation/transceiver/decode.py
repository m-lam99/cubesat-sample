from numpy import float16, array, frombuffer
from transceiver.ax25 import bindings


def parse_int(n, a, b):
    # converts int to float via given formula (n=va+b) where v is the desired value
    return (n-b)/a


def decode_wod(data):
    # Converts an integer array into WOD data

    assert len(data) == 12
    assert min(data) >= 0 and max(data) <= 255

    t = int.from_bytes(data[:4], byteorder='big', signed=True)
    assert t > 0
    assert t < 1000000000

    return [
        t,  # time offset
        data[4],  # mode
        parse_int(data[5], 20, -60),  # battery voltage
        parse_int(data[6], 127, 127),  # battery current
        parse_int(data[7], 40, 0),  # 3v3 current
        parse_int(data[8], 40, 0),  # 5v current
        parse_int(data[9], 4, 60),  # ttc temperature
        parse_int(data[10], 4, 60),  # eps temperature
        parse_int(data[11], 4, 60)  # battery temperature
    ]


def decode_science(data):
    # Converts an integer array into science data

    assert len(data) == 18 or len(data) == 22
    assert min(data) >= 0 and max(data) <= 255

    # Mock data has time, real data does not. add 0 for real data
    if len(data) == 18:
        data = [0,0,0,0] + data
        
    t = int.from_bytes(data[:4], byteorder='big', signed=True)
    assert t >= 0 and t < 10000000000

    channel_readings = []
    for i in range(6):
        channel_readings.append(int.from_bytes(data[4+2*i:6+2*i], byteorder='big', signed=True))
    lat = parse_int(int.from_bytes(data[16:18], byteorder='big', signed=True),32767/90,0)
    lon = parse_int(int.from_bytes(data[18:20], byteorder='big', signed=True),32767/180,0)
    alt = int.from_bytes(data[20:22], byteorder='big', signed=True)*50000/32767+250000

    return channel_readings + [
        lat,
        lon,
        alt
    ]
