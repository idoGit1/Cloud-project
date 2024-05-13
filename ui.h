#pragma once
#define UI_H_
#ifndef MY_PROJECT_HEADER_H_
#include <d:/Cloud project/Header/my_project_header.h>
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
#include <string>
using namespace std::this_thread;
using namespace std::chrono;

class UI
{
	User currentUser;
	string authentication;
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
	void download(string &);
	void upload(string &);
	void share(string &);
	void remove(string &);
	void run();

};

