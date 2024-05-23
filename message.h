#pragma once
#ifndef MESSAGE_H_
#define MESSAGE_H_
#include "my_project_header.h"
#define SUCCESS (std::string)"1"
#define FAILURE (std::string)"0"
#define BLANKAUTH "--------"
#include <string>

struct MessageHeader
{
	size_t size; // Size of the data
	Operation type;
	char auth[9]; // Authentication token.
	bool success;

	MessageHeader()
	{
		size = 0;
		type = (Operation)0;
		memset(auth, 0, 8);
		success = false;
	}
	MessageHeader(const Operation &type, const char auth[9], const bool &success)
	{
		this->size = 0;
		this->type = type;
		strncpy_s(this->auth, 9, auth, 8);
		this->auth[8] = '\0';
		this->success = success;
	}
	MessageHeader(const MessageHeader &obj)
	{
		size = obj.size;
		type = obj.type;
		strncpy_s(auth, 9, obj.auth, 8);
		auth[8] = '\0';
		success = obj.success;
	}
};

struct MainMsg
{
	MessageHeader header;
	std::string data;
	MainMsg()
	{
		// Header is init automatically.
		data = "";
	}
	MainMsg(const MainMsg &obj)
	{
		header = MessageHeader(obj.header);
		data = obj.data;
	}
	MainMsg(const Operation &type, const char auth[9],
		const bool &success, const std::string &data)
	{
		header = MessageHeader(type, auth, success);
		this->data = data;
		header.size = this->data.size();
	}

	void clean()
	{
		header = MessageHeader();
		data = "";
	}
};

void cleanMsg(MainMsg &);

MessageHeader copyHeader(MessageHeader &);

/*class Message
{
	MessageHeader header;
	std::string data;
public:

	Message();

	MessageHeader getHeader();
	void setHeader(MessageHeader &);

	std::string getData();
	void setData(std::string);

	std::string encode();
	void decode(std::string &, std::string);

	std::string encrypt();
	std::string decrypt();
};*/

#endif