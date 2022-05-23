from numpy import float16, array, frombuffer
from transceiver.ax25 import bindings


def parse_int(n, a, b):
    # converts int to float via given formula (n=va+b) where v is the desired value
    return (n-b)/a


def decode_wod(msg):
    # Converts a ctype binary array into WOD data

    # Convert ctype binary array into integer array
    assert msg != 0  # null ptr check

    nbytes = bindings.ax25.Message_getnpayload(msg)
    _bytes = bindings.ax25.Message_getpayload(msg)
    bindings.ax25.Message_del(msg)

    data = [_bytes[i] for i in range(nbytes)]

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


def decode_science(msg):
    # Converts a ctype binary array into science data

    # Convert ctype binary array into integer array
    assert msg != 0  # null ptr check

    nbytes = bindings.ax25.Message_getnpayload(msg)
    _bytes = bindings.ax25.Message_getpayload(msg)
    bindings.ax25.Message_del(msg)

    data = [_bytes[i] for i in range(nbytes)]

    assert len(data) == 12
    assert min(data) >= 0 and max(data) <= 255

    t = int.from_bytes(data[:4], byteorder='big', signed=True)
    assert t > 0

    reading = float(frombuffer(bytes(data[4:6]), dtype=float16))
    lat = float(frombuffer(bytes(data[6:8]), dtype=float16))
    lon = float(frombuffer(bytes(data[8:10]), dtype=float16))
    alt = float(frombuffer(bytes(data[10:12]), dtype=float16))

    assert reading >= -1 and reading <= 1
    assert lat >= -90 and lat <= 90
    assert lon >= -180 and lon <= 180

    return [
        t,
        lat,
        lon,
        alt,
        reading
    ]
