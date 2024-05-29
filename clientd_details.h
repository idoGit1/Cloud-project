#pragma once
#ifndef CLIENT_DETAILS_H_
#define CLIENT_DETAILS_H_

#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class ClientDetails
{
	SOCKET socket;
	std::thread::id myId;

public:
	~ClientDetails();

	SOCKET getSocket();
	std::thread::id getId();

	void setSocket(SOCKET);
	void setThreadId(std::thread::id);
};

#endif
