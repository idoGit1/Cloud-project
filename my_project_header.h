#pragma once
#ifndef MY_PROJECT_HEADER_H_
#undef UNICODE
#define MY_PROJECT_HEADER_H_
#define WIN32_LEAN_AND_MEAN


#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstdint>
#include <exception>
#include <string>
#define PORT 1111
#define SIZE_LENGTH 9
#define HEADER_STRING_SIZE 19
#define IP "127.0.0.1"
#define MAX_FILE_SIZE 900000000

typedef enum {	Quit,
				Signup,
				Login,
				Download,
				Upload,
				Share,
				Status,
				RemoveUser,
				Remove,
				StopShare } Operation;

std::string configLocation(const char *funcName, const char *fileName);

#ifndef MY_LOCATION
#define MY_LOCATION configLocation(__FUNCTION__, __FILE__)
#endif // MY_LOCATION
class FileHandlingError : public std::exception
{
private:
	std::string message;
	std::string location;
public:
	FileHandlingError(const char *msg, std::string location)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}
};

class ContentError : public std::exception
{
private:
	std::string message;
	std::string location;
public:
	ContentError(const char *msg, std::string location)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}
};

class SqlError : public std::exception
{
private:
	std::string message;
	std::string location;
public:
	SqlError(const char *msg, std::string location)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}
};

class CommunicationError : public std::exception
{
private:
	std::string message;
	std::string location;
public:
	CommunicationError(const char *msg, std::string location)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}
};

/*const std::string EXCEPTION_CODES[] = {
"Success",
// Communication errors:
"1: Communication error -- Server::build - Winsock initialization failed.",
"2: Communication error -- Server::encode - Message is too long.",
"3: Communication error -- Server::acceptClient - problem accepting client.",
"4: Communication error -- Server::build - problem creating the listen socket.",
"5: Communication error -- Server::build - problem binding socket.",
"6: Communication error -- Server::build - problem with listening for connections",
"7: Communication error -- Server::receive - problem with receiving info.",
"8: Communication error -- Server::snd - problem sending info.",

"9: Communication error -- Client::Client - Winsock initialization failed.",
"10: Communication error -- Client::Client - problem creating a socket.",
"11: Communication error -- Client::Client - problem with conversion of server address.",
"12: Communication error -- Client::Client - problem connecting socket to server.",
"13: Communication error -- Client::snd - problem sending info.",
"14: Communication error -- Client::receive - problem with receiving info.",
"15: Communication error -- Client::encode - Message is too long.",
// Other:
"16: UM::execute - unknown command type.",
"17: UM::createDatabase - error creating the database.",
"18: UM::createDatabase - Failed to create users table.",
"19: UM::execute- Authentication failed.",
"20: UM::login - cannot open database.",
"21: UM::login - fail to achieve password from db.",

"22: UI::remove - wrong authentication code from server.",
"23: UI::upload - wrong authentication code from server.",
"24: UI::download - wrong authentication code from server.",
"25: UI::status - wrong authentication code from server.",
"26: UI::quit - wrong authentication code from server.",
"27: UI::signup - wrong authentication code from server.",
"28: UI::login - wrong authentication code from server.",
"29: UI::share - wrong authentication code from server."

};*/

#endif
