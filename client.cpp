#include "client.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


void Client::build(const std::string &ip)
{
    int result;

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(PORT);
    result = inet_pton(AF_INET, ip.c_str(), &this->serverAddress.sin_addr.s_addr);

    if (result <= 0)
        throw CommunicationError("Cannot create server address.", MY_LOCATION);

    result = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result == INVALID_SOCKET)
    {
        std::cerr << "Cannot connect to server.- " << MY_LOCATION;
        exit(1);
    }
}

Client::Client()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw CommunicationError("Cannot init WinSock", MY_LOCATION);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        throw CommunicationError("Cannot create socket", MY_LOCATION);

    
}


Client::~Client()
{
    WSACleanup();
    closesocket(clientSocket);
    printf("\n\n\n Client::~Client - Cleaned up successfully!");

}

void Client::snd(const MainMsg &msg)
{
    /*// Counts the bytes sent.
    int iSendResult = 0;
    std::string encodedMsg = "";
    // Transforming the struct MainMsg to a string.
    encodedMsg = encode(msg);
    //encrypt(encodedMsg);
    // Sending the data as char array (.data()).
    iSendResult = send(clientSocket, encodedMsg.data(), encodedMsg.size(), 0);
    // If the number of bytes sent is not equal to the size of the 
    // encoded message cerr << EXCEPTION_CODES[an error.
    if (iSendResult != encodedMsg.size())
    {
        std::string msgErr;
        msgErr = "Did not send full message. Sent" + std::to_string(iSendResult)
            + "/" + std::to_string(encodedMsg.size()) + " bytes.";
        throw CommunicationError(msgErr.c_str(), MY_LOCATION);
    }*/
    Network::snd(clientSocket, msg);
}

void Client::receive(MainMsg &msg)
{
   /* // Count number of bytes received
    uint64_t iResult = 0;
    // Initiating all memory to 0
    msg = MainMsg();

    char *buffer = new char[HEADER_STRING_SIZE + 1];
    memset(buffer, 0, HEADER_STRING_SIZE + 1);
    
    // Receiving the header which has a constant size. Contains- size of data, type of action, authenticatoin.
    
    iResult = recv(clientSocket, buffer, HEADER_STRING_SIZE, 0);
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
    iResult = 0;
    memset(buffer, 0, decodedHeader.size + 1);

    // Receiving data in chunks of 4096 bytes (4 kb)
    // To avoid data loss.
    if (decodedHeader.size == 0)
        goto zero_data;
    for (int i = 0; i < decodedHeader.size / 4096; i++)
        iResult += recv(clientSocket, &buffer[iResult], 4096, 0);
    // The last part of the data. (For example if the size is 4097, it will be 1).
    iResult += recv(clientSocket, &buffer[iResult], decodedHeader.size - iResult, 0);
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
    msg.header = copyHeader(decodedHeader);

    delete[] buffer;*/
    Network::receive(clientSocket, msg);
}



/*void Client::encrypt(std::string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}


void Client::decrypt(std::string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}

std::string Client::encode(MainMsg &msg)
{
    using std::string;

    size_t len = 0;
    len = (msg.header.size > 0) ? log10(msg.header.size) : 0;
    len++;
    if (len > 9)
        throw CommunicationError("Length of message it too long", MY_LOCATION);

    string strNum = std::to_string(msg.header.size);
    strNum.insert(0, SIZE_LENGTH - len, '0');
    string success = "1";

    string str = strNum + std::to_string((int)msg.header.type)
        + (string)(msg.header.auth) + success + msg.data;

    str.resize(msg.header.size + HEADER_STRING_SIZE);
    return str;
}

MessageHeader Client::decodeHeader(std::string &buffer)
{
    //printf("\nServer::decodeHeader- %s\n", str);
    MessageHeader decodedHeader;


    int inputSize = stoi(buffer.substr(0, SIZE_LENGTH));
    Operation inputType = (Operation)stoi(buffer.substr(SIZE_LENGTH, 1));
    std::string auth = buffer.substr(SIZE_LENGTH + 1, 8);
    std::string success = buffer.substr(SIZE_LENGTH + 9, 1);
    decodedHeader.size = inputSize;
    strncpy_s(decodedHeader.auth, 9, auth.c_str(), strlen(auth.c_str()));
    decodedHeader.auth[8] = '\0';
    decodedHeader.type = inputType;
    decodedHeader.success = success == "1" ? true : false;
    return decodedHeader;
}
*/