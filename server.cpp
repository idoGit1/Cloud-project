#include "server.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


Server::Server()
{
    listenSocket = 0;
    int result;
    try 
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw "Winsock initialization failed.";
        }
        
    }
    catch (/*std::*/string errorMsg)
    {
        /*std::*/cerr << errorMsg;
        exit(1);
    }
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    this->serverAddress.sin_port = htons(PORT);

    /*std::*/cerr << "Server constructed succesfully\n";
}


char *Server::encode(Message *msg)
{
    char *str = reinterpret_cast<char *>(&msg);
    return str;
}

Message *Server::decode(char *str)
{
    Message *msg = new Message;
    msg = reinterpret_cast<Message *>(&str);
    return msg;
}


SOCKET Server::acceptClient()
{
    SOCKET clientSocket;
    // Accepting a client. In the future add thread.
    try
    {
        clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
            throw 4;
    }
    catch (int errorCode)
    {
        closesocket(listenSocket);
        WSACleanup();
        exit(errorCode);
    }
    /*std::*/cerr << "Client accepted succesfully\n";
    return clientSocket;
}


void Server::handleClient(SOCKET clientSocket)
{
    Message* msg = new Message;
    Message* eResult;

    // First message coming is the authentication of a user.
    UM* user = new UM(); // Initializing user manager with the first buffer inputed, that suppose to posses auth
    // info
    while (receive(clientSocket, msg))
    {
        /*std::*/cerr << "Received: " << msg->msg.data << "\n";
        eResult = user->execute(msg); // First message will be the auth messgae.
        // If at some point the user will quit, the user object will erase its auth
        // details and at the next time login the new user.
        // create response
        if (snd(clientSocket, eResult) < 0)
        {
            /*std::*/cerr << "Sending error\n";
            exit(1);
        }
        ///*std::*/cerr << "Sent: " << eResult << "\n";
    }
    /*std::*/cerr << "Connection ended.\n";
    closesocket(clientSocket);
}

void Server::build()
{
    try
    {
        // Create server socket
        if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
            throw 1;
        // Bind socket
        if (bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
            throw 2;
        // Listen for connections
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
            throw 3;
    }
    catch (int errorCode)
    {
        WSACleanup();
        switch (errorCode)
        {
        case 2:
            closesocket(listenSocket);
        case 3:
            closesocket(listenSocket);
        }
        exit(errorCode);

    }
    /*std::*/cerr << "Built succesfully\n";
}

void Server::run()
{
    SOCKET clientSocket;
    // Creating server socket, binding to ip and listening.
    
    build();
    /*std::*/cout << "Server is up and running!\n";

    while (true)
    {
        clientSocket = acceptClient();

        cerr << "Client accepted.\n";

        handleClient(clientSocket);
        //thread t(handleClient, clientSocket);
        //t.detach();
    }
    WSACleanup();
    closesocket(listenSocket);
}


int Server::receive(SOCKET clientSocket, Message* msg)
{
    char* buffer = new char[sizeof(MessageHeader)];
    MessageHeader* header = new MessageHeader();
    int iResult = recv(clientSocket, buffer, sizeof(MessageHeader), 0);
    if (iResult < 0)
        return iResult;

    //decrypt(buffer);

    memcpy(header, buffer, sizeof(MessageHeader));

    msg = new Message(header);
    buffer = new char[msg->msg.header.size];

    iResult = recv(clientSocket, buffer, msg->msg.header.size, 0);
    decrypt(buffer);
    msg = decode(buffer);
    return iResult;
}


int Server::snd(SOCKET clientSocket, Message *msg)
{
    int iSendResult;
    char* strMsg;
    strMsg = encode(msg);

    encrypt(strMsg);

    iSendResult = send(clientSocket, strMsg, strlen(strMsg), 0);
    return iSendResult;
}


void Server::encrypt(char *buffer)
{
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = ~buffer[i] ^ 1;
    }
}


void Server::decrypt(char *buffer)
{
    for (int i = 0; i < sizeof(buffer); i++)
    {
        buffer[i] = ~buffer[i];
    }
    string err = buffer;
}
