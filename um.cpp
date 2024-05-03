#include "um.h"

static string SQLPassword;
int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	//fprintf(stderr, "%s: ", (const char *)data);

	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		printf("Col name: %s\n", azColName[i]);
		printf("got into callback()\n");
		SQLPassword = (string)argv[i];
	}

	printf("\n");
	return 0;
}

void UM::createDatabase()
{
	sqlite3 *db;
	string query;

	char *errMsg;
	int exitResult;

	exitResult = sqlite3_open(DATABASE_FILE, &db);
	if (exitResult == SQLITE_OK)
		cerr << "Database (user list) created successfully!\n";
	else
	{
		cerr << "Failed to create database (user list)\n";
		exit(1);
	}
	// SQL query to create table where username and password are required. 
	// In addition, username is PRIMARY KEY, which means that it is unique.
	query = "CREATE TABLE list("
		"username TEXT PRIMARY KEY,"
		"password TEXT);";

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);

	if (exitResult == SQLITE_OK)
		cerr << "Users table created successfully!\n";
	else
	{
		cerr << "Failed to create users table.\n" << (string)errMsg;
		exit(1);
	}
}

UM::UM()
{
	authentication = BLANKAUTH;
}


main_msg UM::success(Operation type)
{
	main_msg success;
	memset(&success, 0, sizeof(main_msg));
	success.header.type = type;
	strncpy_s(success.header.auth, 9, BLANKAUTH, 8);
	success.header.auth[8] = '\0';
	success.data = (string)SUCCESS;
	success.header.size = 1;
	return success;
}
main_msg UM::failure(Operation type)
{
	main_msg failure;
	memset(&failure, 0, sizeof(main_msg));
	failure.header.type = type;
	strncpy_s(failure.header.auth, 9, BLANKAUTH, 8);
	failure.header.auth[8] = '\0';
	failure.data = (string)FAILURE;
	failure.header.size = 1;

	return failure;
}


main_msg UM::execute(main_msg msg)
{
	//typedef enum { Logout, Signup, Login, Download, Upload, Share, Status} Operation;
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
	{
		cerr << "UM::execute- Authentication failed\n";
		return failure(msg.header.type);
	}
	switch (msg.header.type)
	{
	case Quit:
		return quit(msg);
	case Signup:
		return signup(msg);
	case Login:
		return login(msg);
	case Download:
		return download(msg);
	case Upload:
		return upload(msg);
	case Share:
		return share(msg);
	case Status:
		return status(msg);
	case Exit:
		exit_(msg);
	default:
		exit(1);
	}

}


void UM::exit_(main_msg msg)
{
	exit(0);
}


main_msg UM::quit(main_msg msg)
{
	currentUser = User();
	authentication = (string)BLANKAUTH;

	return success(msg.header.type);
}

main_msg UM::signup(main_msg msg)
{
	string strMsg = msg.data; // Format: [username password]

	size_t spaceIdx = strMsg.find(" ");

	string username = strMsg.substr(0, spaceIdx);
	string password = strMsg.substr(spaceIdx + 1, strMsg.length());

	currentUser.username = username;
	currentUser.password = password;

	// TO DO: check if username is unique:
	sqlite3 *db;
	char *errMsg;
	int exitResult;
	string query;
	// ADD MUTEX - FILE PROTECTION
	exitResult = sqlite3_open(DATABASE_FILE, &db);

	if (exitResult != SQLITE_OK)
	{
		cerr << "problem inside signup func.\n";
		exit(1);
	}
	query = "INSERT INTO " + (string)TABLE + 
		" VALUES('" + currentUser.username + "', '" + currentUser.password +
		"');";
	cerr << "Query: " << query;

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);

	if (exitResult == SQLITE_OK)
	{
		return success(msg.header.type);
	}
	else
	{
		cerr << errMsg << "\n";
		return failure(msg.header.type);// Otherwise return Message that username is not unique and needs to be replaced.

	}
}


main_msg UM::null()
{
	main_msg Null;
	memset(&Null, 0, sizeof(main_msg));
	return Null;
}


main_msg UM::login(main_msg msg)
{
	User user1;
	string strMsg = msg.data; // Format: [username password]

	size_t spaceIdx = strMsg.find(" ");
	//int endUsernameIdx = spaceIdx;

	string username = strMsg.substr(0, spaceIdx);
	string password = strMsg.substr(spaceIdx + 1, strMsg.length());

	user1.username = username;
	user1.password = password;


	// TO DO: check if user is in users list:
		// Then return auth code
		// Otherwise return failure
	sqlite3 *db;
	char *errMsg;
	int exitResult;
	string query;
	// ADD MUTEX - FILE PROTECTION
	exitResult = sqlite3_open(DATABASE_FILE, &db);

	if (exitResult != SQLITE_OK)
	{
		cerr << "problem inside login func.\n";
		exit(1);
	}
	query = "SELECT password FROM " + (string)TABLE +
		" WHERE username = '" + user1.username + "';";

	exitResult = sqlite3_exec(db, query.c_str(), callback, 0, &errMsg);

	if (exitResult != SQLITE_OK)
		return failure(msg.header.type);

	if (user1.password == SQLPassword)
	{
		currentUser = User(user1);
		authentication = generateAuthentication();

		main_msg respond;
		memset(&respond, 0, sizeof(main_msg));
		//respond.data = authentication;
		respond.data = (string)SUCCESS;
		respond.header.type = Login;
		strncpy_s(respond.header.auth, 9, authentication.c_str(), 8);
		respond.header.auth[8] = '\0';
		respond.header.size = (respond.data).size();
		return respond;
	}
	return failure(Login); // Password incorrect
	
}

main_msg UM::download(main_msg msg)
{
	// TO DO: 
			// 2.1 if file cannot be located return failure
			// 2.2 if file is located, send it.
	return null();
}

main_msg UM::upload(main_msg msg)
{
	
		// YES: locate directory and create + upload file, return success
		// NO: return failure
	return null();
}

main_msg UM::share(main_msg msg)
{
	return null();
}

main_msg UM::status(main_msg msg)
{
	return null();
	// TO DO:
}


string UM::generateAuthentication()
{
	mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string auth;
	auth.reserve(8);

	for (int i = 0; i < 8; ++i) {
		auth += alphanum[rng() % (sizeof(alphanum) - 1)];
	}
	return auth;
}