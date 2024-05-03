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
class UI
{
	User currentUser;
	string authentication;
public:
	UI();
	~UI();
	void run();

};

