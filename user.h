#pragma once
#define USER_H_
#ifndef MY_PROJECT_HEADER_H_
#include <d:/Cloud project/Header/my_project_header.h>
#endif
class User
{
public:
	string username;
	string password;

	bool operator==(User const &);
	User();

	User(User &);

	string toString();
};