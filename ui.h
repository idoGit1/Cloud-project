#pragma once
#define UI_H_
#ifndef COMMON_H_
#include "common.h"
#endif
#ifndef CLIENT_H_
#include "client.h"
#endif
#ifndef MESSAGE_H_
#include "message.h"
#endif
#ifndef USER_H_
#include "user.h"
#endif
#include <vector>

using namespace std::this_thread;
using namespace std::chrono;

class UI
{
	User currentUser;
	string authentication;

	//bool readFile(vector<string> &, string);
public:
	UI();
	~UI();
	void run();

};

