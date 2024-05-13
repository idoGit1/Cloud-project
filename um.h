#pragma once
#define UM_H_
#define DATABASE_FILE "D:\\Cloud project\\Cloud\\Data\\user_list.db\0"
#define DATA_PATH "D:\\Cloud project\\Cloud\\Data\\"
#define TABLE "list"
#ifndef MY_PROJECT_HEADER_H_
#include <d:/Cloud project/Header/my_project_header.h>
#endif
#ifndef MESSAGE_H_
#include "message.h"
#endif
#ifndef USER_H_
#include "user.h"
#endif

#include <winsqlite/winsqlite3.h>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sys/stat.h>
#include <mutex>
#include <stdio.h>
namespace fs = std::filesystem;

class UM
{
private:
	User currentUser;
	string authentication;


	MainMsg quit(MainMsg &);
	MainMsg signup(MainMsg &);
	MainMsg login(MainMsg &);
	MainMsg download(MainMsg &);
	MainMsg upload(MainMsg &);
	MainMsg share(MainMsg &);
	MainMsg status(MainMsg &);
	MainMsg remove_(MainMsg &);
	void exit_(MainMsg &);
	static string generateAuthentication();


	MainMsg null();
	// Handle sqlite

	//static int callback(void *, int, char **, char **);
public:
	static void createDatabase(); // Runs only ONCE.

	UM();
	MainMsg execute(MainMsg);
	MainMsg success(Operation);
	MainMsg failure(Operation);
};