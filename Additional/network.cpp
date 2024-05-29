#include "d:/Cloud project/Additional/network.h"

bool Network::isSocketActive(SOCKET sock)
{
    char oneCharBuffer;
    int result = recv(sock, &oneCharBuffer, 1, MSG_PEEK);

    if (result == SOCKET_ERROR)
        return false;

    else if (result == 0) // Connection is closed
        return false;
    return true;
}

void Network::encrypt(std::string &data) const
{
    ;
}


void Network::decrypt(std::string &data) const
{
    ;
}

std::string Network::encode(const MainMsg &msg) const
{
    using std::string;
    

    string sizeStr = Local::toStringFillZeroes(msg.header.size, 9);

    string typeStr = Local::toStringFillZeroes((int)msg.header.type, 2);

    string success = msg.header.success ? "1" : "0";


    string str = sizeStr + typeStr + (string)(msg.header.auth) + success + msg.data;

    str.resize(msg.header.size + HEADER_STRING_SIZE);
    return str;
}


MessageHeader Network::decodeHeader(const std::string &buffer) const
{
    MessageHeader decodedHeader;


    int inputSize = stoi(buffer.substr(0, SIZE_LENGTH));

    Operation inputType = (Operation)((int)stoi(buffer.substr(SIZE_LENGTH, 2)));

    std::string auth = buffer.substr(SIZE_LENGTH + 2, 8);
    std::string success = buffer.substr(SIZE_LENGTH + 10, 1);

    decodedHeader.size = inputSize;
    strncpy_s(decodedHeader.auth, 9, auth.c_str(), strlen(auth.c_str()));
    decodedHeader.auth[8] = '\0';
    decodedHeader.type = inputType;
    decodedHeader.success = success == "1" ? true : false;
    return decodedHeader;
}

void Network::snd(SOCKET socket, const MainMsg &msg)
{
    int iSendResult = 0;
    std::string encodedMsg;

    // Transforming the struct MainMsg to a string.
    encodedMsg = encode(msg);
    //encrypt(encodedMsg);
    // Sending the data as char array (.data()).
    iSendResult = send(socket, encodedMsg.data(), encodedMsg.size(), 0);
    // If the number of bytes sent is not equal to the size of the 
    // encoded message cerr << EXCEPTION_CODES[an error.
    if (iSendResult != encodedMsg.size())
    {
        std::string msgErr;
        msgErr = "Did not send full message. Sent" + std::to_string(iSendResult)
            + "/" + std::to_string(encodedMsg.size()) + " bytes.";

        throw ContentError(msgErr.c_str(), MY_LOCATION);
    }
}


void Network::receive(SOCKET socket, MainMsg &msg)
{
    // Count number of bytes received
    uint64_t iResult = 0;

    msg = MainMsg();

    char *buffer = new char[HEADER_STRING_SIZE + 1];
    memset(buffer, 0, HEADER_STRING_SIZE + 1);

    // Receiving the header which has a constant size. Contains- size of data, type of action, authenticatoin.
    iResult = recv(socket, buffer, HEADER_STRING_SIZE, 0);
    if (iResult != HEADER_STRING_SIZE)
    {
        std::string errMsg = "";
        errMsg = std::format("Did not got full message. Received {}/{} bytes."
            , iResult, HEADER_STRING_SIZE);
        throw CommunicationError(errMsg.c_str(), MY_LOCATION);
    }

    buffer[HEADER_STRING_SIZE] = '\0';
    std::string decryptedBuffer(buffer, HEADER_STRING_SIZE);
    //decrypt(decryptedBuffer);
    MessageHeader decodedHeader;

    decodedHeader = decodeHeader(decryptedBuffer);

    delete[] buffer;
    buffer = new char[decodedHeader.size + 1];
    memset(buffer, 0, decodedHeader.size + 1);

    iResult = 0;

    // Receiving data in chunks of 4096 bytes (4 kb)
    // To avoid data loss.
    if (decodedHeader.size == 0)
        goto zero_data;
    for (int i = 0; i < decodedHeader.size / 4096; i++)
        iResult += recv(socket, &buffer[iResult], 4096, 0);
    // The last part of the data. (For example if the size is 4097, it will be 1).
    iResult += recv(socket, &buffer[iResult], decodedHeader.size - iResult, 0);
    // If number of bytes received is not equal the size specified in the decodedHeader
zero_data:
    if (iResult != decodedHeader.size)
    {
        std::string errMsg =
            "Did not got full message. Received" + std::to_string(iResult) +
            "/" + std::to_string(decodedHeader.size);
        throw CommunicationError(errMsg.c_str(), MY_LOCATION);
    }

    buffer[decodedHeader.size] = '\0';
    std::string data(buffer, decodedHeader.size);
    //decrypt(data);

    msg.data = data;
    msg.header = MessageHeader();
    msg.header = MessageHeader(decodedHeader);
    //cerr << data;

    //cerr << (string)data;
    //printf("\n%s", buffer);
    delete[] buffer;
}