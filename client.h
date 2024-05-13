#pragma once
#define WIN32_LEAN_AND_MEAN
#define CLIENT_H_
#ifndef MY_PROJECT_HEADER_H_
#include <d:/Cloud project/Header/my_project_header.h>
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
	void encrypt(string &);
	void decrypt(string &);

	string encode(MainMsg &);
	MessageHeader decodeHeader(string &);
	//MainMsg decode(char *);

	//int sendFile(MainMsg, vector<string> &);
	void snd(MainMsg &);
	void receive(MainMsg &);
};

