import pickle
from transceiver.db.db import DB
from transceiver.ax25 import bindings
from transceiver.decode import decode_wod, decode_science

def run():
    db = DB()
    cursor = db.con.cursor()
    # cursor.execute("DELETE FROM wod;")
    # cursor.execute("DELETE FROM science;")
    
    print("Reading in encoded WOD packets...")
    with open('sample-wod.pickle', 'rb') as f:
        WOD = pickle.load(f)
        
    decoded_data = []
    successes = 0
    for packet in WOD:
        nbytes, _bytes = bindings.list_to_bytes(packet)
        decoded_msg = bindings.ax25._searchForMessage(_bytes, nbytes, 0)
        if decoded_msg == 0:
            # null ptr, no valid message
            continue
        nbytes = bindings.ax25.Message_getnpayload(decoded_msg)
        _bytes = bindings.ax25.Message_getpayload(decoded_msg)
        decoded_payload = [_bytes[i] for i in range(nbytes)]
        try:
            data = decode_wod(decoded_payload)
            decoded_data.append(data)
            successes += 1
        except AssertionError:
            continue
    
    print(f"Decoded {successes} WOD packets, inserting into DB")
    cursor.executemany("""
        INSERT OR REPLACE INTO wod (offsetTime, mode, batteryVoltage, batteryCurrent, `3V3Current`, `5VCurrent`, commTemperature, epsTemperature, batteryTemperature)
        VALUES (?,?,?,?,?,?,?,?,?)
    """, decoded_data)
    print(f"Insertion complete of {successes} WOD entries")
    cursor.close()
    db.con.commit()
    
    
    print("Reading in encoded science packets...")
    with open('sample-science.pickle', 'rb') as f:
        science = pickle.load(f)
        
    decoded_data = []
    successes = 0
    for packet in science:
        nbytes, _bytes = bindings.list_to_bytes(packet)
        decoded_msg = bindings.ax25._searchForMessage(_bytes, nbytes, 0)
        if decoded_msg == 0:
            # null ptr, no valid message
            continue
        nbytes = bindings.ax25.Message_getnpayload(decoded_msg)
        _bytes = bindings.ax25.Message_getpayload(decoded_msg)
        decoded_payload = [_bytes[i] for i in range(nbytes)]
        # try:
        data = decode_science(decoded_payload)
        decoded_data.append(data)
        successes += 1
        # except AssertionError:
        #     continue
    
    print(f"Decoded {successes} science packets, inserting into DB")
    cursor = db.con.cursor()
    cursor.executemany("""
        INSERT OR REPLACE INTO science (offsetTime, reading, latitude, longitude, altitude)
        VALUES (?,?,?,?,?)
    """, [d[0:2] + d[-3:] for d in decoded_data])
    print(f"Insertion complete of {successes} science entries")
    cursor.close()
    db.con.commit()
    