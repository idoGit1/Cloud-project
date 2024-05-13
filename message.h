#pragma once
#define MESSAGE_H_
#ifndef MY_PROJECT_HEADER_H_
#include "d:/Cloud project/Header/my_project_header.h"
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
	char auth[9]; // Authentication token.

	MessageHeader()
	{
		size = 0;
		type = Null;
		memset(auth, 0, 8);
	}
};

MessageHeader copyHeader(MessageHeader &);
struct MainMsg
{
	MessageHeader header;
	string data;

	MainMsg()
	{
		// Header is init automatically.
		data = "";
	}
};

void cleanMsg(MainMsg &);

/*
class Message
{
	MainMsg msg;
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
*/