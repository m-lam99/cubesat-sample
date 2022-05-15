import sqlite3

DB_HOST = "ground-station.db"


class DB:
    def __init__(self):
        self.con = sqlite3.connect(DB_HOST)
        self.initialise()

    def initialise(self):
        with self.con.cursor() as cursor:
            cursor.execute(
                """
                CREATE TABLE IF NOT EXISTS wod
                (TODO)
                        """
            )
