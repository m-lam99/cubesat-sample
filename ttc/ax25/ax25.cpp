#include <iostream>
#include <string>
#include <cstring>

struct ByteArray {
    int nbytes;
    unsigned char* bytes;
};

// Modulation factor for sequence
int SEQUENCE_MOD = 8;

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

int sendMessage(ByteArray* msg) {
    // returns 1 on success, 0 on failure
    // TODO: interface with transceiver module
    return 1;
}

unsigned short crc16(unsigned char *payload, int npayload)
{
   int  crc;
   char i;
   crc = 0;
   while (--npayload >= 0)
   {
      crc = crc ^ (int) *payload++ << 8;
      i = 8;
      do
      {
         if (crc & 0x8000)
            crc = crc << 1 ^ 0x1021;
         else
            crc = crc << 1;
      } while(--npayload);
   }
   return (crc);
}

// 7E3949615102027DAAB2A688B8B2C30000150B530247574B636B0223277B773B02337B0F022B575B0767027B37532702170B53021B432F4F02137B733427BF00

ByteArray* stuffBits(unsigned char* bytes, int nbytes) {
    // Performs bit stuffing on an encoded message
    // If there is ever a pattern "11111", a 0 is inserted

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

    for (int byteCounter = 0; byteCounter < nbytes; byteCounter++) {
        for (int bitCounter = 0; bitCounter < 8; bitCounter++) {
            int bit=(bytes[byteCounter]>>bitCounter)&1;
            std::cout<<bit;
        }
        std::cout<<' ';
    }
    std::cout<<'\n';

    for (int byteCounter = 0; byteCounter < newByteCounter; byteCounter++) {
        for (int bitCounter = 0; bitCounter < 8; bitCounter++) {
            std::cout<<((newBytes[byteCounter]>>bitCounter)&1);
        }
        std::cout<<' ';
    }
    std::cout<<'\n';

    unsigned char* newPayload = new unsigned char[newByteCounter];
    memcpy(newPayload, newBytes, newByteCounter);

    ByteArray* r = new ByteArray{
        newByteCounter,
        newPayload
    };
    return r;
}

ByteArray* encode(
    unsigned char* payload,
    int npayload,
    int dataType,
    int commandResponse,
    int controlType,
    int sendSequence,
    int receiveSequence
    ) {
    /*
        Params:
        payload: array of bytes to send
        npayload: number of bytes in payload
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

        Returns: pointer to the encoded message. This will need to be freed after usage
    */

    // first some constants
    unsigned char flag = 0x7e; // message initialiser flag
    // Note: will need to be flipped on OBC code
    unsigned char destaddr[6] = {'N','I','C','E',' ',' '};
    unsigned char srcaddr[6] = {'U','S','Y','D','G','S'};
    unsigned char repeaterMask = 0b01100000;


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

    // Build frame check sequence
    // CRC-CCITT implementation
    unsigned char* crc = (unsigned char*)&(payload, npayload);

    // Build the message
    int nmsg = 1+14+2+npayload+2;
    unsigned char msg[nmsg];
    msg[0] = flag;
    std::memcpy(&msg[1],address,14);
    msg[15] = control;
    msg[16] = pid;
    std::memcpy(&msg[17],payload,npayload);
    std::memcpy(&msg[17+npayload],crc,2);

    ByteArray* r = stuffBits(msg, nmsg);

    return r;
}

int main() {
    // NOTE: all messaging is done with unsigned char, representing 1 byte
    std::cout<<"Sending:\n";
    std::string benchmarkMsg = "The quick brown fox jumps over the lazy dog";
    std::cout << benchmarkMsg << '\n';

    // conv string to list of bytes
    int nmsg = benchmarkMsg.size();
    unsigned char msg[nmsg];
    for(int i = 0; i < nmsg; i++)
        msg[i] = (unsigned char)(benchmarkMsg[i]);

    // printList(msg, nmsg);
    printListHex(msg, nmsg);

    // Do the messaging
    int sendState = 0;
    int receiveState = 0;
    ByteArray* encodedMsg = encode(msg, nmsg, 1, 1, 0, sendState, receiveState);
    if (encodedMsg != NULL) {
        int success = sendMessage(encodedMsg);
        if (success == 1) {
            sendState += 1;
            sendState %= SEQUENCE_MOD;   
        }
    }

    printListHex(encodedMsg->bytes, encodedMsg->nbytes);

    return 0;
}