#include "server.h"
#include <memory>
#include <format>
#pragma comment(lib, "Ws2_32.lib")

Server::Server()
{
    // Building the server- initializing listenSocket (member) and 
        // result to check if functions worked.
    listenSocket = 0;
    int result = 0;

    // Taken from Microsoft tutorial- initializing winsock with 
    // version 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Init did not work
        throw CommunicationError("Cannot init WinSock", MY_LOCATION);
    }
    // Defining the server address/
    this->serverAddress.sin_family = AF_INET;
    // Defining that the server will listen to every ip address.
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    // Defining port, 1111. Randomly chosen because it is not in use.
    this->serverAddress.sin_port = htons(PORT);

    //cerr << "Server constructed successfully\n";
}


Server::~Server()
{
    // Cleaning up, releasing port and closing socket.

    WSACleanup();
    closesocket(listenSocket);

    printf("\n\n\n Cleaned up successfully!");
}

SOCKET Server::acceptClient()
{
    SOCKET clientSocket = 0;
    // Accepting a client. In the future add thread.

        // Taken from Microsoft tutorial
    clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
        throw CommunicationError("Cannot accept client", MY_LOCATION);
    return clientSocket;
}


void Server::handleClient(ClientDetails *clientDet)
{
    MainMsg msg;
    MainMsg eResult;
    clientDet->setThreadId(std::this_thread::get_id());
    //activeClients.push_back(clientDet);

    // First message coming is the authentication of a user.
    // Initializing user manager with the first
    // buffer inputed, that suppose to posses auth
    std::unique_ptr<UM> user(new UM());
    while (isSocketActive(clientDet->getSocket()))
    {
        try
        {
            receive(clientDet->getSocket(), msg);
        }
        catch (CommunicationError &ex)
        {
            std::cerr << ex.what();
            return;
        }
        // First message will be the auth messgae.
        try
        {
            eResult = user->execute(msg);

            if (eResult.header.type == Null)
            {
                delete clientDet;
                return;
            }
        }
        catch (CommunicationError &ex)
        {
            std::cerr << ex.what();
            delete clientDet;
            return;
        }
        // If at some point the user will quit, 
        // the user object will erase its auth
        
        // create response
        try
        {
            snd(clientDet->getSocket(), eResult);
        }
        catch (...)
        {
            delete clientDet;
            throw;
        }
    }
    //activeClients.erase()
    delete clientDet;
}

void Server::build()
{

        // 1. Creating listen socket
        // 2. Binding the socket to the address specified
        // 3. Listening for new connections
        // Taken from Microsoft tutorial
        // 
        // Create server socket
    if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        throw CommunicationError("Cannot create a server socket", MY_LOCATION);
        // Bind socket
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        throw CommunicationError("Cannot bind socket", MY_LOCATION);        // Listen for connections - length of queue is Maximum
    if (listen(listenSocket, 5) == SOCKET_ERROR)
        throw CommunicationError("Cannot open socket for listening", MY_LOCATION);

    //cerr << "Built succesfully\n";
}

void Server::run()
{
    //cerr << "Server::run\n";
    SOCKET clientSocket;
    // Creating server socket, binding to ip and listening.
    try
    {
        build();
    }
    catch (...)
    {
        throw; // Cannot continue with program if server is not up and running.
    }
    //cerr << "Server is up and running\n";
    while (true)
    {

        try
        {
            // Accepting the client
            clientSocket = acceptClient();
        }
        catch (CommunicationError &ex)
        {
            std::cerr << ex.what() << clientSocket;
            break;
        }

        // Creating a thread for the client and detaching it-
        // NOT waiting for it to end.
        ClientDetails *clientDetails = new ClientDetails;
        clientDetails->setSocket(clientSocket);
        std::thread clientThread(&Server::handleClient, this, clientDetails);
        clientThread.detach();
    }
}
