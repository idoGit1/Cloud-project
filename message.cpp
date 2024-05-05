#include "message.h"


MessageHeader copyHeader(MessageHeader &obj)
{
	MessageHeader cpy;
	cpy.size = obj.size;
	cpy.type = obj.type;
	strncpy_s(cpy.auth, 9, obj.auth, 8);
	cpy.auth[8] = '\0';
	return cpy;
}

/*
Message::Message()
{

	msg.header.auth.reserve(8);
	memset(&msg, 0, sizeof(this));
}

Message::Message(uint64_t size, Operation type)
{
	msg.header.auth.reserve(8);
	memset(&msg, 0, sizeof(this));
	msg.header.size = size;
	msg.header.type = type;
}


Message::Message(MessageHeader *obj)
{
	msg.header.auth.reserve(8);
	memset(&msg, 0, sizeof(this));
	msg.header.size = obj->size;
	msg.header.type = obj->type;
}


void Message::setData(string data)
{
	msg.data = data;
	msg.header.size = sizeof(msg.data.c_str());
}


void Message::setType(Operation type)
{
	this->msg.header.type = type;
}


void Message::setAuth(string auth)
{
	msg.header.auth = auth;
}


string Message::getData()
{
	return msg.data;
}


Operation Message::getType()
{
	return msg.header.type;
}


string Message::getAuth()
{
	return msg.header.auth;
}


uint64_t Message::getSize()
{
	return msg.header.size;
}


MessageHeader *Message::getHeader()
{
	MessageHeader *cpy = new MessageHeader;
	cpy->auth = msg.header.auth;
	cpy->size = msg.header.size;
	cpy->type = msg.header.type;

	return cpy;
}
*/