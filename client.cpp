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


int Client::snd(Message *msg)
{
    int iSendResult;
    char *strMsg;
    strMsg = encode(msg);
    //encrypt(strMsg, sizeof(main_msg));

    iSendResult = send(clientSocket, strMsg, strlen(strMsg) + 1, 0);

    return iSendResult;
}


int Client::receive(Message *msg)
{
    char *buffer = new char[sizeof(MessageHeader)];
    MessageHeader *header;
    int iResult = recv(clientSocket, buffer, sizeof(MessageHeader), 0);
    if (iResult < 0)
        return iResult;

    decrypt(buffer);

    header = new MessageHeader();
    memcpy(header, buffer, sizeof(MessageHeader));

    msg = new Message(header);
    buffer = new char[msg->msg.header.size];

    iResult = recv(clientSocket, buffer, msg->msg.header.size, 0);
    decrypt(buffer);
    msg = decode(buffer);
    return iResult;
}



void Client::encrypt(char *buffer, int len)
{
    for (int i = 0; i < len; i++)
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


char *Client::encode(Message *msg)
{
    char *ret = new char[sizeof(main_msg)];
    memset(ret, 0, sizeof(main_msg));
    //memcpy(ret, &msg->msg, sizeof(main_msg));
    memcpy(ret, &(msg->msg), sizeof(main_msg));
    return ret;
}

Message *Client::decode(char *str)
{
    Message *msg = new Message();
    memcpy(msg, str, sizeof(str));
    return msg;
}