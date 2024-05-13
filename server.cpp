#include "server.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;



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
            cerr << EXCEPTION_CODES[1];
        }
    // Defining the server address/
    this->serverAddress.sin_family = AF_INET;
    // Defining that the server will listen to every ip address.
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    // Defining port, 1111. Randomly chosen because it is not in use.
    this->serverAddress.sin_port = htons(PORT);

    cerr << "Server constructed successfully\n";
}


Server::~Server()
{
    // Cleaning up, releasing port and closing socket.

    WSACleanup();
    closesocket(listenSocket);

    printf("\n\n\n Cleaned up successfully!");
}


string Server::encode(MainMsg msg)
{
    size_t len = 0;

    len = (int)log10(msg.header.size) + 1;
    if (len > 9)
        cerr << EXCEPTION_CODES[2];

    string strNum = to_string(msg.header.size);
    strNum.insert(0, SIZE_LENGTH - len, '0');
    string str = strNum + to_string((int)msg.header.type)
        + (string)(msg.header.auth) + msg.data;

    str.resize(msg.header.size + 18);
    return str;
}

MessageHeader Server::decodeHeader(string &buffer)
{
    MessageHeader decodedHeader;


    int inputSize = stoi(buffer.substr(0, SIZE_LENGTH));
    Operation inputType = (Operation)stoi(buffer.substr(SIZE_LENGTH, 1));
    string auth = buffer.substr(SIZE_LENGTH + 1, 8);

    decodedHeader.size = inputSize;
    strncpy_s(decodedHeader.auth, 9, auth.c_str(), 8);
    decodedHeader.auth[8] = '\0';
    decodedHeader.type = inputType;

    return decodedHeader;
}

SOCKET Server::acceptClient()
{
    SOCKET clientSocket = 0;
    // Accepting a client. In the future add thread.

        // Taken from Microsoft tutorial
        clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
            cerr << EXCEPTION_CODES[3];
        else
            cerr << "Client accepted\n";
    return clientSocket;
}


void Server::handleClient(SOCKET clientSocket)
{
    MainMsg msg;
    MainMsg eResult;


    // First message coming is the authentication of a user.
    // Initializing user manager with the first
    // buffer inputed, that suppose to posses auth
    UM* user = new UM(); 
    while (receive(clientSocket, msg))
    {
        // First message will be the auth messgae.

            eResult = user->execute(msg);

        // If at some point the user will quit, 
        // the user object will erase its auth
        
        // create response
        if (snd(clientSocket, eResult) < 0)
        {
            ///*std::*/cerr << "Sending error\n";
            closesocket(clientSocket);
            exit(1);
        }
        ///*std::*/cerr << "Sent: " << eResult << "\n";
    }
    ///*std::*/cerr << "Connection ended.\n";
    closesocket(clientSocket);
    delete user;
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
            cerr << EXCEPTION_CODES[4];
        // Bind socket
        if (bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
            cerr << EXCEPTION_CODES[5];
        // Listen for connections - length of queue is Maximum
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
            cerr << EXCEPTION_CODES[6];

    cerr << "Built succesfully\n";
}

void Server::run()
{
    cerr << "Server::run\n";
    SOCKET clientSocket;
    // Creating server socket, binding to ip and listening.
    

    build();

    cerr << "Server is up and running\n";
    while (true)
    {

            clientSocket = acceptClient();
            if (clientSocket == INVALID_SOCKET)
            {
                cerr << "Cant accept client" << "\n";
                continue; // Cannot go on to handle client.
            }


        //handleClient(clientSocket);
        // Creating a thread for the client and detaching it-
        // NOT waiting for it to end.
        thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
    closesocket(clientSocket);
}


int Server::receive(SOCKET clientSocket, MainMsg &msg)
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
        cerr << EXCEPTION_CODES[7];
        exit(7);
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
        cerr << EXCEPTION_CODES[7];
        exit(7);
    }

    buffer[decodedHeader.size] = '\0';
    string data(buffer, decodedHeader.size);
    //decrypt(data);

    msg.data = string(data);
    msg.header = copyHeader(decodedHeader);
    cerr << data;

    cerr << (string)data;
    printf("\n%s", buffer);
    delete[] buffer;
}


int Server::snd(SOCKET clientSocket, MainMsg &msg)
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
        cerr << EXCEPTION_CODES[8];
    return iSendResult;
}


void Server::encrypt(string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}


void Server::decrypt(string &buffer)
{
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = ~buffer[i];
    }
}
