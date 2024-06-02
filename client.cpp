#include "client.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



Client::Client()
{
    // Client constructor- initializing the winsock, creating socket, and connecting to server
    
    // Taken from Microsoft tutorial- initializing winsock with 
    // version 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        // Init did not work
        throw CommunicationError("Cannot init WinSock", MY_LOCATION);
    
    // Creating a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        // Creating a socket did not work
        throw CommunicationError("Cannot create socket", MY_LOCATION);

    int result;
    // Creating server address:
    // AF_INET- regular internet communication
    // PORT- the port I chose (htons converts it to short)
    // EXTERN_IP- the ip of the cloud device (extern because I've been using local host)
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(PORT);
    result = inet_pton(AF_INET, EXTERN_IP, &this->serverAddress.sin_addr.s_addr);
    // Checking result
    if (result <= 0)
        throw CommunicationError("Cannot create server address.", MY_LOCATION);
    
    // Connecting to server with address
    result = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result == INVALID_SOCKET)
    {
        std::cerr << "Cannot connect to server- " << MY_LOCATION;
        WSACleanup();
        exit(1);
    }
}


Client::~Client()
{
    // Client destructor- cleaning up.
    WSACleanup();
    closesocket(clientSocket);

}

void Client::snd(const MainMsg &msg)
{
    // Getting a message to send, using the Network::snd with the client socket.
    Network::snd(clientSocket, msg);
}

void Client::receive(MainMsg &msg)
{
   // Getting a message to receive, using the Networ::receive to receive with the client socket.
    Network::receive(clientSocket, msg);
}
