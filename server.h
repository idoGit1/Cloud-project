#pragma once
#ifndef SERVER_H_
#define WIN32_LEAN_AND_MEAN
#define SERVER_H_
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <mutex>
#include "clientd_details.h"
#include "d:/Cloud project/Additional/my_project_header.h"
#include "um.h"
#include "d:/Cloud project/Additional/message.h"
#include "d:/Cloud project/Additional/network.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class Server : Network
{
private:
	WSADATA wsaData;
	struct sockaddr_in serverAddress;
	SOCKET listenSocket;

	//bool isSocketActive(SOCKET sock);
	void build();

	SOCKET acceptClient();
	void handleClient(ClientDetails *);
public:
	//static std::vector<ClientDetails *> activeClients;
	Server();
	~Server();
	

	//void encrypt(std::string &);
	//void decrypt(std::string &);

	//std::string encode(MainMsg);
	//MessageHeader decodeHeader(std::string &);
	//MainMsg decode(char *);

	//void snd(SOCKET, MainMsg &);
	//void receive(SOCKET, MainMsg &);

	//int receiveFile(SOCKET, MainMsg &);

	void run();

};

#endif