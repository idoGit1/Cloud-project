#pragma once
#ifndef DATA_MANAGER_H_
#define DATA_MANAGER_H_
#include "my_project_header.h"
#include "user.h"
#include <winsqlite/winsqlite3.h>
#include <filesystem>
class DataManager
{
	static std::mutex signupMtx;
	static std::mutex loginMtx;
	std::string tmpSqlPassword;
	std::vector<std::pair<std::string, std::string>> tmpList;

public:
	DataManager();

	static const std::string DATA_PATH;
	static const std::string DATABASE_FILE;
	static void createDatabase();
	static int callback(void *data, int argc, char **argv, char **azColName);
	static int getSharingListCallback(void *data, int argc, char **argv, char **azColName);


	std::string registeredUsersList(); // All users
	bool isPasswordCorrect(const User &user);
	std::string mySharingList(const std::string username); // What I'm sharing

	bool unShare(const std::string &fileName, const std::string &destUsername,
		const User &srcUser);

	std::string downloadFile(const std::string &fileName, const std::string &username);

	void createUserFolder(const User &);
	void createSharedWithMeFolder(const User &);
	void createSharingDBTable(const User &);
	void updateUsersTable(const User &);
	std::string getFilesInFolder(std::string &path); // without files that start with '.

	std::string getStatusAdmin();

	void removeUser(const std::string &);
	void deleteFolder(const std::string &);
	bool shareFile(const std::string &destUsername, const User &srcUser,
		const std::string &fileName);
};

#endif
