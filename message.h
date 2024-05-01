#pragma once
#define MESSAGE_H_
#ifndef COMMON_H_
#include "common.h"
#endif
#define SUCCESS "1"
#define FAILURE "0"
#define BLANKAUTH "--------"

using namespace std;

struct MessageHeader
{
	uint64_t size; // Size of the data
	Operation type;
	string auth; // Authehntication token.
};

struct main_msg
{
	MessageHeader header;
	string data;
};
class Message
{
	main_msg msg;
	//MessageHeader header;
	//string data;
public:
	Message();

	Message(uint64_t, Operation);
	Message(MessageHeader *);

	void setData(string);
	void setType(Operation);
	void setAuth(string);

	string getData();
	Operation getType();
	string getAuth();
	uint64_t getSize();
	MessageHeader *getHeader();

	friend class Server;
};