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
#include <vector>

#define PORT 1111
#define SIZE_LENGTH 9
#define HEADER_STRING_SIZE 20
#define IP "127.0.0.1"
#define EXTERN_IP "10.100.102.25"
#define MAX_FILE_SIZE 900000000

typedef enum {	Null,
				Exit,
				Quit,
				Signup,
				Login,
				Download,
				Upload,
				Share,
				Status,
				RemoveUser,
				Remove,
				StopShare,
				} Operation;

std::string configLocation(const char *funcName, const char *fileName);

#ifndef MY_LOCATION
#define MY_LOCATION configLocation(__FUNCTION__, __FILE__)
#endif // MY_LOCATION
class FileHandlingError : public std::exception
{
private:
	std::string message;
	std::string location;
	std::string messageForUser;
public:
	FileHandlingError(const char *msg, std::string location, const char *msgForUser = NULL)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
		if (msgForUser == NULL)
			messageForUser = msg;
		else
			messageForUser = msgForUser;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}

	const char *whatForUser() const throw()
	{
		return messageForUser.c_str();
	}
};

class ContentError : public std::exception
{
private:
	std::string message;
	std::string location;
	std::string messageForUser;
public:
	ContentError(const char *msg, std::string location, const char *msgForUser = NULL)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
		if (msgForUser == NULL)
			messageForUser = msg;
		else
			messageForUser = msgForUser;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}

	const char *whatForUser() const throw()
	{
		return messageForUser.c_str();
	}
};

class SqlError : public std::exception
{
private:
	std::string message;
	std::string location;
	std::string messageForUser;
public:
	SqlError(const char *msg, std::string location, const char *msgForUser = NULL)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
		if (msgForUser == NULL)
			messageForUser = msg;
		else
			messageForUser = msgForUser;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}

	const char *whatForUser() const throw()
	{
		return messageForUser.c_str();
	}
};

class CommunicationError : public std::exception
{
private:
	std::string message;
	std::string location;
	std::string messageForUser;
public:
	CommunicationError(const char *msg, std::string location, const char *msgForUser = NULL)
	{
		message = location + " - " + msg + "\n";
		this->location = location;
		if (msgForUser == NULL)
			messageForUser = msg;
		else
			messageForUser = msgForUser;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}

	const char *whatForUser() const throw()
	{
		return messageForUser.c_str();
	}
};


class UserInputError : public std::exception
{
private:
	std::string message;
	std::string messageForUser;
public:
	UserInputError(const char *msg,const char *msgForUser = NULL)
	{
		message = msg;
		if (msgForUser == NULL)
			messageForUser = msg;
		else
			messageForUser = msgForUser;
	}

	const char *what() const throw()
	{
		return message.c_str();
	}

	const char *whatForUser() const throw()
	{
		return messageForUser.c_str();
	}
};

namespace Local
{
	std::string toStringFillZeroes(int number, int totalStringSize = 9);
	void removeStartEndSpaces(std::string &str);
	template<class T>
	void addToSortedVector(std::vector<T> &vec, const T element)
	{
		int low = 0;
		int high = vec.size() - 1;
		int mid;

		if (vec.size() == 0)
		{
			vec.push_back(element);
			return;
		}

		while (low <= high)
		{
			mid = (low + high) / 2;

			if (vec[mid] == element)
			{
				vec.insert(vec.begin() + mid, element);
				break;
			}
			else if (vec[mid] < element && (mid == high || vec[mid + 1] >= element))
			{
				vec.insert(vec.begin() + mid + 1, element);
				break;
			}
			else if (vec[mid] > element && (mid == low || vec[mid - 1] <= element))
			{
				vec.insert(vec.begin() + mid, element);
				break;
			}

			if (vec[mid] < element)
				low = mid + 1;
			if (vec[mid] > element)
				high = mid - 1;

		}
	}

	template<class T>
	int binarySearchInVector(const std::vector<T> &vec, const T &element)
	{
		int low = 0;
		int high = vec.size() - 1;
		int mid;

		while (low <= high)
		{
			mid = (low + high) / 2;

			if (vec[mid] == element)
				return mid;

			if (vec[mid] < element)
				low = mid + 1;
			if (element < vec[mid])
				high = mid - 1;

		}
		return -1;
	}
}

#endif
