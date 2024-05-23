#pragma once
#ifndef UM_H_
#define UM_H_
//#define DataManagerBASE_FILE "D:\\Cloud project\\Cloud\\DataManager\\user_list.db\0"
//#define DataManager_PATH "D:\\Cloud project\\Cloud\\DataManager\\"
#define TABLE "list"
#include "my_project_header.h"
#include "message.h"
#include "user.h"
#include "data_manager.h"

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
	//static std::mutex signupMtx;
	//static std::mutex loginMtx;
	static std::vector<User> activeUsers;
	DataManager dataManager;
	User currentUser;
	std::string authentication;
	// File system and sql.
	//void createUserFolder(const User &);
	///void createSharedWithMeFolder(const User &);
	//void createSharingDBTable(const User &);
	//void updateUsersTable(const User &);
	//bool isPasswordCorrect(const User &);

	MainMsg quit(MainMsg &);
	MainMsg signup(MainMsg &);
	MainMsg login(MainMsg &);
	MainMsg download(MainMsg &);
	MainMsg upload(MainMsg &);
	MainMsg share(MainMsg &);
	MainMsg status(MainMsg &);
	MainMsg remove_(MainMsg &);
	MainMsg removeUser(MainMsg &);
	void exit_(MainMsg &);
	static std::string generateAuthentication();
	//std::string getFilesInFolder(std::string &); // without files that begin with a '.

	//MainMsg null();
	// Handle sqlite

	//static int callback(void *, int, char **, char **);




public:
	static void createDatabase(); // Runs only ONCE.

	UM();
	MainMsg execute(MainMsg);
	MainMsg success(Operation, std::string);
	MainMsg failure(Operation, std::string);
};

#endif