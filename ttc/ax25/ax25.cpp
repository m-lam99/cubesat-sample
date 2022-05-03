#include <iostream>
#include <string>

struct ByteArray {
    int nbytes;
    unsigned char* bytes;
}

// Modulation factor for sequence
int SEQUENCE_MOD = 8;

std::string pad_right(std::string const& str, size_t s)
{
    if ( str.size() < s )
        return str + std::string(s-str.size(), ' ');
    else
        return str;
}

void printList(unsigned char* l, int n) {
    for (int i = 0; i < n; i++) 
        std::cout << l[i];
    std::cout << '\n';
}

void printListHex(unsigned char* l, int n) {
    for (int i = 0; i < n; i++) 
        std::cout << std::hex << l[i];
    std::cout << '\n';
}

ByteArray* encode(
    unsigned char* payload,
    int npayload,
    int dataType,
    int commandResponse,
    int controlType,
    int sendSequence,
    int receiveSequence) {
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
    int naddr            = 6; // max length of address 
    std::string destaddr = pad_right("NICE",naddr);
    std::string srcaddr  = pad_right("USYDGS",naddr);
    unsigned char repeaterMask = 0b01100000;

    // convert addresses to int
    int ndest = naddr;
    unsigned char dest[ndest];
    for(int i = 0; i < ndest; i++)
        dest[i] = (unsigned char)(destaddr[i]);
    int nsrc = naddr;
    unsigned char src[nsrc];
    for(int i = 0; i < nsrc; i++)
        src[i] = (unsigned char)(srcaddr[i]);

    // build address header
    unsigned char address[28];
    // put actual addresses in
    address[0] = *dest;
    address[7] = *src;
    // encode final byte
    // dataType: bits 4 to 7
    // 1110 -> WOD, 1111 -> Science
    unsigned char dataTypeMask = 0b00011100 + 2*dataType;
    // command/response: bit 1
    // 0 -> command, 1 -> response
    unsigned char destCommandResponseMask = 128*commandResponse;
    address[6] = dataTypeMask | destCommandResponseMask | repeaterMask;
    // flip command/response bit for src address
    unsigned char srcCommandResponseMask = 0b1000000 - 128*srcCommandResponse;
    unsigned char endOfAddressMask = 0b00000001;
    address[13] = dataTypeMask | srcCommandResponseMask | repeaterMask | endOfAddressMask;

    return ByteA
}

int main() {
    // NOTE: all messaging is done with unsigned char, representing 1 byte
    std::string benchmarkMsg = "The quick brown fox jumps over the lazy dog";
    std::cout << benchmarkMsg << '\n';

    // conv string to list of bytes
    int nmsg = benchmarkMsg.size();
    unsigned char msg[nmsg];
    for(int i = 0; i < nmsg; i++)
        msg[i] = (unsigned char)(benchmarkMsg[i]);

    // Do the messaging
    int sendState = 0;
    int receiveState = 0;
    ByteArray* encodedMsg = encode(msg, nmsg, 1, 1, 0, sendState, receiveState);
    // int success = sendMessage(encodedMsg->bytes, encodedMsg->nbytes);

    printList(msg, nmsg);
    printListHex(msg, nmsg);
    return 0;
}