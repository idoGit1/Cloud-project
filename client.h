#pragma once
#ifndef CLIENT_H_
#define WIN32_LEAN_AND_MEAN
#define CLIENT_H_

#include "my_project_header.h"
#include "Message.h"

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
	static IpType ipType;

	void build(const std::string &);
	Client();
	~Client();
	void encrypt(std::string &);
	void decrypt(std::string &);

	std::string encode(MainMsg &);
	MessageHeader decodeHeader(std::string &);
	//MainMsg decode(char *);

	//int sendFile(MainMsg, vector<std::string> &);
	void snd(MainMsg &);
	void receive(MainMsg &);
};

#endif