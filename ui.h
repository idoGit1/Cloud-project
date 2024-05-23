#pragma once
#ifndef UI_H_
#define UI_H_

#include "my_project_header.h"
#include "client.h"
#include "message.h"
#include "user.h"

#include <vector>
#include <string>


class UI
{
	User currentUser;
	std::string authentication;
	Client client;
	//bool readFile(vector<string> &, string);
public:
	UI();
	~UI();

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
	void run(const std::string &ip = IP);

};

#endif // UI_H_