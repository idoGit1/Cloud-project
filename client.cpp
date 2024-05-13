#include "client.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

Client::Client()
{
    int result;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << EXCEPTION_CODES[9];
        exit(9);
    }


    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << EXCEPTION_CODES[10];
        exit(10);
    }
    /*int addrLen = sizeof(serverAddress);
    char *addrStr = new char[strlen(IP) + 1];
    strncpy(addrStr, IP, strlen(IP));
    addrStr[strlen(IP)] = '\0';*/

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(PORT);
    result = inet_pton(AF_INET, IP, &this->serverAddress.sin_addr.s_addr);

    if (result <= 0)
    {
        cerr << EXCEPTION_CODES[11];
        exit(11);
    }

    result = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result == INVALID_SOCKET)
    {
        cerr << EXCEPTION_CODES[12];
        exit(12);
    }
}


Client::~Client()
{
    WSACleanup();
    closesocket(clientSocket);
    printf("\n\n\n Client::~Client - Cleaned up successfully!");

}

void Client::snd(MainMsg &msg)
{
    // Counts the bytes sent.
    int iSendResult = 0;
    string encodedMsg;
    // Transforming the struct MainMsg to a string.
    encodedMsg = encode(msg);
    //encrypt(encodedMsg);
    // Sending the data as char array (.data()).
    iSendResult = send(clientSocket, encodedMsg.data(), encodedMsg.size(), 0);
    // If the number of bytes sent is not equal to the size of the 
    // encoded message cerr << EXCEPTION_CODES[an error.
    if (iSendResult != encodedMsg.size())
    {
        cerr << EXCEPTION_CODES[13];
        exit(13);
    }
}

void Client::receive(MainMsg &msg)
{
    // Count number of bytes received
    int iResult = 0;
    // Initiating all memory to 0
    cleanMsg(msg);

    char *buffer = new char[HEADER_STRING_SIZE + 1];
    memset(buffer, 0, HEADER_STRING_SIZE + 1);
    // Receiving the header which has a constant size. Contains- size of data, type of action, authenticatoin.
    iResult = recv(clientSocket, buffer, HEADER_STRING_SIZE, 0);
    if (iResult != HEADER_STRING_SIZE)
    {
        cerr << EXCEPTION_CODES[14];
        exit(14);
    }

    buffer[HEADER_STRING_SIZE] = '\0';
    string decryptedBuffer(buffer, HEADER_STRING_SIZE);
    //decrypt(decryptedBuffer);
    MessageHeader decodedHeader;

    decodedHeader = decodeHeader(decryptedBuffer);

    buffer = new char[decodedHeader.size + 1];
    iResult = 0;
    memset(buffer, 0, decodedHeader.size + 1);

    // Receiving data in chunks of 4096 bytes (4 kb)
    // To avoid data loss.
    for (int i = 0; i < decodedHeader.size / 4096; i++)
        iResult += recv(clientSocket, &buffer[iResult], 4096, 0);
    // The last part of the data. (For example if the size is 4097, it will be 1).
    iResult += recv(clientSocket, &buffer[iResult], decodedHeader.size - iResult, 0);
    // If number of bytes received is not equal the size specified in the decodedHeader

    if (iResult != decodedHeader.size)
    {
        cerr << EXCEPTION_CODES[14];
        exit(14);
    }
 
    buffer[decodedHeader.size] = '\0';
    string data(buffer, decodedHeader.size);
    //decrypt(data);

    msg.data = string(data);
    msg.header = copyHeader(decodedHeader);

    delete buffer;
}



void Client::encrypt(string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}


void Client::decrypt(string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}

string Client::encode(MainMsg &msg)
{
    size_t len = 0;
    len = log10(msg.header.size) + 1;
    if (len > 9)
    {
        cerr << EXCEPTION_CODES[15];
        exit(15);
    }

    string strNum = to_string(msg.header.size);
    strNum.insert(0, SIZE_LENGTH - len, '0');
    string str = strNum + to_string((int)msg.header.type)
        + (string)(msg.header.auth) + msg.data;

    str.resize(msg.header.size + 18);
    return str;
}

MessageHeader Client::decodeHeader(string &buffer)
{
    //printf("\nServer::decodeHeader- %s\n", str);
    MessageHeader decodedHeader;


    int inputSize = stoi(buffer.substr(0, SIZE_LENGTH));
    Operation inputType = (Operation)stoi(buffer.substr(SIZE_LENGTH, 1));
    string auth = buffer.substr(SIZE_LENGTH + 1, 8);

    decodedHeader.size = inputSize;
    strncpy_s(decodedHeader.auth, 9, auth.c_str(), strlen(auth.c_str()));
    decodedHeader.auth[8] = '\0';
    decodedHeader.type = inputType;

    return decodedHeader;
}
