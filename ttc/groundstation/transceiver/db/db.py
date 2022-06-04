import sqlite3
import datetime
import pandas as pd

DB_HOST = "transceiver/db/ground-station.db"

EPOCH_TIME = datetime.datetime(2000, 1, 1)


class DB:
    def __init__(self):
        self.con = sqlite3.connect(DB_HOST, check_same_thread=False)
        self.initialise()

    def initialise(self):
        cursor = self.con.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS wod (
                offsetTime INTEGER PRIMARY KEY,
                mode INTEGER,
                batteryVoltage REAL,
                batteryCurrent REAL,
                `3V3Current` REAL,
                `5VCurrent` REAL,
                commTemperature REAL,
                epsTemperature REAL,
                batteryTemperature REAL
            )""")

        cursor.execute("""
            CREATE TABLE IF NOT EXISTS science (
                offsetTime INTEGER PRIMARY KEY,
                latitude REAL,
                longitude REAL,
                altitude REAL,
                reading REAL
            )""")
        
        cursor.close()
        self.con.commit()

    def get_wod(self, col, start_time, end_time):
        cursor = self.con.cursor()
        # convert timestamps to offsets
        # hacky af i know but, as previously stated in numerous places, i dont care
        start = (datetime.datetime.strptime(start_time, '%Y-%m-%dT%H:%M:%S') -
                 EPOCH_TIME).total_seconds() if start_time is not None else 0
        end = (datetime.datetime.strptime(end_time, '%Y-%m-%dT%H:%M:%S') -
               EPOCH_TIME).total_seconds() if end_time is not None else 1000000000000000
        cursor.execute(f"""SELECT offsetTime, `{col}` FROM wod WHERE offsetTime BETWEEN :start AND :end;""", {
                       'start': start, 'end': end})
        data = cursor.fetchall()
        cursor.close()

        # convert offsets back to timestamps
        return [[EPOCH_TIME+datetime.timedelta(seconds=d[0]), d[1]] for d in data]

    def get_science(self, start_time, end_time):
        cursor = self.con.cursor()
        start = (datetime.datetime.strptime(start_time, '%Y-%m-%dT%H:%M:%S') -
                 EPOCH_TIME).total_seconds() if start_time is not None else 0
        end = (datetime.datetime.strptime(end_time, '%Y-%m-%dT%H:%M:%S') -
               EPOCH_TIME).total_seconds() if end_time is not None else 1000000000000000
        cursor.execute(
            f"SELECT offsetTime, latitude, longitude, altitude AS altitude, reading FROM science WHERE offsetTime BETWEEN :start AND :end;""", {'start': start, 'end': end})
        data = cursor.fetchall()
        cursor.close()

        # convert offsets back to timestamps
        return pd.DataFrame([[EPOCH_TIME+datetime.timedelta(seconds=d[0]), d[1], d[2], d[3], d[4]] for d in data], columns=['timestamp', 'latitude', 'longitude', 'altitude', 'sensor reading'])
