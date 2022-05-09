import sys
import transceiver

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
        transceiver.test_data_pipeline.run()
    elif sys.argv[1] == "live":
        # TODO: launch transceiver listening stuff on a different thread
        raise NotImplementedError("live integration not setup yet")
    elif sys.argv[1] == "demo":
        pass
    else:
        print(HELP_MSG)
        sys.exit(1)