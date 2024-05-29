#pragma once
#ifndef NETWORK_H_
#define WIN32_LEAN_AND_MEAN
#define NETWORK_H_

#include <WinSock2.h>
#include <iostream>

#include "d:/Cloud project/Additional/message.h"
#include "d:/Cloud project/Additional/my_project_header.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class Network
{
private:
	//WSADATA wsaData;
	//struct sockaddr_in serverAddress;
protected:
	bool isSocketActive(SOCKET sock);

	void encrypt(std::string &) const;
	void decrypt(std::string &) const;

	std::string encode(const MainMsg &) const;
	MessageHeader decodeHeader(const std::string &) const;
public:
	void snd(SOCKET, const MainMsg &);
	void receive(SOCKET, MainMsg &);

};

#endif