#pragma once
#define WIN32_LEAN_AND_MEAN
#define CLIENT_H_
#ifndef COMMON_H_
#include "common.h"
#endif
#ifndef MESSAGE_H_
#include "Message.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Client
{
	WSADATA wsaData;
	struct sockaddr_in serverAddress;
	SOCKET clientSocket;
public:
	Client();
	~Client();
	void encrypt(char *);
	void decrypt(char *);

	string encode(main_msg &);
	MessageHeader decodeHeader(char *);
	//main_msg decode(char *);

	//int sendFile(main_msg, vector<string> &);
	int snd(main_msg &);
	int receive(main_msg &);
};

