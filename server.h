#pragma once
#ifndef SERVER_H_
#define WIN32_LEAN_AND_MEAN
#define SERVER_H_
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <mutex>

#include "my_project_header.h"
#include "um.h"
#include "message.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class Server
{
private:
	WSADATA wsaData;
	struct sockaddr_in serverAddress;
	SOCKET listenSocket;

	bool isSocketActive(SOCKET sock);
public:
	Server();
	~Server();

	void build();

	SOCKET acceptClient();
	void handleClient(SOCKET);

	void encrypt(std::string &);
	void decrypt(std::string &);

	std::string encode(MainMsg);
	MessageHeader decodeHeader(std::string &);
	//MainMsg decode(char *);

	int snd(SOCKET, MainMsg &);
	int receive(SOCKET, MainMsg &);

	//int receiveFile(SOCKET, MainMsg &);

	void run();

};

#endif