#pragma once
#ifndef UI_H_
#define UI_H_

#include "d:/Cloud project/Additional/my_project_header.h"
#include "client.h"
#include "d:/Cloud project/Additional/message.h"
#include "d:/Cloud project/Additional/user.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>

class UI
{
	User currentUser;
	std::string authentication;
	Client client;
	int loginAttemptsCounter; // Against dos attack
	int signupsCounter; // Against dos attack

	// Command functions
	void help();
	void login();
	void signup();
	void quit();
	void exit_();
	void status();
	void download(std::string &);
	void upload(std::string &);
	void share(std::string &);
	void remove(std::string &);
	void removeUser(std::string &);
public:
	UI();
	void run();

};

#endif 