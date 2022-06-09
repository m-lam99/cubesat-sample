from time import sleep
import serial
import sys
import ctypes
from ax25 import bindings
from decode import decode_wod, decode_science

from transceiver import (
    setup_transceiver,
    test_transceiver,
    receive_data,
    transmit_message,
    send_command,
)

MODES = [
    "Safe",
    "Idle",
    "Normal",
    "Station-keep",
    "Transmit",
    "End-of-life",
    "Ejection",
    "Orbital Insertion",
    "Deployment"
]

def ax_conversion(encoded_msg):
    # used for ad-hoc testing
    l = len(encoded_msg)
    cmsg = (ctypes.c_ubyte * l)()
    for j in range(l):
        cmsg[j] = encoded_msg[j]
    decoded_msg = bindings.ax25._searchForMessage(cmsg, l, 0)
    if decoded_msg == 0:
        print("failed decoding")
        return
    nbytes = bindings.ax25.Message_getnpayload(decoded_msg)
    _bytes = bindings.ax25.Message_getpayload(decoded_msg)
    decoded_payload = [_bytes[i] for i in range(nbytes)]
    src = bindings.ax25.Message_getsource(decoded_msg)
    dest = bindings.ax25.Message_getdestination(decoded_msg)
    comresp = bindings.ax25.Message_getcommandresponse(decoded_msg)
    control = bindings.ax25.Message_getcontroltype(decoded_msg)
    dtype = bindings.ax25.Message_getdatatype(decoded_msg)
    sseq = bindings.ax25.Message_getsendsequence(decoded_msg)
    src = [src[i] for i in range(6)]
    dest = [dest[i] for i in range(6)]
    print(
        f"""
    Message found successfully!
    Decoded Payload: {''.join([chr(m) for m in decoded_payload])}
    Source:          {''.join([chr(m) for m in src])}
    Destination:     {''.join([chr(m) for m in dest])}
    Message Type:    {comresp} (0=response, 1=command)
    Control Type:    {control} (0=info I, 2=unnumbered U)
    Data Type:       {dtype} (0=WOD, 1=Science. doesn't apply to command msg types)
    Send Sequence:   {sseq} (-1=unnumbered)
              """
    )

def demo_run_receive_loop():
    # Infinitely loops, searching for AX25 messages
    # Prints decoded AX25 messages and the parsed data contained within
    def find_start_token_index(msg):
        token = ["#","R"]
        for ind in (i for i,e in enumerate(msg) if e==token[0]):
            if msg[ind:ind+2]==token:
                return ind
        return -1

    while True:
        # sleep for a few seconds then try get 100 bytes
        sleep(3)
        data = receive_data(100)
        try:        
            ix = find_start_token_index(data)
            if ix == -1 :
                print("No start token found")
                continue
            data = data[ix+2:]
            
            b = bindings.ByteArray([int(d) for d in data])
            nbytes = bindings.ax25.ByteArray_getnbytes(b)
            _bytes = bindings.ax25.ByteArray_getbytes(b)
            decoded_msg = bindings.ax25._searchForMessage(_bytes, nbytes, 0)
            if decoded_msg == 0:
                print("Message corrupted")
                # null ptr, no valid message
                continue
            nbytes = bindings.ax25.Message_getnpayload(decoded_msg)
            _bytes = bindings.ax25.Message_getpayload(decoded_msg)
            decoded_payload = [_bytes[i] for i in range(nbytes)]
            src = bindings.ax25.Message_getsource(decoded_msg)
            dest = bindings.ax25.Message_getdestination(decoded_msg)
            comresp = bindings.ax25.Message_getcommandresponse(decoded_msg)
            control = bindings.ax25.Message_getcontroltype(decoded_msg)
            dtype = bindings.ax25.Message_getdatatype(decoded_msg)
            sseq = bindings.ax25.Message_getsendsequence(decoded_msg)
            src = [src[i] for i in range(6)]
            dest = [dest[i] for i in range(6)]
            print(
                f"""
Message found successfully! 
    Decoded Payload: {''.join([hex(m) for m in decoded_payload])}
    Source:          {''.join([chr(m) for m in src])}
    Destination:     {''.join([chr(m) for m in dest])}
    Message Type:    {comresp} (0=response, 1=command)
    Control Type:    {control} (0=info I, 2=unnumbered U)
    Data Type:       {dtype} (0=WOD, 1=Science. doesn't apply to command msg types)
    Send Sequence:   {sseq} (-1=unnumbered)
                    """
            )      

            # Decode data and insert
            if dtype == 0:
                try:
                    data = decode_wod(decoded_payload)
                    successes += 1
                except AssertionError:
                    continue

                print(f"""
Decoded WOD payload:
    Offset Time:         {data[0]}
    Mode:                {MODES[data[1]]}
    Battery Voltage:     {data[2]}
    Battery Current:     {data[3]}
    3v3 Current:         {data[4]}
    5v Current:          {data[5]}
    TTC Temperature:     {data[6]}
    EPS Temperature:     {data[7]}
    Battery Temperature: {data[8]}
                      """)
            else:
                try:
                    data = decode_science(decoded_payload)
                    successes += 1
                except AssertionError:
                    continue
                
                # strip out time (unsupported in real thing)
                data = data[4:]
                print(f"""
Decoded science payload:
    Channel readings:
        R:      {data[0]}
        S:      {data[1]}
        T:      {data[2]}
        U:      {data[3]}
        V:      {data[4]}
        W:      {data[5]}
    Latitude:   {data[6]}
    Longitude:  {data[7]}
    Altitude:   {data[8]}
                      """)

        except Exception as e:
            print(e.with_traceback)
            
def demo_run_raw_receive_loop():
    # Infinitely loops, searching for AX25 messages
    # Prints decoded AX25 messages and the parsed data contained within
    def find_start_token_index(msg):
        token = ["#","R"]
        for ind in (i for i,e in enumerate(msg) if e==token[0]):
            if msg[ind:ind+2]==token:
                return ind
        return -1

    while True:
        # sleep for a few seconds then try get 100 bytes
        sleep(3)
        data = receive_data(64)
        ix = find_start_token_index(data)
        if ix == -1 :
            print("No start token found")
            continue
        data = data[ix+2:]
        # Print any valid ascii chars
        print(f"Message: {''.join(chr(d) for d in data if 0x20<=d<=0x7e)}")

def send_mode_command(mode: int):
    """Encodes mode command and invokes message transmitter"""

    msg = [0x4D, 0x30 + mode]

    while True:
        transmit_message(msg)
        sleep(3)
        
        
def send_test_command(test: int):
    """Encodes mode command and invokes message transmitter"""

    msg = [0x4D, 0x30 + test]

    while True:
        transmit_message(msg)
        sleep(3)
        
        
def main():
    i = 0
    while True:
        setup_transceiver(True)
        # test_transceiver()
        message = "Hello there,".encode()
        test_message = [int(hex(x), 16) for x in message]
        received_data = receive_data(packet_size=15)
        hash_confirm = False
        r_confirm = False
        expected_message_length = 15
        message_length = 0
        filtered_message = []
        print(received_data)
        for character in received_data:
            if character == 35:
                hash_confirm = True
            if character == 82 and hash_confirm:
                r_confirm = True
            if hash_confirm and r_confirm and message_length < expected_message_length:
                filtered_message += [character]
                message_length += 1
        print(f"Received message: {filtered_message}. Attempting to decode")
        print([x for x in filtered_message])
        print(ax_conversion(filtered_message))

        # transmit_message(test_message)
        # print(f"Sent message: {message}")

        sleep(5)

        i += 1

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
    if len(sys.argv) == 0:
        main()

    elif sys.argv[1] == "receive":
        print("running AX.25 receive loop")
        demo_run_receive_loop()
        
    elif sys.argv[1] == "receive-raw":
        print("running string receive loop")
        demo_run_raw_receive_loop()
    
    elif sys.argv[1] == "SOS":
        print("sending SOS command")
        transmit_message([0x53, 0x4F, 0x53, 0x2D, 0x53, 0x4F, 0x53])
        
    else:
        if sys.argv[1] == "mode":
            modes = ["safe", "idle", "normal", "station-keep", "transmit", "end-of-life"]
            if sys.argv[2] not in modes :
                print("unrecognised command")
                sys.exit(1)
            print("sending mode change command ", sys.argv[2])
            send_mode_command(modes.index(sys.argv[2]))
        elif sys.argv[1] == "test":
            tests = {
                "exit": 0x60,
                "gpio": 0x11,
                "current": 0x12,
                "adc": 0x13,
                "ir": 0x14,
                "pwm": 0x15,
                "imu": 0x16,
                "wod": 0x17,
                "wod-encode": 0x18,
                "prop": 0x19,
                "ttc": 0x20,
                "24v": 0x21,
                "burn": 0x22,
                "detum": 0x23,
                "point": 0x24,
                "wod-transmit": 0x25,
                "payload": 0x26
            }
            if sys.argv[2] not in tests :
                print("unrecognised command")
                sys.exit(1)
            print("sending test command ", sys.argv[2])
            send_test_command(tests[sys.argv[2]])