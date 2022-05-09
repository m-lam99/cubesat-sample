import math
import random
from numpy import float16, array, frombuffer
import bindings
import pickle

# FORMAT:
# time - 4 bytes, int
# science - 2 bytes, float16
# lat - 2 bytes, float16
# long - 2 bytes, float16
# alt - 2 bytes, float16
# read float16 by np.frombuffer(reading, dtype=float16)

times = range(0, 24*60*60, 2) # 1 day, every 2 secs

current_lat = -60
current_long = 30
current_alt = 250000

ref_t = 705438821

msgs = []
samples_per_orbit = 90*60/2
send_seq = 0
for t in times:
    packet = []
    time = bytearray(int(t + ref_t).to_bytes(4,byteorder='big',signed=True))
    for b in time:
        packet.append(b)
    math.sin(t/samples_per_orbit), current_lat, current_long, current_alt

    reading = array(float16(math.sin(t/samples_per_orbit))).tobytes()
    for b in reading:
        packet.append(b)
        
    lat = array(float16(current_lat)).tobytes()
    for b in lat:
        packet.append(b)
        
    lon = array(float16(current_long)).tobytes()
    for b in lon:
        packet.append(b)
        
    alt = array(float16(current_alt-250000)).tobytes()
    for b in alt:
        packet.append(b)

    msg = bindings.Message(
        packet,
        "NICE",
        "USYDGS",
        0,
        0,
        0,
        send_seq,
        0
    )
    
    b = bindings.ax25._encode(msg.obj)
    nbytes = bindings.ax25.ByteArray_getnbytes(b)
    _bytes = bindings.ax25.ByteArray_getbytes(b)
    encoded_msg = [_bytes[i] for i in range(nbytes)]
    
    send_seq+=1
    send_seq%=8
    msgs.append(encoded_msg)
            
    current_lat += 90/samples_per_orbit*2
    if current_lat > 90:
        current_lat-=180
    current_long += 180/samples_per_orbit*2
    if current_long > 180:
        current_long-=360
    current_alt+=(random.randint(-250000,250000)/10000)

with open("sample-science.pickle", "wb") as p:
    pickle.dump(msgs, p)
    