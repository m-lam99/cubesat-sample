import ctypes
ax25 = ctypes.cdll.LoadLibrary('./ax25.so')

class ByteArray:
    def __init__(self, ls):
        # ls: a list of ints, 0-255
        sz = len(ls)
        array = (ctypes.c_ubyte * sz)()
        for i,n in enumerate(ls):
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
        sz = len(payload)
        c_payload = (ctypes.c_ubyte * sz)()
        for i,n in enumerate(payload):
            assert type(n) == int
            assert n >= 0
            assert n <= 255
            c_payload[i] = n
            
        c_src = (ctypes.c_ubyte * 6)()
        for i,n in enumerate(source.ljust(6, ' ')):
            c_src[i] = ord(n)
        c_dest = (ctypes.c_ubyte * 6)()
        for i,n in enumerate(destination.ljust(6, ' ')):
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
    
    
m = Message(
    [ord(c) for c in "The quick brown fox jumps over the lazy dog"],
    "NICE",
    "USYDGS",
    1,
    1,
    2,
    0,
    0
)

ax25._encode(m)