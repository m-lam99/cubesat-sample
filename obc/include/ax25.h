

namespace ax25 {

    struct Message {
        /*
        Params:
        payload: array of bytes in message
        npayload: number of bytes in payload
        source: max 6 chars
        destination: max 6 chars
        dataType: 0 -> WOD, 1 -> Science
        commandResponse: 0 -> response, 1 -> command
        controlType: 0 -> Information, 1 -> Supervisory, 2 -> Unnumbered
                        Supervisory is not implemented
        sendSequence: int between 0 and 7
                        Not applicable to Unnumbered control types
                        This counter must be stored and updated by the host application
                        Iterated upon successful transmission of Information frames
        receiveSequence: int between 0 and 7
                            Not applicable to Unnumbered control types
                            This counter must be stored and updated by the host application
                            Iterated upon successful retrieval of Information frames
        */
        unsigned char * source;
        unsigned char * destination;
        int dataType;
        int commandResponse;
        int controlType;
        int sendSequence;
        int receiveSequence;
        int npayload;
        unsigned char * payload;
    };

    struct ByteArray {
        int nbytes;
        unsigned char * bytes;
    };

    ByteArray * encode(Message * message);

    Message * searchForMessage(unsigned char * stream, int nstream, int receiveState);

}