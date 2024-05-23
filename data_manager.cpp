#include "data_manager.h"

std::mutex DataManager::signupMtx;
std::mutex DataManager::loginMtx;
const std::string DataManager::DATA_PATH = "D:\\Cloud project\\Cloud\\Data\\";
const std::string DataManager::DATABASE_FILE =
"D:\\Cloud project\\Cloud\\Data\\user_list.db";



DataManager::DataManager()
{
	tmpSqlPassword = "";
	tmpList.clear();
}



void DataManager::createDatabase()
{
	// Only in this specific function, because there is a great use.
	using namespace std;

	sqlite3 *db;
	string query;

	char *errMsg;
	int exitResult;

	exitResult = sqlite3_open(DATABASE_FILE.c_str(), &db);
	if (exitResult != SQLITE_OK)
		throw SqlError("Cannot create database file", MY_LOCATION);
	// SQL query to create table where username and password are required. 
	// In addition, username is PRIMARY KEY, which means that it is unique.
	query = "CREATE TABLE list("
		"username TEXT PRIMARY KEY,"
		"password TEXT);";

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);
	sqlite3_close(db);
	if (exitResult != SQLITE_OK)
		throw SqlError("Cannot open table in the database", MY_LOCATION);
}


std::string DataManager::registeredUsersList()
{
	sqlite3 *usersList;
	int exitResult;
	std::string path = DATABASE_FILE;
	std::string data = "";
	exitResult = sqlite3_open(path.c_str(), &usersList);

	if (exitResult != SQLITE_OK)
		throw SqlError((std::format("Cannot open the database {}.", path)).c_str()
			, MY_LOCATION);
	std::string query = "SELECT * FROM list";
	DataManager::tmpList.clear();

	exitResult = sqlite3_exec(usersList, query.c_str(), getSharingListCallback, this, NULL);
	sqlite3_close(usersList);
	if (exitResult != SQLITE_OK)
		throw SqlError((std::format("Cannot preform the database {}.", query)).c_str()
			, MY_LOCATION);
	int cnt = 1;
	std::string pathUser;
	std::string sharedWithMePath;
	std::vector<std::pair<std::string, std::string>> cpy = tmpList;
	for (auto &i : cpy)
	{
		data += std::format("{}. Username: {} - Password: {}\n", std::to_string(cnt),
			i.first, i.second);
		pathUser = DATA_PATH + i.first;
		sharedWithMePath = pathUser + "\\.shared";
		data += getFilesInFolder(pathUser) + "\n";
		data += getFilesInFolder(sharedWithMePath) + "\n";
		data += mySharingList(i.first) + "\n";
		cnt++;
	}
	return data;
}



bool DataManager::isPasswordCorrect(const User &user)
{
	sqlite3 *db;
	char *errMsg;
	int exitResult;
	std::string query;
	// Protecting file accessing.
	loginMtx.lock();

	exitResult = sqlite3_open(DATABASE_FILE.c_str(), &db);
	if (exitResult != SQLITE_OK)
		throw SqlError("Cannot open user_list.db", MY_LOCATION);

	query = "SELECT password FROM list"
		" WHERE username = '" + user.username + "';";

	exitResult = sqlite3_exec(db, query.c_str(), callback, this, &errMsg);
	sqlite3_close(db);
	loginMtx.unlock();
	if (exitResult != SQLITE_OK)
	{
		std::string errMsg;
		errMsg = "user_list.db cannot preform the query\n" + query;
		throw SqlError(errMsg.c_str(), MY_LOCATION);
	}
	return user.password == tmpSqlPassword;
}



std::string DataManager::mySharingList(const std::string username)
{
	sqlite3 *mySharings;
	int exitResult;
	std::string path = DATA_PATH + username + "\\.sharing.db";

	std::string data = "";
	exitResult = sqlite3_open(path.c_str(), &mySharings);

	if (exitResult != SQLITE_OK)
		throw SqlError((std::format("Cannot open the database {}.", path)).c_str()
			, MY_LOCATION);
	std::string query = "SELECT * FROM sharing";
	DataManager::tmpList.clear();

	exitResult = sqlite3_exec(mySharings, query.c_str(), getSharingListCallback, this, NULL);
	sqlite3_close(mySharings);
	if (exitResult != SQLITE_OK)
		throw SqlError((std::format("Cannot preform the database {}.", query)).c_str()
			, MY_LOCATION);
	int cnt = 1;
	for (auto &i : tmpList)
	{
		data += std::format("{}. User: {} - File: {}\n", std::to_string(cnt),
			i.first, i.second);
		cnt++;
	}
	return data;
}

void DataManager::deleteFolder(const std::string &username)
{
	namespace fs = std::filesystem;
	fs::path path = fs::path(DATA_PATH) / username;


	try
	{
		fs::remove_all(path);
	}
	catch (const fs::filesystem_error &ex)
	{
		throw FileHandlingError(("Filesystem error: " + (std::string)ex.what()).c_str(),
			MY_LOCATION);
	}
	catch (const std::exception &ex)
	{
		throw FileHandlingError(("Filesystem error: " + (std::string)ex.what()).c_str(),
			MY_LOCATION);
	}

}


void DataManager::removeUser(const std::string &username)
{
	using std::string;
	sqlite3 *db;
	int exitResult;
	exitResult = sqlite3_open(DATABASE_FILE.c_str(), &db);
	char *errMsg;
	if (exitResult != SQLITE_OK)
		throw SqlError("Cannot open database file", MY_LOCATION);

	string query = "DELETE FROM list "
		"WHERE username='" + username + "';";

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);
	sqlite3_close(db);
	if (exitResult != SQLITE_OK)
		throw SqlError(errMsg, MY_LOCATION);
}


std::string DataManager::downloadFile(const std::string &fileName, const std::string &username)
{
	std::string path;

	std::ifstream file;

	if (fileName[0] == '.') // Shared
		path = DATA_PATH + username + "\\.shared\\" + fileName.substr(1, fileName.size() - 1);
	else
		path = DATA_PATH + username + "\\" + fileName;


	file.open(path, std::ios::binary);

	if (!file.is_open())
		throw FileHandlingError("Cannot open file", MY_LOCATION);

	std::string fileContent(std::istreambuf_iterator<char>(file), {});

	file.close();

	return fileContent;
}


std::string DataManager::getFilesInFolder(std::string &path) // without files that start with '.
{
	namespace fs = std::filesystem;
	std::string data = "";
	std::filesystem::path fileName;
	std::string fileNameStr;
	bool hasFiles = false;
	size_t prefixLen = (path + "\\").size();
	std::string fileNameNoPrefix;
	for (const fs::directory_entry &entry : fs::directory_iterator(path))
	{
		hasFiles = true;
		fileName = entry.path();
		fileNameStr = fileName.string();

		// Remove the starting - D:\\Cloud project...
		fileNameNoPrefix = fileNameStr.substr(prefixLen, fileNameStr.size() - prefixLen);
		if (fileNameNoPrefix[0] == '.') // My hidden files.
			continue;
		data += "\t";
		data += fileNameNoPrefix;
		data += "\n";
	}
	return data;
}

std::string DataManager::getStatusAdmin()
{
	;
	return "";
}


bool DataManager::shareFile(const std::string &destUsername, const User &srcUser,
	const std::string &fileName)
{
	using namespace std;

	string destUserPath = DATA_PATH + destUsername + "\\.shared\\" + srcUser.username + "_" + fileName;
	string srcUserPath = DATA_PATH + srcUser.username + "\\" + fileName;

	if (!std::filesystem::exists(srcUserPath))
		throw FileHandlingError("No such file", MY_LOCATION);

	sqlite3 *srcSharingList;
	char *errMsg;
	int exitResult;
	string query;
	// Opening the .sharing.db file, which holds the list of users and files that
	// the src user shares.
	exitResult = sqlite3_open((DATA_PATH + srcUser.username
		+ "\\.sharing.db").c_str(), &srcSharingList);

	if (exitResult != SQLITE_OK)
		throw SqlError("Cannot open sharing.db", MY_LOCATION);

	// Inserting the new user and the file.

	query = "INSERT INTO sharing VALUES('" + destUsername + "','" + fileName
		+ "');";

	exitResult = sqlite3_exec(srcSharingList, query.c_str(), NULL, 0, &errMsg);
	sqlite3_close(srcSharingList);
	if (exitResult != SQLITE_OK)
	{
		string msgErr;
		msgErr = format("sharing.db cannot preform the query:\n{}", query);
		throw SqlError(msgErr.c_str(), MY_LOCATION);
	}
	// Copying the files from the src user folder
	//  to the dest user .shared folder. Taken from Stack overflow.

	ifstream src(srcUserPath, ios::binary);
	ofstream dest(destUserPath, ios::binary);
	copy(istreambuf_iterator<char>(src),
		istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(dest));

	return true;
}


int DataManager::callback(void *data, int argc, char **argv, char **azColName)
{
	int i = 0;
	//fprintf(stderr, "%s: ", (const char *)data);
	DataManager *me = reinterpret_cast<DataManager *>(data);
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		printf("Col name: %s\n", azColName[i]);
		printf("got into callback()\n");
		me->tmpSqlPassword = (std::string)(argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}


int DataManager::getSharingListCallback(void *data, int argc, char **argv, char **azColName)
{
	DataManager *me = reinterpret_cast<DataManager *>(data);

	me->tmpList.push_back({ argv[0], argv[1] });
	return 0;
}




void DataManager::createUserFolder(const User &user)
{
	namespace fs = std::filesystem;
	fs::path folderPath = (DATA_PATH + user.username);
	bool isGood;
	if (fs::exists(folderPath))
	{
		std::string msgErr;
		msgErr = std::format("Folder: {} already exists.", user.username);
		throw FileHandlingError(msgErr.c_str(), MY_LOCATION);
	}
	isGood = fs::create_directory(folderPath);
	if (!isGood)
	{
		std::string msgErr;
		msgErr = std::format("Cannot create the folder: {}.", user.username);
		throw FileHandlingError(msgErr.c_str(), MY_LOCATION);
	}
}

void DataManager::createSharedWithMeFolder(const User &user)
{
	namespace fs = std::filesystem;
	bool isGood;
	fs::path shareWithMeFolderPath = (DATA_PATH + user.username + "\\.shared"); // With me

	isGood = fs::create_directory(shareWithMeFolderPath);

	if (!isGood)
	{
		std::string msgErr;
		msgErr = std::format("Cannot create the .shared folder for user"
			" {}.", user.username);
		throw FileHandlingError(msgErr.c_str(), MY_LOCATION);
	}
}


void DataManager::createSharingDBTable(const User &user)
{
	namespace fs = std::filesystem;

	char *errMsg;
	int result = -1;
	std::string query{};
	sqlite3 *sharingList; // The files the user shares with others.
	result = sqlite3_open((DATA_PATH + user.username + "\\.sharing.db").c_str(), &sharingList);

	if (result != SQLITE_OK)
	{
		sqlite3_close(sharingList);
		throw SqlError(".sharing.db cannot open.", MY_LOCATION);
	}
	query = "CREATE TABLE sharing("
		"username TEXT,"
		"fileName TEXT); ";
	result = sqlite3_exec(sharingList, query.c_str(), NULL, 0, &errMsg);

	if (result != SQLITE_OK)
	{
		sqlite3_close(sharingList);
		throw SqlError(errMsg, MY_LOCATION);
	}

	sqlite3_close(sharingList);

}


void DataManager::updateUsersTable(const User &user)
{
	int exitResult = 0;
	sqlite3 *db;
	std::string query{};
	signupMtx.lock(); // Guarding the userlist.db file.
	char *errMsg;
	exitResult = sqlite3_open(DATABASE_FILE.c_str(), &db);

	if (exitResult != SQLITE_OK)
	{
		sqlite3_close(db);
		throw SqlError("user_list.db cannot open.", MY_LOCATION);
	}

	// Inserting the new user into the users table.
	query = "INSERT INTO list"
		" VALUES('" + user.username + "', '" + user.password +
		"');";

	exitResult = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);
	sqlite3_close(db);
	signupMtx.unlock();
	if (exitResult != SQLITE_OK)
	{
		//std::string msgErr;
		//msgErr = std::format("Cannot perform on user_list.db the query:\n{}", query);
		throw SqlError(errMsg, MY_LOCATION);
	}
}
