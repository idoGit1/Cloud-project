#pragma once
#define WIN32_LEAN_AND_MEAN
#define SERVER_H_
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#ifndef COMMON_H_
#include "common.h"
#endif
#ifndef UM_H_
#include "um.h"
#endif

#ifndef MESSAGE_H_
#include "message.h"
#endif

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Server
{
private:
	WSADATA wsaData;
	SOCKET listenSocket;
	struct sockaddr_in serverAddress;
	//vector<string>dataChunks;
public:
	Server();

	~Server();

	void build();

	SOCKET acceptClient();
	void handleClient(SOCKET);

	void encrypt(char *);
	void decrypt(char *);

	//char *encode(Message *);
	//Message *decode(char *);
	string encode(main_msg);
	MessageHeader decodeHeader(char *);
	//main_msg decode(char *);

	int snd(SOCKET, main_msg);
	int receive(SOCKET, main_msg &);

	//int receiveFile(SOCKET, main_msg &);

	void run();

};

