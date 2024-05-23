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


bool Server::isSocketActive(SOCKET sock)
{
    // Try to receive data, without removing it from the buffer using the MSG_PEEK flag.
    char oneCharBuffer; 
    int result = recv(sock, &oneCharBuffer, 1, MSG_PEEK);

    if (result == SOCKET_ERROR)
        return false;

    else if (result == 0) // Connection is closed
        return false;
    return true;
}


std::string Server::encode(MainMsg msg)
{
    using std::string;
    size_t len = 0;

    len = (msg.header.size > 0) ? (size_t)log10(msg.header.size) : 0;
    len++;
    if (len > 9)
        throw CommunicationError("Length of message it too long", MY_LOCATION);
    
    string strNum = std::to_string(msg.header.size);
    strNum.insert(0, SIZE_LENGTH - len, '0');
    string success = msg.header.success ? "1" : "0";
    string str = strNum + std::to_string((int)msg.header.type)
        + (string)(msg.header.auth) + success + msg.data;

    str.resize(msg.header.size + HEADER_STRING_SIZE);
    return str;
}

MessageHeader Server::decodeHeader(std::string &buffer)
{
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


void Server::handleClient(SOCKET clientSocket)
{
    MainMsg msg;
    MainMsg eResult;


    // First message coming is the authentication of a user.
    // Initializing user manager with the first
    // buffer inputed, that suppose to posses auth
    std::unique_ptr<UM> user(new UM());
    while (isSocketActive(clientSocket))
    {
        try
        {
            receive(clientSocket, msg);
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
        }
        catch (CommunicationError &ex)
        {
            std::cerr << ex.what();
            return;
        }
        // If at some point the user will quit, 
        // the user object will erase its auth
        
        // create response
        try
        {
            snd(clientSocket, eResult);
        }
        catch (...)
        {
            throw;
        }
    }
    closesocket(clientSocket);
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
            clientSocket = acceptClient();
        }
        catch (CommunicationError &ex)
        {
            std::cerr << ex.what() << clientSocket;
            break;
        }

        //handleClient(clientSocket);
        // Creating a thread for the client and detaching it-
        // NOT waiting for it to end.
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
    closesocket(clientSocket);
}


int Server::receive(SOCKET clientSocket, MainMsg &msg)
{
    // Count number of bytes received
    uint64_t iResult = 0;

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
    memset(buffer, 0, decodedHeader.size + 1);

    iResult = 0;

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
    //cerr << data;

    //cerr << (string)data;
    //printf("\n%s", buffer);
    delete[] buffer;
}


int Server::snd(SOCKET clientSocket, MainMsg &msg)
{
    // Counts the bytes sent.
    int iSendResult = 0;
    std::string encodedMsg;

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

        throw ContentError(msgErr.c_str(), MY_LOCATION);
    }
    return iSendResult;
}


void Server::encrypt(std::string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}


void Server::decrypt(std::string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}
