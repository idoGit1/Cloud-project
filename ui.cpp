#include "ui.h"


namespace stringFuncs
{
	void removeStartSpaces(std::string &str)
	{
		int idx = 0;
		for (idx; idx < str.size(); idx++)
		{
			if (str[idx] == ' ')
				continue;
			else
				break;
		}
		str = str.substr(idx, str.size() - idx);
	}
}



UI::UI()
{
	authentication = BLANKAUTH;
	//currentUser = User();
	client = Client();
}

UI::~UI()
{
	printf("\n\n UI destructor");
}



void UI::run(const std::string &ip)
{
	using namespace std;
	
	try
	{
		client.build(ip);;
	}
	catch (...)
	{
		exit(1); // TO DO: HANDLE EXCEPTIONS
	}

	string op;

	//client = Client();

	printf("(cloud) Enter commands in lower letters. For help, type h\n Notice, you can upload "
		"files with size no greater then 900,000,000 bytes (~900 Megabytes)!\n For fast"
		" performance avoid files larger then 10 Megabytes");

	while (true)
	{
		if (currentUser.username != "")
			printf("\n(%s)>>", currentUser.username.c_str());
		else
			printf("\n>>");
		getline(cin, op);
		//printf("\n");
		if (op == "")
			getline(cin, op);
		stringFuncs::removeStartSpaces(op);
		size_t spaceIdx = op.find(" ");

		try
		{
			if (op == "h" && spaceIdx == string::npos)
			{
				help();
			}
			else if (op == "login" && spaceIdx == string::npos)
			{
				if (authentication != BLANKAUTH)
				{
					printf("(cloud) To login to another use, first quit this user.");
					continue;
				}
				login();
			}
			else if (op == "signup" && spaceIdx == string::npos)
			{
				if (authentication != BLANKAUTH)
				{
					printf("(cloud) To create another user, first quit this one");
					continue;
				}
				signup();
			}
			else if (op == "quit" && spaceIdx == string::npos)
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				quit();
			}
			else if (op == "exit" && spaceIdx == string::npos)
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				exit_();
			}
			else if (op == "status" && spaceIdx == string::npos)
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				status();
			}
			else if (op.substr(0, spaceIdx) == "download")
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				download(op);
			}
			else if (op.substr(0, spaceIdx) == "upload")
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				upload(op);
			}
			else if (op.substr(0, spaceIdx) == "share")
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				share(op);
			}
			else if (op.substr(0, spaceIdx) == "remove")
			{
				if (authentication == BLANKAUTH)
				{
					printf("(cloud) No user logged in.");
					continue;
				}
				remove(op);
			}
			else if (currentUser == ADMIN && op.substr(0, spaceIdx) == "removeuser")
			{
				printf("(cloud) Are you sure? (y/n)");
				char ans;
				cin >> ans;
				if (ans == 'n')
				{
					printf("(cloud) Aborted.");
				}
				removeUser(op);
			}
			else
			{
				printf("Unknown command. For help type h.\n");
			}
		}
		catch (UserInputError &ex)
		{
			printf("(cloud) %s", ex.what());
		}
		catch (CommunicationError &ex)
		{
			cerr << ex.what();
			return;
		}
		catch (ContentError &ex)
		{
			printf("(cloud) %s", ex.what());
		}
		catch (FileHandlingError &ex)
		{
			printf("(cloud) %s", ex.what());
		}

	}
}


void UI::help()
{
	printf("(cloud) h - help\nlogin\nsignup\nquit"
		"(to quit Account)\nstatus\ndownload filename.type\nupload"
		" filename.type\nshare @username@ filename.type\nremove filename.type"
		"\n");
}

void UI::login()
{

	User user;
	printf("(cloud) Username: ");
	std::cin >> user.username;
	printf("\n(cloud) Password: ");
	std::cin >> user.password;

	MainMsg msg;

	strncpy_s(msg.header.auth, 9, BLANKAUTH, 8);
	msg.header.type = Login;
	msg.data = user.toString();
	msg.header.size = msg.data.size();
	client.snd(msg);

	msg = MainMsg();

	client.receive(msg);


	if (msg.header.success 
		&& strncmp(msg.header.auth, msg.data.c_str(), 8) == 0)
	{
		authentication = msg.header.auth;
		printf("(cloud) Welcome %s! Logged in successfully!", user.username.c_str());
		currentUser = User(user);
	}
	else
	{
		throw UserInputError("Wrong username or password");
	}
	
}

void UI::signup()
{
	User user;
	printf("(cloud) Enter username: ");
	std::cin >> user.username;
	printf("\n(cloud) Enter password: ");
	std::cin >> user.password;
	printf("\n");

	MainMsg msg;

	strncpy_s(msg.header.auth, 9, BLANKAUTH, 8);
	msg.header.auth[8] = '\0';
	msg.header.type = Signup;
	msg.data = user.toString();
	msg.header.size = (msg.data).size();

	client.snd(msg);

	cleanMsg(msg);
	client.receive(msg);

	if (msg.header.type == Signup && msg.header.success)
		printf("(cloud) %s signed up successfully!", user.username.c_str());
	else
		throw UserInputError("Username is not unique.");
}


void UI::removeUser(std::string &op)
{


	using std::string;
	size_t spaceIdx = op.find(' ');

	string username = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);

	MainMsg msg(RemoveUser, authentication.c_str(), 1, username);

	client.snd(msg);

	msg = MainMsg();

	client.receive(msg);

	if (msg.header.success)
	{
		printf("(cloud) User removed successfully.");
	}
	else
	{
		printf("Error: %s", msg.data.c_str());
	}
	
}


void UI::quit()
{
	if (currentUser.username == "" && currentUser.password == "")
	{
		printf("(cloud) No active user.");
		return;
	}

	MainMsg msg;

	msg.header.type = Quit;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = "";
	msg.header.size = msg.data.size();

	client.snd(msg);

	cleanMsg(msg);

	client.receive(msg);
	if (msg.header.type == Quit && msg.header.success)
	{
		authentication = BLANKAUTH;
		printf("(cloud) %s Quitted successfully!", currentUser.username.c_str());
		currentUser = User();
	}
	else
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);
}

void UI::exit_()
{
	MainMsg msg;

	msg.header.type = Quit;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = SUCCESS;
	msg.header.size = 1;
	client.snd(msg);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	exit(0);
}

void UI::status()
{

	MainMsg msg;

	msg.header.type = Status;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = "";
	msg.header.size = msg.data.size();

	client.snd(msg);

	cleanMsg(msg);
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code.", MY_LOCATION);
	else if (!msg.header.success)
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);
	else if (msg.data == "")
	{
		printf("(cloud) No files uploaded so far.");
	}
	else
	{
		printf("(cloud) Status:\n%s", msg.data.c_str());
	}
}

void UI::download(std::string &op)
{
	using namespace std;

	size_t spaceIdx = op.find(" ");
	string inputFileName = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);
	MainMsg msg;

	stringFuncs::removeStartSpaces(inputFileName);
	if (spaceIdx == string::npos || inputFileName == "")
	{
		printf("Please specify the name of the file you would"
			"like to download, For example: download myfile.txt is a valid command.");
		return;
	}
	msg.header.type = Download;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = (string)inputFileName;
	msg.header.size = msg.data.size();

	client.snd(msg);
	cleanMsg(msg);
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code.", MY_LOCATION);
	if (msg.header.success)
	{
		size_t starPos = msg.data.find("*");
		string outFileName = msg.data.substr(0, starPos);
		string fileContent = msg.data.substr(starPos + 1, msg.data.size() - starPos);
		// Need to check if file is already exist.
		ofstream outFile(outFileName, ios::binary);
		//cerr << outFileName;
		// Need to check if worked.
		if (outFile)
		{
			outFile.write(fileContent.data(), fileContent.size());
			//outFile << fileContent;
			outFile.close();
			printf("(cloud) %s downloaded successfully!", outFileName.c_str());
		}
		else
			throw FileHandlingError("File did not open properly or does not exists.", MY_LOCATION);
	}
	else
		throw UserInputError("No such file.");

}

void UI::upload(std::string &op)
{
	using namespace std;

	size_t spaceIdx = op.find(" ");


	string inputFilePath = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);
	stringFuncs::removeStartSpaces(inputFilePath);

	if (spaceIdx == string::npos || inputFilePath == "")
	{
		printf("Please specify the path of the file you would"
			"like to upload, For example: upload C:/desktop/myfile.txt is a valid command.");
		return;
	}



	string inputFileName; // Without path (\\)
	size_t namePos = inputFilePath.find_last_of("\\");
	size_t namePos2 = inputFilePath.find_last_of("/");
	if (namePos != string::npos)
		inputFileName = inputFilePath.substr(namePos + 1, inputFilePath.size() - namePos - 1);
	else if (namePos2 != string::npos)
		inputFileName = inputFilePath.substr(namePos2 + 1, inputFilePath.size() - namePos - 1);
	else
		inputFileName = inputFilePath;
	//cerr << "File name: " << inputFilePath;
	if (inputFileName.find('*') != string::npos)
	{
		printf("(cloud) Invalid name. You must not use '*'.");
		return;
	}
	
	
	ifstream inputFile;
	inputFile.open(inputFilePath, ios::binary);
	if (!inputFile.is_open())
		throw UserInputError("No such file.");

	string fileContent(istreambuf_iterator<char>(inputFile), {});

	inputFile.close();
	if (fileContent.size() > MAX_FILE_SIZE)
		throw UserInputError("File is too large. Notice that the size of the file"
			"do not go over 90,000,000 bytes.");
	
	MainMsg msg;

	msg.header.type = Upload;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';

	msg.data = inputFileName + "*";
	msg.data += fileContent;
	msg.header.size = msg.data.size();
	client.snd(msg);

	msg = MainMsg();
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code.", MY_LOCATION);
	if (msg.header.success)
		printf("(cloud) %s uploaded successfully!", inputFilePath.c_str());
	else
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);

}

void UI::share(std::string &op)
{

	size_t lastSpaceIdx = op.find_last_of(" "); // The structure is: share @username@ fileName
	size_t firstAtIdx = op.find("@");
	size_t secondAtIdx = op.find_last_of("@");
	std::string sharedUsername;
	std::string fileName;

	fileName = op.substr(lastSpaceIdx + 1, op.size() - lastSpaceIdx - 1);
	sharedUsername = op.substr(firstAtIdx + 1, secondAtIdx - firstAtIdx - 1);

	MainMsg msg;

	msg.header.type = Share;
	
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';

	msg.data = sharedUsername + "*" + fileName;
	msg.header.size = msg.data.size();

	client.snd(msg);

	msg = MainMsg();
	client.receive(msg);

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code", MY_LOCATION);
	if (msg.header.success)
		printf("(cloud) %s is shared successfully with %s", fileName.c_str(), sharedUsername.c_str());
	else
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);
}

void UI::remove(std::string &op)
{
	size_t spaceIdx = op.find(" ");
	std::string inputFileName = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);

	MainMsg msg;

	msg.header.type = Remove;
	memset(msg.header.auth, 0, 9);
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';

	msg.data = inputFileName;
	msg.header.size = msg.data.size();

	client.snd(msg);

	
	msg = MainMsg();
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code", MY_LOCATION);
	if (msg.header.success)
		printf("(cloud) %s removed successfully!", inputFileName.c_str());
	else
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);
}