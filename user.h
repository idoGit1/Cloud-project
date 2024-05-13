#pragma once
#define USER_H_
#ifndef MY_PROJECT_HEADER_H_
#include <d:/Cloud project/Header/my_project_header.h>
#endif
struct User
{
public:
	string username;
	string password;

	bool operator==(User const &);
	User();

	explicit User(const User &);

	string toString();

	bool operator <(const User &obj) const noexcept
	{
		return (username.compare(obj.username) < 0) ? false : true;
	}

};