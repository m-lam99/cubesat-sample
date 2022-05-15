import sys
from transceiver import test_data_pipeline
from gui import gui

HELP_MSG = """
Usage: python3 main.py [live|demo|import]
                        live: listen to transceiver
                        demo: use sample/stored data
                        import: test data import feature
"""

if __name__ == '__main__':
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
    elif sys.argv[1] == "demo":
        gui.run()
    else:
        print(HELP_MSG)
        sys.exit(1)
