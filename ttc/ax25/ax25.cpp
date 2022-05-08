// IMPLEMENTATION OF AX.25 ENCODING AND DECODING WITH HDLC WRAPPER

#include <iostream>
#include <string>
#include <cstring>

struct ByteArray {
    int nbytes;
    unsigned char* bytes;
};

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
    unsigned char* source;
    unsigned char* destination;
    int dataType;
    int commandResponse;
    int controlType;
    int sendSequence;
    int receiveSequence;
    int npayload;
    unsigned char* payload;
};

// Modulation factor for sequence
int SEQUENCE_MOD = 8;

// Lookup table for CRC checking
unsigned short CRC_TABLE[16*16] = {
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
        0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
        0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
        0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
        0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
        0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
        0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
        0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
        0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 
    };

/*
    UTILITY FUNCTIONS
*/

void printList(unsigned char* l, int n) {
    for (int i = 0; i < n; i++) 
        std::cout << l[i];
    std::cout << '\n';
}

void printListHex(unsigned char* l, int n) {
    for (int i = 0; i < n; i++) 
        std::cout << std::hex << int(l[i]) << ' ';
    std::cout << '\n';
}

unsigned char swapByteOrder(unsigned char b)
{
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

/*
    THE GUTS OF THE THING
*/

unsigned short crc16(unsigned char *payload, int npayload)
{
    int crc = 0xffff;
    for (int i = 0; i<npayload; i++) {
        int crcIndex = (payload[i]^(crc >> 8)) & 0xff;
        crc = CRC_TABLE[crcIndex] ^ (crc << 8);
    }
    crc = (crc ^ 0xffff) & 0x0ffff;
    return crc & 0xffff;
}

ByteArray* stuffBits(unsigned char* bytes, int nbytes) {
    // Performs bit stuffing on an encoded message
    // If there is ever a pattern "11111", a 0 is inserted
    // Also performs flips from MSB to LSB
    // Technically CRC should stay as MSB but I don't care enough

    int newBitCounter = 0;
    int newByteCounter = 0;
    unsigned char* newBytes = new unsigned char[2*nbytes](); // oversized array
    int consecutiveCounter = 0;
    int nMaxConsecutive = 5;

    // skip first byte (the only valid place for 6 1s in a row)
    newBytes[0] = bytes[0];
    newByteCounter++;

    for (int byteCounter = 1; byteCounter < nbytes; byteCounter++) {
        for (int bitCounter = 0; bitCounter < 8; bitCounter++) {
            int bit=(bytes[byteCounter]>>bitCounter)&1;
            if (bit == 1) {
                consecutiveCounter++;
                // Note: only need to set bits when 1
                newBytes[newByteCounter] |= (bit << newBitCounter);
            } else {
                consecutiveCounter = 0;
            }
            newBitCounter++;
            newBitCounter %= 8;
            if (newBitCounter == 0) {
                newBytes[newByteCounter] = swapByteOrder(newBytes[newByteCounter]);
                newByteCounter++;
            }
            if (consecutiveCounter == nMaxConsecutive) {
                newBitCounter++;
                newBitCounter %= 8;
                if (newBitCounter == 0) {
                newBytes[newByteCounter] = swapByteOrder(newBytes[newByteCounter]);
                    newByteCounter++;
                }
                consecutiveCounter = 0;
            }
        }
    }

    newBytes[newByteCounter] = swapByteOrder(newBytes[newByteCounter]);
    newByteCounter++;

    // Leave room for end bit
    unsigned char* newPayload = new unsigned char[newByteCounter+1];
    memcpy(newPayload, newBytes, newByteCounter);

    delete newBytes;

    ByteArray* r = new ByteArray{
        newByteCounter+1,
        newPayload
    };
    return r;
}

ByteArray* unstuffBits(ByteArray* message) {
    // Performs bit unstuffing on an encoded message
    // If there is ever a pattern "111110", the 0 is removed
    int nbytes = message->nbytes;
    unsigned char* bytes = message->bytes;

    int newBitCounter = 0;
    int newByteCounter = 0;
    unsigned char* newBytes = new unsigned char[nbytes](); // oversized array
    int consecutiveCounter = 0;
    int nMaxConsecutive = 5;

    for (int byteCounter = 0; byteCounter < nbytes; byteCounter++) {
        for (int bitCounter = 0; bitCounter < 8; bitCounter++) {
            int bit=(bytes[byteCounter]>>bitCounter)&1;
            if (consecutiveCounter == nMaxConsecutive) {
                if (bit == 1) {
                    // Impossible in a correctly-formatted packet
                    return NULL;
                }
                consecutiveCounter=0;
                continue;
            }

            if (bit == 1) {
                consecutiveCounter++;
                // Note: only need to set bits when 1
                newBytes[newByteCounter] |= (bit << newBitCounter);
            } else {
                consecutiveCounter = 0;
            }

            newBitCounter++;
            newBitCounter %= 8;
            if (newBitCounter == 0) {
                newByteCounter++;
            }
        }
    }

    message->nbytes = newByteCounter;
    message->bytes = newBytes;
    return message;
}

ByteArray* encode(Message* message) {
    /*
        Note: current implementation does not check for max payload size of 255
        Per the CDR, all our messages should be far less than this limit
    */

    // first some constants
    unsigned char flag = 0x7e; // message initialiser flag
    unsigned char repeaterMask = 0b01100000;

    unsigned char* destaddr = message->destination;
    unsigned char* srcaddr = message->source;
    int dataType = message->dataType;
    int controlType = message->controlType;
    int commandResponse = message->commandResponse;
    int sendSequence = message->sendSequence;
    int receiveSequence = message->receiveSequence;
    int npayload = message->npayload;
    unsigned char* payload = message->payload;

    // build address header
    unsigned char address[14];
    for (int i=0; i<6; i++) {
        destaddr[i]<<=1;
        srcaddr[i]<<=1;
    }
    // put actual addresses in
    std::memcpy(&address[0],destaddr,6);
    std::memcpy(&address[7],srcaddr,6);

    // encode final byte
    // dataType: bits 4 to 7
    // 1110 -> WOD, 1111 -> Science
    unsigned char dataTypeMask = 0b00011100 + 2*dataType;
    // command/response: bit 1
    // 0 -> command, 1 -> response
    unsigned char destCommandResponseMask = 128*commandResponse;
    address[6] = dataTypeMask | destCommandResponseMask | repeaterMask;
    // flip command/response bit for src address
    unsigned char srcCommandResponseMask = 128*(1-commandResponse);
    unsigned char endOfAddressMask = 0b00000001;
    address[13] = srcCommandResponseMask | repeaterMask | endOfAddressMask;

    // build control field
    unsigned char control = 0b00000000;
    if (controlType == 0) {
        // send sequence: bits 2 to 4
        control |= (sendSequence << 1);
        // receive sequence: bits 6 to 8
        control |= (receiveSequence << 5);
    } else if (controlType == 2) {
        control = 0b00000011;
    }

    // PID: not implemented
    unsigned char pid = 0b00000000;


    // Build the message
    int nmsg = 1+14+2+npayload+2;
    unsigned char msg[nmsg];
    msg[0] = flag;
    std::memcpy(&msg[1],address,14);
    msg[15] = control;
    msg[16] = pid;
    std::memcpy(&msg[17],payload,npayload);

    // Build frame check sequence
    // CRC-CCITT implementation
    // NB: only check the AX.25 frame (hence the [1,n-2] window)
    unsigned short crcVal = crc16(msg+1, nmsg-3);
    unsigned char crc[2];
    crc[0] = crcVal >> 8;
    crc[1] = crcVal & 0xFF;
    std::memcpy(&msg[17+npayload],crc,2);

    // Stuff the bits
    ByteArray* r = stuffBits(msg, nmsg);

    // Add end flag
    r->bytes[r->nbytes-1] = flag;

    return r;
}

Message* decode(ByteArray* frame, int receiveState) {
    // Strip away start flags and revert to MSB
    for (int i = 1; i<frame->nbytes-1; i++) {
        frame->bytes[i-1] = swapByteOrder(frame->bytes[i]);
    }
    frame->nbytes-=2;

    // Unstuff bits
    frame = unstuffBits(frame);
    if (frame == NULL) {
        std::cout<<"Frame failed bit unstuffing verification\n";
        return NULL;
    }

    // Check CRC
    unsigned short crcCalced = crc16(frame->bytes, frame->nbytes-2);
    unsigned char crc[2];
    memcpy(crc,&frame->bytes[frame->nbytes-2],2);
    unsigned short crcGiven = (((short)crc[0]) << 8) | (0x00ff & crc[1]);
    if (crcGiven != crcCalced) {
        std::cout<<"Frame failed CRC checksum\n";
        return NULL;
    }

    unsigned char *destaddr = new unsigned char[6];
    unsigned char *srcaddr = new unsigned char[6];
    memcpy(destaddr,&frame->bytes[0],6);
    memcpy(srcaddr,&frame->bytes[7],6);
    for (int i=0; i<6; i++) {
        destaddr[i]>>=1;
        srcaddr[i]>>=1;
    }

    unsigned char destExtraByte = frame->bytes[6];
    int commandResponse = (destExtraByte & 0b10000000)>>7;
    int dataType = ((destExtraByte & 0b00011110)>>1) - 14;
    if (commandResponse > 1) {
        std::cout<<"Frame failed command/response check\n";
        return NULL;
    }
    if (dataType > 1) {
        std::cout<<"Frame failed dataType check (SSID)\n";
        return NULL;
    }

    unsigned char controlByte = frame->bytes[14];
    int controlType; 
    unsigned char iFrameCheck = controlByte & 0b00000001;
    int receiveSequence=-1;
    int sendSequence=-1;
    if (iFrameCheck==0) {
        // Information frame
        controlType=0;

        // Verify receive state
        receiveSequence = (controlByte & 0b11100000)>>5;
        if (receiveSequence != receiveState) {
            std::cout<<"[WARNING] Receive state does not match expected. Did we miss a packet??\n";
        }

        sendSequence = (controlByte & 0b00001110)>>1;
    } else {
        unsigned char uFrameCheck = (controlByte & 0b00000010) >> 1;
        if (uFrameCheck==1) {
            // Unnumbered frame
            controlType=2;
        }
    }

    int npayload = frame->nbytes-18;
    unsigned char* payload = new unsigned char[npayload];
    memcpy(payload,&frame->bytes[16],npayload);

    delete frame->bytes;

    Message* m = new Message{
        srcaddr,
        destaddr,
        dataType,
        commandResponse,
        controlType,
        sendSequence,
        receiveSequence,
        npayload,
        payload
    };

    return m;
}

// FUNCTIONS TO INTERFACE WITH TRANSCEIVER

int sendMessage(ByteArray* msg) {
    // returns 1 on success, 0 on failure
    // TODO: interface with transceiver module
    return 1;
}

Message* searchForMessage(unsigned char* stream, int nstream, int receiveState) {
    // We will need to implement something like this that is constantly listening
    // to radio transmissions. It finds the data between two `0x7e` start/end flags
    // In reality this will probably be a stream of data not just a long string
    // TODO: interface with transceiver module
    unsigned char flag = 0x7e;
    unsigned char window = 0b00000000; // moving byte of data throughout the stream
    int maxFrameLength = 1+14+2+1+255+4+1; // max length of payload + overhead
    unsigned char* frame = new unsigned char[maxFrameLength]();
    int startFlagFound = 0;
    int frameLength = 0;
    int bitCounter = 0;

    for (int i = 0; i<nstream; i++) {
        for (int b = 0; b<8; b++) {
            int bit=(stream[i]>>b)&1;
            window = ((window << 1) + bit) & 0xff;
            if (window == flag) {
                if (startFlagFound == 1) {
                    if (frameLength < 16) {
                        // Too small, we can reject instantly
                        startFlagFound = 0;
                        continue;
                    }

                    std::cout<<"Decoding candidate frame\n";

                    // This is the only successful exit case
                    // Add the end flag
                    frame[frameLength] = flag;
                    frameLength++;

                    ByteArray r = {
                        frameLength,
                        frame
                    };

                    // Decode the message to check its validity
                    Message* m = decode(&r, receiveState);
                    if (m == NULL) {
                        startFlagFound = 0;
                        continue;
                    }

                    std::cout<<"Successfully validated frame\n";
                    delete frame;
                    return m;
                } else {
                    startFlagFound = 1;
                    frame[frameLength] = flag;
                    frameLength++;
                    continue;
                }
            }

            if (startFlagFound) {
                if (bit == 1) {
                    frame[frameLength] |= (bit << bitCounter);
                }
                bitCounter++;
                bitCounter %= 8;
                if (bitCounter == 0) {
                    frameLength++;
                }
                if (frameLength > maxFrameLength) {
                    startFlagFound = 0;
                }
            }
        }
    }
    // No frame found
    delete frame;
    return NULL;
}

int main() {
    // This is a sample use case, showing how the interface with the functions
    // should be developed
    // NOTE: all messaging is done with unsigned char, representing 1 byte
    
    std::cout<<"Sending:\n";
    std::string benchmarkMsg = "The quick brown fox jumps over the lazy dog";
    std::cout << benchmarkMsg << '\n';

    // Note: will need to be flipped on OBC code
    unsigned char destaddr[6] = {'N','I','C','E',' ',' '};
    unsigned char srcaddr[6] = {'U','S','Y','D','G','S'};
    int sendState = 0;
    int receiveState = 0;

    // conv string to list of bytes
    int nmsg = benchmarkMsg.size();
    unsigned char msg[nmsg];
    for(int i = 0; i < nmsg; i++)
        msg[i] = (unsigned char)(benchmarkMsg[i]);

    Message sampleMessage = {
        srcaddr,
        destaddr,
        1,
        1,
        2,
        sendState,
        receiveState,
        nmsg,
        msg
    };

    // Do the messaging
    ByteArray* encodedMsg = encode(&sampleMessage);
    if (encodedMsg != NULL) {
        int success = sendMessage(encodedMsg);
        if (success == 1) {
            sendState++;
            sendState %= SEQUENCE_MOD;
        }
    }

    std::cout<<"\nEncoded:\n";
    printListHex(encodedMsg->bytes, encodedMsg->nbytes);

    // Simulate some noise
    std::cout<<"\nGenerating 1mio bits of random noise and searching for packets\n";
    unsigned char noisyMessage[1000000];
    // TODO: move to backend of message once decoding is validated
    memcpy(&noisyMessage[800000],encodedMsg->bytes,encodedMsg->nbytes);
    Message* retrievedMsg = searchForMessage(noisyMessage, 1000000, receiveState);
    if (retrievedMsg == NULL) {
        std::cout<<"Failed to find message in noise\n";
    } else {
        receiveState++;
        receiveState%=SEQUENCE_MOD;
    }

    std::cout<<"\nDecoded:\n";
    std::cout<<"Source: ";
    printList(retrievedMsg->source,6);
    std::cout<<"Destination: ";
    printList(retrievedMsg->destination,6);
    std::cout<<"Data Type (0=WOD, 1=Science): "<< int(retrievedMsg->dataType) << '\n';
    std::cout<<"Command(1)/Response(0) Type : "<< int(retrievedMsg->commandResponse) << '\n';
    std::cout<<"Control Type (0=Info, 2=Unnumbered): "<< int(retrievedMsg->controlType) << '\n';
    std::cout<<"Sequence Nos (Send, Receive): "<< int(retrievedMsg->sendSequence) << ',' << int(retrievedMsg->receiveSequence) << '\n';
    std::cout<<"PAYLOAD: ";
    printList(retrievedMsg->payload, retrievedMsg->npayload);

    delete retrievedMsg->payload;
    delete retrievedMsg->source;
    delete retrievedMsg->destination;
    delete retrievedMsg;
    delete encodedMsg->bytes;
    delete encodedMsg;

    return 0;
}

// Python bindings
extern "C" {
    ByteArray* ByteArray_new(
        unsigned char* bytes,
        int nbytes){
            return new ByteArray{
                nbytes,
                bytes
            };
        }

    Message* Message_new(
        unsigned char* source,
        unsigned char* destination,
        int dataType,
        int commandResponse,
        int controlType,
        int sendSequence,
        int receiveSequence,
        int npayload,
        unsigned char* payload
    ){ 
        return new Message{
            source,
            destination,
            dataType,
            commandResponse,
            controlType,
            sendSequence,
            receiveSequence,
            npayload,
            payload
        }; 
    }

    ByteArray* _encode(Message* message) { return encode(message); }
}