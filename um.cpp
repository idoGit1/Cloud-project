#include "um.h"

std::vector<User> UM::activeUsers;
std::mutex UM::activeUsersMtx;

UM::UM()
{
	authentication = BLANKAUTH;
}



MainMsg UM::success(Operation type, std::string data = "")
{
	return MainMsg(type, authentication.c_str(), true, data);
}
MainMsg UM::failure(Operation type, std::string data = "")
{
	return MainMsg(type, authentication.c_str(), false, data);
}


MainMsg UM::execute(MainMsg msg)
{
	using std::cerr;
	MainMsg result;
	Operation current = msg.header.type;
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code",
			MY_LOCATION);
	try
	{
		switch (msg.header.type)
		{
		case Exit:
			result = exit_(msg);
			break;
		case RemoveUser:
			result = removeUser(msg);
			break;
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
		default:
			throw ContentError("Unknown command", MY_LOCATION);
			break;
		}
	}
	catch (CommunicationError &ex)
	{
		cerr << ex.what();
		result = MainMsg(current, authentication.c_str(), false, ex.whatForUser());
	}
	catch (SqlError &ex)
	{
		cerr << ex.what();
		result = MainMsg(current, authentication.c_str(), false, ex.whatForUser());
	}
	catch (ContentError &ex)
	{
		cerr << ex.what();
		result = MainMsg(current, authentication.c_str(), false, ex.whatForUser());
	}
	catch (FileHandlingError &ex)
	{
		cerr << ex.what();
		result = MainMsg(current, authentication.c_str(), false, ex.whatForUser());
	}
	catch (UserInputError &ex)
	{
		cerr << ex.what();
		result = MainMsg(current, authentication.c_str(), false, ex.whatForUser());
	}
	catch (...)
	{
		cerr << "Unknown error";
	}
	return result;
}



MainMsg UM::removeUser(MainMsg &msg)
{
	if (currentUser != ADMIN)
		throw ContentError("User doesn't have permission do commit this action", MY_LOCATION);

	using std::string;

	string username = msg.data;

	dataManager.removeUser(username);
	dataManager.deleteFolder(username);
	
	return MainMsg(RemoveUser, authentication.c_str(), true, "");
}



MainMsg UM::exit_(MainMsg &msg)
{
	if (authentication != BLANKAUTH)
		quit(msg);
	return MainMsg();
}




MainMsg UM::quit(MainMsg &msg)
{
	// Erase the user from active users.
	activeUsersMtx.lock();
	//std::vector<User>::iterator i = find(activeUsers.begin(), activeUsers.end(), currentUser);
	int idx = Local::binarySearchInVector(activeUsers, currentUser);
	if (idx == -1)
		throw std::exception("In UM::quit in file um.cpp: cannot find user in activeUsers vector.");
	activeUsers.erase(activeUsers.begin() + idx);
	activeUsersMtx.unlock();
	currentUser = User();
	authentication = BLANKAUTH;
	return success(msg.header.type);
}

MainMsg UM::signup(MainMsg &msg)
{
	using std::string;

	string strMsg = msg.data; // Format: [username password]

	size_t spaceIdx = strMsg.find(" ");

	string username = strMsg.substr(0, spaceIdx);
	string password = strMsg.substr(spaceIdx + 1, strMsg.length());

	int exitResult;
	char *errMsg;
	User user;
	user.username = username;
	user.password = password;

	dataManager.updateUsersTable(user); 
	dataManager.createUserFolder(user);
	dataManager.createSharingDBTable(user); 
	dataManager.createSharedWithMeFolder(user);

	return success(msg.header.type);
}

MainMsg UM::login(MainMsg &msg)
{
	using std::string;

	User user1;
	string strMsg = msg.data; // Format: [username password]

	size_t spaceIdx = strMsg.find(" ");

	string username = strMsg.substr(0, spaceIdx);
	string password = strMsg.substr(spaceIdx + 1, strMsg.length());

	user1.username = username;
	user1.password = password;
	// Check if user is currently active.
	activeUsersMtx.lock();
	if (Local::binarySearchInVector(activeUsers, user1) != -1) // Found user in vector
	{
		// User is already active from another client.
		activeUsersMtx.unlock();
		throw CommunicationError("User is already active", MY_LOCATION);
		
	}

	if (user1 == ADMIN)
	{
		// User is admin
		//activeUsers.push_back(ADMIN);
		Local::addToSortedVector(activeUsers, ADMIN);
		activeUsersMtx.unlock();

		currentUser = ADMIN;
		authentication = generateAuthentication();
		return success(Login, authentication);
	}
	else if (dataManager.isPasswordCorrect(user1))
	{
		currentUser = User(user1);
		// Protecting double accessing from different devices to the same user.
		//activeUsers.push_back(currentUser);
		Local::addToSortedVector(activeUsers, currentUser);
		activeUsersMtx.unlock();
		authentication = generateAuthentication();

		return success(Login, authentication);
	}
	// Password is inncorrect
	activeUsersMtx.unlock();
	throw CommunicationError("Incorrect password", MY_LOCATION); // Password incorrect
	
}

MainMsg UM::download(MainMsg &msg)
{
	std::string fileName;
	std::string fileContent;
	
	fileContent = dataManager.downloadFile(msg.data, currentUser.username);
	
	if (msg.data[0] == '.')
		fileName = msg.data.substr(1, msg.data.size() - 1);
	else
		fileName = msg.data;

	return MainMsg(Download, authentication.c_str(), true, fileName + "*" + fileContent);
	
}

MainMsg UM::upload(MainMsg &msg)
{
	printf("UM::upload\n");
	size_t starPos = msg.data.find("*");
	std::string outFileName = msg.data.substr(0, starPos);
	std::string fileContent = msg.data.substr(starPos + 1, msg.data.size() - starPos);
	// Need to check if file is already exist.

	// TO DO: write dataManager.uploadFile();

	std::ofstream outFile((DataManager::DATA_PATH + currentUser.username + "\\" + outFileName)
		, std::ios::binary);
	if (outFile)
	{
		outFile.write(fileContent.data(), fileContent.size());
		outFile.close();
		return success(msg.header.type);
	}
	throw FileHandlingError("Cannot open file", MY_LOCATION);
}

MainMsg UM::share(MainMsg &msg)
{
	using namespace std;

	size_t starIdx = msg.data.find("*");
	
	string sharedUsername = msg.data.substr(0, starIdx);
	string fileName = msg.data.substr(starIdx + 1, msg.data.size() - starIdx - 1);

	if (sharedUsername == currentUser.username)
		throw UserInputError("Cannot share file with yourself");

	dataManager.shareFile(sharedUsername, currentUser, fileName);

	return success(msg.header.type);

}

MainMsg UM::status(MainMsg &msg)
{
	using namespace std;

	if (currentUser == ADMIN)
	{
		MainMsg respond(Status, authentication.c_str(),
			true, dataManager.registeredUsersList());
		return respond;
	}

	string path = DataManager::DATA_PATH + currentUser.username; // The directory with the user's files.
	string data = "";
	string sharedWithMePath = path + "\\.shared";
	
	data = dataManager.getFilesInFolder(path);
	data += "\nSHARED WITH ME:\n";
	data += dataManager.getFilesInFolder(sharedWithMePath);
	data += "\n\nSHARING WITH OTHERS:\n";
	data += dataManager.mySharingList(currentUser.username);

	return MainMsg(Status, authentication.c_str(), true, data);
}

MainMsg UM::remove_(MainMsg &msg)
{
	std::string path = DataManager::DATA_PATH + currentUser.username; // The directory with the user's files.
	std::string fileName = msg.data;
	path += "\\" + fileName;
	if (remove(path.c_str()) == 0)
		return success(msg.header.type);
	else
	{
		std::string msgErr;
		msgErr = std::format("Cannot remove the file: {}.", fileName);
		throw FileHandlingError(msgErr.c_str(), MY_LOCATION);
	}
}



std::string UM::generateAuthentication()
{
	// Taken from geeksforgeeks.
	std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

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
