import sys
from transceiver import test_data_pipeline, transceiver
from gui import gui

HELP_MSG = """
Usage: python3 main.py [live|demo|import]
                        live: listen to transceiver and launch GUI
                        demo: launch GUI
                        import: test data import feature
                        receive: listen to transceiver and store in DB
"""

if __name__ == '__main__':
    print("""
        IMPORTANT: FOR THIS TO WORK
        You must have compiled the ax25 C++ layer (go to `transceiver/ax25` and run `make`)
        You must run this with the same processing bits as you compiled ax25 with (32bit vs 64bit)
                I (Adam) use 32bit Python because my C++ compiler was being a bitch
          """)
    
    if len(sys.argv) < 2:
        print(HELP_MSG)
        sys.exit(1)

    if sys.argv[1] == "import":
        print("Running data import test")
        test_data_pipeline.run()
    elif sys.argv[1] == "live":
        # TODO: launch transceiver listening stuff on a different thread
        raise NotImplementedError("live integration not setup yet")
        gui.run()
    elif sys.argv[1] == "receive":
        transceiver.run_receive_loop()
    elif sys.argv[1] == "demo":
        gui.run()
    else:
        print(HELP_MSG)
        sys.exit(1)
