import ctypes
from random import randint

ax25 = ctypes.cdll.LoadLibrary(
    # "/home/pi/Documents/AERO4701-NICE/ttc/groundstation/transceiver/ax25/ax25.so"
    r"C:\Users\adamh\Documents\UNI\Y5S1\AERO4701\AERO4701-NICE\ttc\groundstation\transceiver\ax25\ax25.so"
)
# N.B: use the below if running standalone
# ax25 = ctypes.cdll.LoadLibrary('./ax25.so')

ax25.ByteArray_getbytes.restype = ctypes.POINTER(ctypes.c_ubyte)
ax25.Message_getpayload.restype = ctypes.POINTER(ctypes.c_ubyte)
ax25.Message_getsource.restype = ctypes.POINTER(ctypes.c_ubyte)
ax25.Message_getdestination.restype = ctypes.POINTER(ctypes.c_ubyte)


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
    def __init__(
        self,
        payload,
        source,
        destination,
        data_type,
        command_response,
        control_type,
        send_sequence,
        receive_sequence,
    ):
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

        global c_src
        global c_dest
        sz, c_payload = list_to_bytes(payload)
        c_src = (ctypes.c_ubyte * 6)()
        for i, n in enumerate(source.ljust(6, " ")):
            c_src[i] = ord(n)
        c_dest = (ctypes.c_ubyte * 6)()
        for i, n in enumerate(destination.ljust(6, " ")):
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
            c_payload,
        )


def list_to_bytes(payload):
    # Converts list of ints to cpp bytes
    sz = len(payload)

    # Make sure python doesn't clean it up - somehow only a problem on the RPI
    global c_payload
    c_payload = (ctypes.c_ubyte * sz)()
    for i, n in enumerate(payload):
        assert type(n) == int
        assert n >= 0
        assert n <= 255
        c_payload[i] = n

    return sz, c_payload


def test_bindings():
    # sample usage
    payloads = [
        "The quick brown fox jumped over the lazy dog",  # text
        [0x53, 0x4F, 0x53, 0x2D, 0x53, 0x4F, 0x53],  # command
        [0x40, 0x3F, 0x69, 0x49, 0x12, 0xAE, 0xE9],  # random data, science
        [0x3D, 0x95, 0x21, 0xE3, 0x0F, 0xF1, 0x54],  # random data, WOD
    ]

    msgtypes = [  # (datatype, commandresponse, controltype)
        (0, 1, 2),
        (0, 1, 2),
        (1, 0, 0),
        (0, 0, 0),
    ]

    send_seq = 0

    for i, payload in enumerate(payloads):

        data_type, command_response, control_type = msgtypes[i]
        _payload = payload
        if type(payload[0]) == str:
            payload = [ord(p) for p in payload]
        print(
            f"""
--- TEST {i} ---
Preparing to send: 
    {_payload}
    (hex: {' '.join([hex(m) for m in payload])})
msg type {'command' if command_response else 'response'}, data type {'N/A' if command_response else ('Science' if data_type else 'WOD')}
              """
        )

        m = Message(
            payload,
            "USYDGS" if command_response == 1 else "NICE",
            "NICE" if command_response == 1 else "USYDGS",
            data_type,
            command_response,
            control_type,
            send_seq,
            0,
        )

        print("Encoding message...")
        b = ax25._encode(m.obj)
        nbytes = ax25.ByteArray_getnbytes(b)
        _bytes = ax25.ByteArray_getbytes(b)
        encoded_msg = [_bytes[i] for i in range(nbytes)]
        print(f"Encoded message: {' '.join([hex(m) for m in encoded_msg])}")

        print("Placing encoded message inside 1mio bytes of random noise...")
        cnoise = (ctypes.c_ubyte * 1000000)()
        for j in range(1000000):
            if j > 800000 and j < (800000 + nbytes):
                cnoise[j] = encoded_msg[j - 800000]
            else:
                cnoise[j] = randint(0, 255)
        decoded_msg = ax25._searchForMessage(_bytes, nbytes, 0)
        nbytes = ax25.Message_getnpayload(decoded_msg)
        _bytes = ax25.Message_getpayload(decoded_msg)
        decoded_payload = [_bytes[i] for i in range(nbytes)]
        src = ax25.Message_getsource(decoded_msg)
        dest = ax25.Message_getdestination(decoded_msg)
        comresp = ax25.Message_getcommandresponse(decoded_msg)
        control = ax25.Message_getcontroltype(decoded_msg)
        dtype = ax25.Message_getdatatype(decoded_msg)
        sseq = ax25.Message_getsendsequence(decoded_msg)
        src = [src[i] for i in range(6)]
        dest = [dest[i] for i in range(6)]
        print(
            f"""
Message found successfully!
    Decoded Payload: {('' if i==0 else ' ').join([chr(m) if i==0 else hex(m) for m in decoded_payload])}
    Source:          {''.join([chr(m) for m in src])}
    Destination:     {''.join([chr(m) for m in dest])}
    Message Type:    {comresp} (0=response, 1=command)
    Control Type:    {control} (0=info I, 2=unnumbered U)
    Data Type:       {dtype} (0=WOD, 1=Science. doesn't apply to command msg types)
    Send Sequence:   {sseq} (-1=unnumbered)
              """
        )

        assert decoded_payload == payload

        print("## TEST PASSED ##")
        print()

        if control_type == 0:
            send_seq += 1


if __name__ == "__main__":
    test_bindings()
