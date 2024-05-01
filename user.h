#pragma once
#define USER_H_
#ifndef COMMON_H_
#include "common.h"
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