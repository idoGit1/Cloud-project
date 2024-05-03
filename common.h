#pragma once
#undef UNICODE
#define COMMON_H_

#include <iostream>
#include <fstream>
#include <thread>
#include <cstdint>

#ifndef MACROS
#define MACROS
#define PORT 1111
#define IP "127.0.0.1"
#define SIZE_LENGTH 9
#define HEADER_STRING_SIZE 18

//#define SIZE (int)1e9
#define FAMILY AF_INET
#define TYPE SOCK_STREAM
#define PROTOCOL 0
#endif

using namespace std;

typedef enum { Quit, Signup, Login, Download, Upload, Share, Status, Null, Exit } Operation;



/*inline string sub(char src[], int pos, int len)
{
	string dest = "";
	for (; pos < pos + len; pos++)
		dest += src[pos];
	return dest;
}



string toStr(char* cString)
{
	string result = "";
	for (int i = 0; i < strlen(cString); i++)
		result += cString[i];
	return result;
}*/

/*uint64_t toNum(char *len)
{
	uint64_t result = 0;

	result += len[0] - '0';
	for (int i = 1; i < strlen(len); i++)
	{
		result *= 10;
		result += len[i] - '0';
	}
	return result;
}*/