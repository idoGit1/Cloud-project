#include "client.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

Client::Client()
{
    int result;
    try
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw "Winsock initialization failed.";
        }
    }
    catch (string errorMsg)
    {
        cerr << errorMsg;
        exit(1);
    }
    clientSocket = socket(FAMILY, TYPE, 0);

    /*int addrLen = sizeof(serverAddress);
    char *addrStr = new char[strlen(IP) + 1];
    strncpy(addrStr, IP, strlen(IP));
    addrStr[strlen(IP)] = '\0';*/

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(PORT);
    result = inet_pton(AF_INET, IP, &this->serverAddress.sin_addr.s_addr);

    /*if (result != 0)
    {
        cerr << "Error in conversion";
        exit(1);
    }
    */

    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "Error create client socket";
        exit(1);
    }
    cerr << "Client constructed successfully\n";


    result = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (result == INVALID_SOCKET)
    {
        cerr << "Error connecting to server";
        exit(1);
    }
        
}


Client::~Client()
{
    WSACleanup();
    closesocket(clientSocket);
    printf("\n\n\n Cleaned up successfully!");

}


int Client::snd(main_msg &msg)
{
    int iSendResult;
    string result = encode(msg);
    //size_t mSize = strlen(result.c_str()); // Message size

    //printf("\nClient::snd- \nsize of message: %zu\nauth: %s\ndata: %s\n\n", msg.header.size,
        //msg.header.auth, msg.data.c_str());
    //encrypt(strMsg);
    if (result.size() != msg.header.size + 18)
        printf("Client::snd\n");
    iSendResult = send(clientSocket, result.data(), result.size(), 0);
    if (iSendResult != result.size())
        printf("Client::snd2\n");
    cerr << "Sent " << iSendResult << " bytes\n";
    return iSendResult;
}


/*int Client::sendFile(main_msg msg, vector<string> &dataChunks)
{
    snd(msg);
    main_msg line;
    for (int i = 0; i < dataChunks.size(); i++)
    {
        memset(&line, 0, sizeof(main_msg));

        line.header.type = Upload;
        strncpy_s(line.header.auth, 9, msg.header.auth, 8);
        line.header.auth[8] = '\0';
        line.data = (string)dataChunks[i];
        line.header.size = line.data.size();
        snd(line);
    }
}*/



int Client::receive(main_msg &msg)
{
    memset(&msg, 0, sizeof(main_msg));
    char *buffer = new char[HEADER_STRING_SIZE + 1];
    memset(buffer, 0, HEADER_STRING_SIZE + 1);
    int iResult = recv(clientSocket, buffer, HEADER_STRING_SIZE, 0);
    buffer[HEADER_STRING_SIZE] = '\0';
    if (iResult < 0)
        return iResult;

    //decrypt(buffer);

    MessageHeader header = decodeHeader(buffer);

    buffer = new char[header.size + 1];

    memset(buffer, 0, header.size + 1);
    iResult = recv(clientSocket, buffer, header.size, 0);
    buffer[header.size] = '\0';
    //decrypt(buffer);
    cerr << (string)buffer;
    msg.data = (string)buffer;
    msg.header = copyHeader(header);

    cerr << "\nmsg.data: " << msg.data << "\n";

    delete[] buffer;
    return iResult;
}



void Client::encrypt(char *buffer)
{
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = ~buffer[i] ^ 1;
    }
}


void Client::decrypt(char *buffer)
{
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = ~buffer[i];
    }
}


string Client::encode(main_msg &msg)
{
    size_t len = log10(msg.header.size) + 1;
    string strNum = to_string(msg.header.size);
    printf("%lld", ((string)msg.header.auth).size());
    strNum.insert(0, SIZE_LENGTH - len, '0');
    string str = strNum + to_string((int)msg.header.type)
        + (string)(msg.header.auth) + msg.data;

    printf("size str: %d size data: %d\n", str.size(), msg.data.size());
    printf("%c\n", str[str.size() - 1]);
    str.resize(msg.header.size + 18);
    return str;
}

MessageHeader Client::decodeHeader(char *str)
{
    printf("\nServer::decodeHeader- %s\n", str);
    MessageHeader result;
    memset(&result, 0, sizeof(result));

    string input = str;

    int inputSize = stoi(input.substr(0, SIZE_LENGTH));
    Operation inputType = (Operation)stoi(input.substr(SIZE_LENGTH, SIZE_LENGTH + 1));
    string auth = input.substr(SIZE_LENGTH + 1, SIZE_LENGTH + 9);

    result.size = inputSize;
    strncpy_s(result.auth, 9, auth.c_str(), strlen(auth.c_str()));
    result.auth[8] = '\0';
    result.type = inputType;

    return result;
}



/*main_msg Client::decode(char *str)
{
    main_msgmsg = new Message();
    memcpy(msg, str, sizeof(str));
    return msg;
}*/