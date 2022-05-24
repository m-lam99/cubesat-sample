from time import sleep
import serial
import ctypes
from ax25 import bindings

from transceiver import (
    setup_transceiver,
    test_transceiver,
    receive_data,
    transmit_message,
    send_command,
)


def ax_conversion(encoded_msg):
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
    main()
