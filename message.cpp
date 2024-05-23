#include "message.h"

MessageHeader copyHeader(MessageHeader &obj)
{
	MessageHeader cpy;
	cpy.success = obj.success;
	cpy.size = obj.size;
	cpy.type = obj.type;
	strncpy_s(cpy.auth, 9, obj.auth, 8);
	cpy.auth[8] = '\0';
	return cpy;
}

void cleanMsg(MainMsg &msg)
{
	memset(msg.header.auth, 0, 9);
	msg.header.type = (Operation)0;
	msg.header.size = 0;
	msg.data = "";
}

/*Message::Message()
{
	memset(this->header.auth, 0, 9);
	this->header.type = Null;
	this->header.size = 0;
	this->data = "";
}

MessageHeader Message::getHeader()
{
	return copyHeader(header);
}
void Message::setHeader(MessageHeader &header)
{
	this->header = copyHeader(header);
}


string Message::getData()
{
	return data;
}
void Message::setData(string data)
{
	this->data = data;
}


string Message::encode()
{
	size_t len = 0;

	len = (int)log10(this->header.size) + 1;
	if (len > 9)
		cerr << EXCEPTION_CODES[2];

	string strNum = to_string(this->header.size);
	strNum.insert(0, SIZE_LENGTH - len, '0');
	string str = strNum + to_string((int)this->header.type)
		+ (string)(this->header.auth) + this->data;

	str.resize(this->header.size + 18);
	return str;
}

void Message::decode(string &headerBuffer, string dataBuffer)
{
	
	int inputSize = stoi(headerBuffer.substr(0, SIZE_LENGTH));
	Operation inputType = (Operation)stoi(headerBuffer.substr(SIZE_LENGTH, 1));
	string auth = headerBuffer.substr(SIZE_LENGTH + 1, 8);

	header.size = inputSize;
	strncpy_s(header.auth, 9, auth.c_str(), 8);
	header.auth[8] = '\0';
	header.type = inputType;

	this->data = dataBuffer;

}*/

