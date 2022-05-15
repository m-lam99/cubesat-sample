import pickle
from db.db import DB
from transceiver.ax25 import bindings
from transceiver.decode import decode_wod, decode_science

def run():
    db = DB()
    
    print("Reading in encoded WOD packets...")
    with open('transceiver/ax25/sample-wod.pickle', 'rb') as f:
        WOD = pickle.load(f)
        
    decoded_data = []
    successes = 0
    for packet in WOD:
        nbytes, _bytes = bindings.list_to_bytes(packet)
        decoded_msg = bindings.ax25._searchForMessage(_bytes, nbytes, 0)
        try:
            data = decode_wod(decoded_msg)
            decoded_data.append(data)
            successes += 1
        except AssertionError:
            continue
    
    print(f"Decoded {successes} WOD packets, inserting into DB")
    cursor = db.con.cursor()
    cursor.executemany("""
        INSERT OR REPLACE INTO wod (offsetTime, mode, batteryVoltage, batteryCurrent, `3V3Current`, `5VCurrent`, commTemperature, epsTemperature, batteryTemperature)
        VALUES (?,?,?,?,?,?,?,?,?)
    """, decoded_data)
    print(f"Insertion complete of {successes} WOD entries")
    cursor.close()
    db.con.commit()
    
    
    print("Reading in encoded science packets...")
    with open('transceiver/ax25/sample-science.pickle', 'rb') as f:
        science = pickle.load(f)
        
    decoded_data = []
    successes = 0
    for packet in science:
        nbytes, _bytes = bindings.list_to_bytes(packet)
        decoded_msg = bindings.ax25._searchForMessage(_bytes, nbytes, 0)
        try:
            data = decode_science(decoded_msg)
            decoded_data.append(data)
            successes += 1
        except AssertionError:
            continue
    
    print(f"Decoded {successes} science packets, inserting into DB")
    cursor = db.con.cursor()
    cursor.executemany("""
        INSERT OR REPLACE INTO science (offsetTime, latitude, longitude, altitude, reading)
        VALUES (?,?,?,?,?)
    """, decoded_data)
    print(f"Insertion complete of {successes} science entries")
    cursor.close()
    db.con.commit()
    