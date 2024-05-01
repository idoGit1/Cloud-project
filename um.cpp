#include "um.h"

User user;

int UM::callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	//fprintf(stderr, "%s: ", (const char *)data);

	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		printf("Col name: %s\n", azColName[i]);
		if (strncmp(azColName[i], "password", 9) == 0)
		{
			user.password = argv[i];
		}
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


Message* UM::success(Operation type)
{
	Message* success = new Message();
	success->setType(type);
	success->setAuth(BLANKAUTH);
	success->setData(SUCCESS);

	return success;
}
Message* UM::failure(Operation type)
{
	Message* failure = new Message();
	failure->setType(type);
	failure->setAuth(BLANKAUTH);
	failure->setData(FAILURE);

	return failure;
}


Message* UM::execute(Message* msg)
{
	//typedef enum { Logout, Signup, Login, Download, Upload, Share, Status} Operation;
	if (msg->getAuth() != authentication)
	{
		cerr << "Authenticatoin failed";
		return failure(msg->getType());
	}
	switch (msg->getType())
	{
	case Logout:
		return logout(msg);
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
	default:
		exit(1);
	}

}


Message* UM::logout(Message* msg)
{
	currentUser.username = "";
	currentUser.password = "";

	return success(msg->getType());
}

Message* UM::signup(Message* msg)
{
	string strMsg = msg->getData(); // Format: [username password]

	int spaceIdx = strMsg.find(" ");
	int endUsernameIdx = spaceIdx;

	string username = strMsg.substr(0, endUsernameIdx);
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
		"VALUES('" + (string)currentUser.username + "', '" + (string)currentUser.password +
		"');";

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);

	if (exitResult == SQLITE_OK)
	{
		return success(msg->getType());
	}
	else
	{
		cerr << errMsg << "\n";
		return failure(msg->getType());// Otherwise return Message that username is not unique and needs to be replaced.

	}
}

Message* UM::login(Message* msg)
{
	User user1;
	string strMsg = msg->getData(); // Format: [username password]

	int spaceIdx = strMsg.find(" ");
	int endUsernameIdx = spaceIdx;

	string username = strMsg.substr(0, endUsernameIdx);
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
	query = "SELECT * FROM " + (string)TABLE +
		"WHERE username = '" + user.username + "';";

	exitResult = sqlite3_exec(db, query.c_str(), callback, 0, &errMsg);

	if (exitResult != SQLITE_OK)
		return failure(msg->getType());

	if (user1.password == user.password)
	{
		currentUser = User(user1);
		authentication = generateAuthentication();

		Message *respond = new Message();
		
		respond->setData(authentication);
		respond->setType(Login);
		respond->setAuth(authentication);
		return respond;
	}
	return failure(Login); // Password incorrect
	
}

Message* UM::download(Message* msg)
{
	// TO DO: 
			// 2.1 if file cannot be located return failure
			// 2.2 if file is located, send it.
	return NULL;
}

Message* UM::upload(Message* msg)
{
	
		// YES: locate directory and create + upload file, return success
		// NO: return failure
	return NULL;
}

Message* UM::share(Message* msg)
{
	return NULL;
}

Message* UM::status(Message* msg)
{
	return NULL;
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