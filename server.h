#pragma once
#define WIN32_LEAN_AND_MEAN
#define SERVER_H_
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <mutex>
#ifndef MY_PROJECT_HEADER_H_
#include <d:/Cloud project/Header/my_project_header.h>
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
	struct sockaddr_in serverAddress;
	SOCKET listenSocket;
public:
	Server();

	~Server();

	void build();

	SOCKET acceptClient();
	void handleClient(SOCKET);

	void encrypt(string &);
	void decrypt(string &);

	string encode(MainMsg);
	MessageHeader decodeHeader(string &);
	//MainMsg decode(char *);

	int snd(SOCKET, MainMsg &);
	int receive(SOCKET, MainMsg &);

	//int receiveFile(SOCKET, MainMsg &);

	void run();

};

