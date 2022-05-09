import math
import random
from matplotlib import pyplot as plt
import pickle
import bindings

# FORMAT:
# time: 4 bytes, int
# mode: 1 byte, int
# ...: 1 byte, int
# ...: 1 byte, int
# ...: 1 byte, int
# ...: 1 byte, int
# ...: 1 byte, int
# ...: 1 byte, int
# ...: 1 byte, int

times = range(0,24*60*60, 30)

functions = [
    (lambda x,y: int((math.sin(x/3000)+1)/2*255/y)),
    (lambda x,y: int((math.cos(x/3000)+1)/2*255/y)),
    (lambda x,y: int(1/(1+(x/30000)**2)*255/y)),
    (lambda x,y: int((1/(1+math.e**(x/30000))+0.5)*255/y)),
    (lambda x,y: int(1/math.atan(x/3000+2)*255/y))
] # random functions lol

data = [[]]

# mode (1 bit)
for t in times:
    if t < 24*30*30:
        data[0].append(0)
    else :
        data[0].append(128)

for _ in range(7): # 7 1-byte 
    fn = random.choice(functions)
    scale = random.randint(100,300)/100
    offset = random.randint(-500,500)
    datarow = []
    for t in times:
        datarow.append(max(min(fn(t+offset,scale) + random.randint(-10,10),255),0))
    data.append(datarow)

for d in data:
    plt.plot(d)

ref_t = 705438821
msgs = []
send_seq = 0
for i,t in enumerate(times):
    # build sample packet
    # time (seconds since epoch)
    packet = []
    time = bytearray(int(t + ref_t).to_bytes(4,byteorder='big',signed=True))
    for b in time:
        packet.append(b)
    for d in range(8):
        packet.append(data[d][i])
    
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
    
with open("sample-wod.pickle", "wb") as p:
    pickle.dump(msgs, p)
    