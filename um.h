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


	Message* logout(Message*);
	Message* signup(Message*);
	Message* login(Message*);
	Message* download(Message*);
	Message* upload(Message*);
	Message* share(Message*);
	Message* status(Message*);

	static string generateAuthentication();

	Message *success(Operation);
	Message *failure(Operation);

	// Handle sqlite

	static int callback(void *, int, char **, char **);
public:
	static void createDatabase(); // Runs only ONCE.

	UM();



	Message* execute(Message*);




};