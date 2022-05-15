import sqlite3

DB_HOST = "db/ground-station.db"


class DB:
    def __init__(self):
        self.con = sqlite3.connect(DB_HOST)
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
                `5V5Current` REAL,
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
