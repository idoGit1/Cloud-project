#include "um.h"

static mutex signupMtx;
static mutex loginMtx;
static vector<User> activeUsers;
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
	try
	{
		exitResult = sqlite3_open(DATABASE_FILE, &db);
		if (exitResult == SQLITE_OK)
			cerr << "Database (user list) created successfully!\n";
		else
			cerr << EXCEPTION_CODES[17];
	}
	catch (int errorCode)
	{
		cerr << EXCEPTION_CODES[errorCode];
		exit(errorCode);
	}
	// SQL query to create table where username and password are required. 
	// In addition, username is PRIMARY KEY, which means that it is unique.
	query = "CREATE TABLE list("
		"username TEXT PRIMARY KEY,"
		"password TEXT);";
	try
	{
		exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);

		if (exitResult == SQLITE_OK)
			cerr << "Users table created successfully!\n";
		else
		{
			cerr << EXCEPTION_CODES[18];
		}
	}
	catch (int errorCode)
	{
		cerr << EXCEPTION_CODES[errorCode] << "\n";
		exit(errorCode);
	}
	
}

UM::UM()
{
	authentication = BLANKAUTH;
}


MainMsg UM::success(Operation type)
{
	MainMsg success;

	success.header.type = type;
	strncpy_s(success.header.auth, 9, authentication.c_str(), 8);
	success.header.auth[8] = '\0';
	success.data = (string)SUCCESS;
	success.header.size = 1;
	return success;
}
MainMsg UM::failure(Operation type)
{
	MainMsg failure;

	failure.header.type = type;
	strncpy_s(failure.header.auth, 9, authentication.c_str(), 8);
	failure.header.auth[8] = '\0';
	failure.data = (string)FAILURE;
	failure.header.size = 1;

	return failure;
}


MainMsg UM::execute(MainMsg msg)
{
	MainMsg result;
		

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		cerr << EXCEPTION_CODES[19];
	cerr << authentication << "\n";
	switch (msg.header.type)
	{
	case Quit:
		result = quit(msg);
		break;
	case Signup:
		result = signup(msg);
		break;
	case Login:
		result = login(msg);
		break;
	case Download:
		result = download(msg);
		break;
	case Upload:
		result = upload(msg);
		break;
	case Share:
		result = share(msg);
		break;
	case Status:
		result = status(msg);
		break;
	case Remove:
		result = remove_(msg);
		break;
	case Exit:
		exit_(msg);
		break;
	default:
		cerr << EXCEPTION_CODES[16];
		break;
	}
	return result;
}


void UM::exit_(MainMsg &msg)
{
	exit(0);
}


MainMsg UM::quit(MainMsg &msg)
{
	// Erase the user from active users.
	vector<User>::iterator i = find(activeUsers.begin(), activeUsers.end(), currentUser);
	activeUsers.erase(i);
	currentUser = User();
	authentication = (string)BLANKAUTH;
	return success(msg.header.type);
}

MainMsg UM::signup(MainMsg &msg)
{
	string strMsg = msg.data; // Format: [username password]

	size_t spaceIdx = strMsg.find(" ");

	string username = strMsg.substr(0, spaceIdx);
	string password = strMsg.substr(spaceIdx + 1, strMsg.length());

	
	sqlite3 *db;
	char *errMsg;
	int exitResult;
	string query;

	signupMtx.lock();
	exitResult = sqlite3_open(DATABASE_FILE, &db);

	if (exitResult != SQLITE_OK)
	{
		cerr << "problem inside signup func.\n";
		exit(1);
	}
	query = "INSERT INTO " + (string)TABLE + 
		" VALUES('" + username + "', '" + password +
		"');";
	cerr << "Query: " << query;

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);
	signupMtx.unlock();

	if (exitResult == SQLITE_OK)
	{
		fs::path folderPath = ((string)DATA_PATH + username);
		if (fs::exists(folderPath))
		{
			cerr << "UM::singup- folder exists.";
			exit(1);
		}
		else
		{
			if (fs::create_directory(folderPath))
			{
				cerr << "Folder for user " << username << " created successfully.";
				
				fs::path shareFolderPath = (DATA_PATH + username + "\\.shared");
				
				if (!fs::create_directory(shareFolderPath))
				{
					cerr << "UM::signup cannot open share folder\n";
					exit(85);
				}
			}
			else
			{
				cerr << "UM::signup- Folder for " << username << " failed to create.";
				exit(1);
			}
		}
		return success(msg.header.type);
	}
	else
	{
		cerr << errMsg << "\n";
		return failure(msg.header.type);// Otherwise return Message that username is not unique and needs to be replaced.

	}
}

MainMsg UM::login(MainMsg &msg)
{
	User user1;
	string strMsg = msg.data; // Format: [username password]

	size_t spaceIdx = strMsg.find(" ");
	//int endUsernameIdx = spaceIdx;

	string username = strMsg.substr(0, spaceIdx);
	string password = strMsg.substr(spaceIdx + 1, strMsg.length());

	user1.username = username;
	user1.password = password;

	if (find(activeUsers.begin(), activeUsers.end(), user1) != activeUsers.end())
	{
		// Need to cerr << EXCEPTION_CODES[an error code.
		return failure(msg.header.type);
	}

	sqlite3 *db;
	char *errMsg;
	int exitResult;
	string query;
	// Protecting file accessing.
	loginMtx.lock();

		exitResult = sqlite3_open(DATABASE_FILE, &db);
		if (exitResult != SQLITE_OK)
			cerr << EXCEPTION_CODES[20];

	query = "SELECT password FROM " + (string)TABLE +
		" WHERE username = '" + user1.username + "';";

		exitResult = sqlite3_exec(db, query.c_str(), callback, 0, &errMsg);
		loginMtx.unlock();
		if (exitResult != SQLITE_OK)
			cerr << EXCEPTION_CODES[21];


	if (user1.password == SQLPassword)
	{
		currentUser = User(user1);
		// Protecting double accessing from different devices to the same user.
		activeUsers.push_back(currentUser);

		authentication = generateAuthentication();

		MainMsg respond;

		respond.data = (string)SUCCESS;
		respond.header.type = Login;
		strncpy_s(respond.header.auth, 9, authentication.c_str(), 8);
		respond.header.auth[8] = '\0';
		respond.header.size = (respond.data).size();
		return respond;
	}
	return failure(Login); // Password incorrect
	
}

MainMsg UM::download(MainMsg &msg)
{

	MainMsg result;

	string inputFilePath = (string)DATA_PATH + currentUser.username
		+ "\\" + msg.data; // "D:\\Cloud project\\Cloud\\Data\\'username'\\filename"

	ifstream file;
	file.open(inputFilePath, ios::binary);
	
	if (!file.is_open())
	{
		return failure(msg.header.type);
	}

	string fileContent(istreambuf_iterator<char>(file), {});

	file.close();

	result.header.type = Download;
	strncpy_s(result.header.auth, 9, authentication.c_str(), 8);
	result.header.auth[8] = '\0';

	result.data = msg.data + "*";
	result.data += fileContent;

	result.header.size = result.data.size();
	
	return result;
}

MainMsg UM::upload(MainMsg &msg)
{
	printf("UM::upload\n");
	size_t starPos = msg.data.find("*");
	string outFileName = msg.data.substr(0, starPos);
	string fileContent = msg.data.substr(starPos + 1, msg.data.size() - starPos);
	// Need to check if file is already exist.
	ofstream outFile(((string)DATA_PATH + currentUser.username + "\\" + outFileName), ios::binary);
	cerr << ((string)DATA_PATH + outFileName);
	// Need to check if worked.
	if (outFile)
	{
		outFile.write(fileContent.data(), fileContent.size());
		//outFile << fileContent;
		outFile.close();
		return success(msg.header.type);
	}
	return failure(msg.header.type);
}

MainMsg UM::share(MainMsg &msg)
{
	size_t starIdx = msg.data.find("*");
	
	string sharedUsername = msg.data.substr(0, starIdx);
	string fileName = msg.data.substr(starIdx + 1, msg.data.size() - starIdx - 1);

	string destUserPath = DATA_PATH + sharedUsername + "\\.shared\\" + fileName;
	string srcUserPath = DATA_PATH + currentUser.username + "\\" + currentUser.username + "_" + fileName;

	// TO DO:
	// Add in the sqlite db of the sharing user

	ifstream src(srcUserPath, ios::binary);
	ofstream dest(destUserPath, ios::binary);

	// Taken from stack overflow:

	copy(istreambuf_iterator<char>(src),
		istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(dest));

	return success(msg.header.type);

}

MainMsg UM::status(MainMsg &msg)
{
	printf("UM::status\n");
	MainMsg respond;

	string path = (string)DATA_PATH + currentUser.username; // The directory with the user's files.
	string data = "";
	fs::path fileName;
	string fileNameStr;
	bool hasFiles = false;
	for (const fs::directory_entry &entry : fs::directory_iterator(path))
	{
		hasFiles = true;
		fileName = entry.path();
		fileNameStr = fileName.string();

		// Remove the starting - D:\\Cloud project...

		size_t perffixLen = (path + "\\").size();
		data += "\t";
		data += fileNameStr.substr(perffixLen, fileNameStr.size() - perffixLen);
		data += "\n";
	}

	if (!hasFiles)
	{
		respond.header.type = Status;
		strncpy_s(respond.header.auth, 9, authentication.c_str(), 8);
		respond.header.auth[8] = '\0';

		respond.data = (string)SUCCESS;

		respond.header.size = 1;
		return respond;
	}

	respond.header.type = Status;
	strncpy_s(respond.header.auth, 9, authentication.c_str(), 8);
	respond.header.auth[8] = '\0';

	respond.data = (string)data;

	respond.header.size = respond.data.size();

	return respond;
}

MainMsg UM::remove_(MainMsg &msg)
{
	MainMsg respond;

	string path = (string)DATA_PATH + currentUser.username; // The directory with the user's files.
	string fileName = (string)msg.data;
	path += "\\" + fileName;
	if (remove(path.c_str()) == 0)
		return success(msg.header.type);
	else
		return failure(msg.header.type);
}



string UM::generateAuthentication()
{
	// Taken from geeksforgeeks.
	mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

	static const char alphaNum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string auth;
	auth.reserve(8);

	for (int i = 0; i < 8; ++i) {
		auth += alphaNum[rng() % (sizeof(alphaNum) - 1)];
	}
	return auth;
}


