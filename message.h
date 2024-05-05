#pragma once
#define MESSAGE_H_
#ifndef COMMON_H_
#include "common.h"
#endif
#define SUCCESS (string)"1"
#define FAILURE (string)"0"
#define BLANKAUTH "--------"

#include <string>
using namespace std;

struct MessageHeader
{
	size_t size; // Size of the data
	Operation type;
	char auth[9]; // Authehntication token.
};

MessageHeader copyHeader(MessageHeader &);

struct main_msg
{
	MessageHeader header;
	string data;
};
/*
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



	friend class Client;
};
*/