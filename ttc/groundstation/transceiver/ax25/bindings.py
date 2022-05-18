import ctypes

ax25 = ctypes.cdll.LoadLibrary('./transceiver/ax25/ax25.so')
# N.B: use the below if running standalone
# ax25 = ctypes.cdll.LoadLibrary('./ax25.so')

ax25.ByteArray_getbytes.restype = ctypes.POINTER(ctypes.c_ubyte)
ax25.Message_getpayload.restype = ctypes.POINTER(ctypes.c_ubyte)


class ByteArray:
    def __init__(self, ls):
        # ls: a list of ints, 0-255
        sz = len(ls)
        array = (ctypes.c_ubyte * sz)()
        for i, n in enumerate(ls):
            assert type(n) == int
            assert n >= 0
            assert n <= 255
            array[i] = n
        self.obj = ax25.ByteArray_new(ctypes.c_int(sz), array)


class Message:
    def __init__(self, payload, source, destination, data_type, command_response, control_type, send_sequence, receive_sequence):
        """ 
        Params:
        payload: list of ints, 0-255
        source: string, max 6 chars
        destination: string, max 6 chars
        dataType: 0 -> WOD, 1 -> Science
        commandResponse: 0 -> response, 1 -> command
        controlType: 0 -> Information, 1 -> Supervisory, 2 -> Unnumbered
                        Supervisory is not implemented
        sendSequence: int between 0 and 7
                        Not applicable to Unnumbered control types
                        This counter must be stored and updated by the host application
                        Iterated upon successful transmission of Information frames
        receiveSequence: int between 0 and 7
                            Not applicable to Unnumbered control types
                            This counter must be stored and updated by the host application
                            Iterated upon successful retrieval of Information frames  
        """
        assert len(source) <= 6
        assert len(destination) <= 6

        sz, c_payload = list_to_bytes(payload)
        # if sz > 16:
        #     print(
        #         "[WARNING] python bindings do not support payloads >16 bytes. Truncating to 16 :)")
        #     sz = 16
        #     payload = payload[:16]

        c_src = (ctypes.c_ubyte * 6)()
        for i, n in enumerate(source.ljust(6, ' ')):
            c_src[i] = ord(n)
        c_dest = (ctypes.c_ubyte * 6)()
        for i, n in enumerate(destination.ljust(6, ' ')):
            c_dest[i] = ord(n)
            
        self.obj = ax25.Message_new(
            c_src,
            c_dest,
            ctypes.c_int(data_type),
            ctypes.c_int(command_response),
            ctypes.c_int(control_type),
            ctypes.c_int(send_sequence),
            ctypes.c_int(receive_sequence),
            ctypes.c_int(sz),
            c_payload
        )


def list_to_bytes(payload):
    # Converts list of ints to cpp bytes
    sz = len(payload)

    c_payload = (ctypes.c_ubyte * sz)()
    for i, n in enumerate(payload):
        assert type(n) == int
        assert n >= 0
        assert n <= 255
        c_payload[i] = n

    return sz, c_payload


if __name__ == '__main__':
    # sample usage
    payload = "The quick brown fox jumped over the lazy dog"

    m = Message(
        [ord(m) for m in payload],
        "USYDGS",
        "NICE",
        1,
        1,
        2,
        0,
        0
    )
    b = ax25._encode(m.obj)
    nbytes = ax25.ByteArray_getnbytes(b)
    _bytes = ax25.ByteArray_getbytes(b)
    encoded_msg = [_bytes[i] for i in range(nbytes)]
    print([hex(m) for m in encoded_msg])

    decoded_msg = ax25._searchForMessage(_bytes, nbytes, 0)
    nbytes = ax25.Message_getnpayload(decoded_msg)
    _bytes = ax25.Message_getpayload(decoded_msg)
    decoded_payload = [_bytes[i] for i in range(nbytes)]
    print([chr(m) for m in decoded_payload])

    ax25.ByteArray_del(b)
    ax25.Message_del(m.obj)
    ax25.Message_del(decoded_msg)
