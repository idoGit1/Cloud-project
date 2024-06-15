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
	void download(const std::string &);
	void upload(const std::string &);
	void share(const std::string &);
	void remove(const std::string &);
	void removeUser(const std::string &);

	bool handleNoArgCommands(const std::string &);

	bool handle1ArgCommands(const std::string &);

	std::string getFileNameFromPath(const std::string &);

	std::string inputCommand();

	bool handleCommand(const std::string &);
public:
	UI();
	void run();

};

namespace ManageFiles
{
	// For managing files and data on user side
	void stringIntoFile(const std::string &, const std::string &);
	std::string fileIntoString(const std::string &);
}

#endif 