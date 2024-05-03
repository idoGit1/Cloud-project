#pragma once
#define UM_H_
#define DATABASE_FILE "user_list.db\0"
#define TABLE "list"
#ifndef COMMON_H_
#include "common.h"
#endif
#ifndef MESSAGE_H_
#include "message.h"
#endif
#ifndef USER_H_
#include "user.h"
#endif
#ifndef SQLITE3_H_
#include <winsqlite/winsqlite3.h>
#endif
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
class UM
{
private:
	User currentUser;
	string authentication;


	main_msg quit(main_msg);
	main_msg signup(main_msg);
	main_msg login(main_msg);
	main_msg download(main_msg);
	main_msg upload(main_msg);
	main_msg share(main_msg);
	main_msg status(main_msg);
	void exit_(main_msg);
	static string generateAuthentication();

	main_msg success(Operation);
	main_msg failure(Operation);
	main_msg null();
	// Handle sqlite

	//static int callback(void *, int, char **, char **);
public:
	static void createDatabase(); // Runs only ONCE.

	UM();
	main_msg execute(main_msg);

};