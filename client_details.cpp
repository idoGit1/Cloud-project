#include "clientd_details.h"

ClientDetails::~ClientDetails()
{
	closesocket(socket);
}

SOCKET ClientDetails::getSocket()
{
	return socket;
}
std::thread::id ClientDetails::getId()
{
	return myId;
}

void ClientDetails::setSocket(SOCKET socket)
{
	this->socket = socket;
}

void ClientDetails::setThreadId(std::thread::id id)
{
	this->myId = id;
}