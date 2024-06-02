#pragma once
#ifndef CLIENT_H_
#define WIN32_LEAN_AND_MEAN // To speed up build
#define CLIENT_H_

#include "d:/Cloud project/Additional/my_project_header.h"
#include "d:/Cloud project/Additional/Message.h"
#include "D:/Cloud project/Additional/network.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class Client : public Network
{
	WSADATA wsaData;
	struct sockaddr_in serverAddress;
	SOCKET clientSocket;
public:

	Client();
	~Client();

	void snd(const MainMsg &);
	void receive(MainMsg &);
};

#endif