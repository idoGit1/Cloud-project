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


Server::~Server()
{
    WSACleanup();
    closesocket(listenSocket);

    printf("\n\n\n Cleaned up successfully!");
}


string Server::encode(main_msg msg)
{
    int len = log10(msg.header.size) + 1;
    string strNum = to_string(msg.header.size);
    strNum.insert(0, SIZE_LENGTH - len, '0');
    string str = strNum + to_string((int)msg.header.type)
        + (string)(msg.header.auth) + msg.data;

    return str;
}

MessageHeader Server::decodeHeader(char *str)
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
        closesocket(clientSocket);
        exit(errorCode);
    }
    /*std::*/cerr << "Client accepted succesfully\n";
    return clientSocket;
}


void Server::handleClient(SOCKET clientSocket)
{
    main_msg msg;
    main_msg eResult;

    memset(&msg, 0, sizeof(main_msg));
    memset(&eResult, 0, sizeof(main_msg));

    // First message coming is the authentication of a user.
    UM* user = new UM(); // Initializing user manager with the first buffer inputed, that suppose to posses auth
    // info
    while (receive(clientSocket, msg))
    {
        /*std::*/cerr << "Received: " << msg.data << "\n";
        eResult = user->execute(msg); // First message will be the auth messgae.
        // If at some point the user will quit, the user object will erase its auth
        // details and at the next time login the new user.
        // create response
        if (snd(clientSocket, eResult) < 0)
        {
            /*std::*/cerr << "Sending error\n";
            closesocket(clientSocket);
            exit(1);
        }
        ///*std::*/cerr << "Sent: " << eResult << "\n";
    }
    /*std::*/cerr << "Connection ended.\n";
    closesocket(clientSocket);
    delete[] user;
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
    closesocket(clientSocket);
}


int Server::receive(SOCKET clientSocket, main_msg &msg)
{
    memset(&msg, 0, sizeof(main_msg));
    char *buffer = new char[HEADER_STRING_SIZE + 1];
    memset(buffer, 0, HEADER_STRING_SIZE + 1);
    int iResult = recv(clientSocket, buffer, HEADER_STRING_SIZE, 0);
    buffer[HEADER_STRING_SIZE] = '\0';
    if (iResult < 0)
        return iResult;

    //decrypt(buffer);
    
    MessageHeader header;
    memset(&header, 0, sizeof(MessageHeader));
    header = decodeHeader(buffer);

    buffer = new char[header.size + 1];

    memset(buffer, 0, header.size + 1);
    iResult = recv(clientSocket, buffer, header.size, 0);
    buffer[header.size] = '\0';
    //decrypt(buffer);
    cerr << (string)buffer;
    msg.data = (string)buffer;
    msg.header = header;
    
    cerr << "\nmsg.data: " << msg.data << "\n";

    return iResult;
}


int Server::snd(SOCKET clientSocket, main_msg msg)
{
    int iSendResult;
    string result = encode(msg);
    size_t mSize = strlen(result.c_str()); // Message size

    printf("\nServer::snd- \nsize of message: %zu\nauth: %s\ndata: %s\n\n", msg.header.size,
        msg.header.auth, msg.data.c_str());
    //encrypt(strMsg);

    iSendResult = send(clientSocket, result.c_str(), mSize, 0);
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
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = ~buffer[i];
    }
    string err = buffer;
}
