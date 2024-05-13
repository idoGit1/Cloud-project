#include "ui.h"

UI::UI()
{
	authentication = (string)BLANKAUTH;
	currentUser = User();
}

UI::~UI()
{
	printf("\n\n UI destructor");
}



void UI::run()
{
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
		size_t spaceIdx = op.find(" ");
		try
		{
			if (op == "h" && spaceIdx == string::npos)
			{
				help();
			}
			else if (op == "login" && spaceIdx == string::npos)
			{
				login();
			}
			else if (op == "signup" && spaceIdx == string::npos)
			{
				signup();
			}
			else if (op == "quit" && spaceIdx == string::npos)
			{
				quit();
			}
			else if (op == "exit" && spaceIdx == string::npos)
			{
				exit_();
			}
			else if (op == "status" && spaceIdx == string::npos)
			{
				status();
			}
			else if (op.substr(0, spaceIdx) == "download")
			{
				download(op);
			}
			else if (op.substr(0, spaceIdx) == "upload")
			{
				upload(op);
			}
			else if (op.substr(0, spaceIdx) == "share")
			{
				share(op);
			}
			else if (op.substr(0, spaceIdx) == "remove")
			{
				remove(op);
			}
			else
			{
				printf("Unknown command. For help type h.\n");
			}
		}
		catch (int error)
		{
			cerr << error << "\n";;
		}

	}
}


void UI::help()
{
	printf("(cloud) h - help\nlogin\nsignup\nquit"
		"(to quit Account)\nstatus\ndownload filename.type\nupload"
		" filename.type\nshare @username@ filename.type\n\n");
}

void UI::login()
{

	User user;
	printf("(cloud) Username: ");
	cin >> user.username;
	printf("\n(cloud) Password: ");
	cin >> user.password;

	MainMsg msg;

	strncpy_s(msg.header.auth, 9, BLANKAUTH, 8);
	msg.header.type = Login;
	msg.data = user.toString();
	msg.header.size = msg.data.size();
	client.snd(msg);

	cleanMsg(msg);

	client.receive(msg);

	if (msg.header.type == Login && msg.data == SUCCESS)
	{
		authentication = msg.header.auth;
		printf("(cloud) Welcome %s! Logged in successfully!", user.username.c_str());
		currentUser = User(user);
	}
	else
	{
		printf("Problem occurred. Wrong username or password.");
	}
	
}

void UI::signup()
{
	User user;
	printf("(cloud) Enter username: ");
	cin >> user.username;
	printf("\n(cloud) Enter password: ");
	cin >> user.password;
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

	if (msg.header.type == Signup && msg.data == (string)SUCCESS)
	{
		printf("(cloud) %s signed up successfully!", user.username.c_str());
		//currentUser = User(user);
	}
	else
	{
		printf("(cloud) Problem occurred! Username is not unique.");
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
	msg.data = (string)SUCCESS;
	msg.header.size = 1;

	client.snd(msg);

	cleanMsg(msg);

	client.receive(msg);
	if (msg.header.type == Quit && msg.data == (string)SUCCESS)
	{
		authentication = (string)BLANKAUTH;
		printf("(cloud) %s Quitted successfully!", currentUser.username.c_str());
		currentUser = User();
	}
	else
	{
		printf("(cloud) problem occurred with quitting, UI::run\n");
		exit(1);
	}
}

void UI::exit_()
{
	MainMsg msg;

	msg.header.type = Exit;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = (string)SUCCESS;
	msg.header.size = 1;
	client.snd(msg);
	sleep_for(seconds(1));
	exit(0);
}

void UI::status()
{
	MainMsg msg;

	msg.header.type = Status;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = (string)SUCCESS;
	msg.header.size = 1;

	client.snd(msg);

	cleanMsg(msg);
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
	{
		cerr << EXCEPTION_CODES[25] << "\n";
		exit(25);
	}
	if (msg.header.type != Status || msg.data == (string)FAILURE)
	{
		cerr << "UI::execute- status: Problem occurred\n";
	}
	else if (msg.data == (string)SUCCESS)
	{
		printf("(cloud) No files uploaded so far.");
	}
	else
	{
		printf("(cloud) Status:\n%s", msg.data.c_str());
	}
}

void UI::download(string &op)
{
	size_t spaceIdx = op.find(" ");
	string inputFileName = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);
	MainMsg msg;

	msg.header.type = Download;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';
	msg.data = (string)inputFileName;
	msg.header.size = msg.data.size();

	client.snd(msg);
	cleanMsg(msg);
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
	{
		cerr << EXCEPTION_CODES[24] << "\n";
		exit(24);
	}
	if (msg.header.type == Download && msg.data != (string)FAILURE)
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
		{
			printf("UI::download file did not open\n");
		}
	}
	else
		printf("(cloud) Problem occurred. Double check the name of the file.");

}

void UI::upload(string &op)
{
	size_t spaceIdx = op.find(" ");
	string inputFilePath = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);
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
	ifstream inputFile;
	inputFile.open(inputFilePath, ios::binary);
	if (!inputFile.is_open())
	{
		printf("(cloud) No such file.");
		return;
	}
	//inputFile >> fileContent;
	string fileContent(istreambuf_iterator<char>(inputFile), {});

	inputFile.close();
	if (fileContent.size() > MAX_FILE_SIZE)
	{
		printf("File is to big! Notice, the greatest size of a file can be 900,000,000 bytes, or ~ 900 Megabytes!");
		return;
	}
	cerr << "File size is: " << fileContent.size() << "\n";
	cerr << "File.data() size is " << strlen(fileContent.data()) << "\n";
	cerr << "File.c_str() size is " << strlen(fileContent.c_str()) << "\n";

	MainMsg msg;

	msg.header.type = Upload;
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';

	msg.data = inputFileName + "*";
	msg.data += fileContent;
	msg.header.size = msg.data.size();
	printf("in UI: %lld", msg.data.size());
	client.snd(msg);

	cleanMsg(msg);
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
	{
		cerr << EXCEPTION_CODES[23] << "\n";
		exit(23);
	}
	if (msg.header.type == Upload && msg.data == (string)SUCCESS)
	{
		printf("(cloud) %s uploaded successfully!", inputFilePath.c_str());
	}
	else
	{
		printf("(cloud) Problem occurred, please try again and check that the file location you provided is correct.");
	}

}

void UI::share(string &op)
{
	size_t lastSpaceIdx = op.find_last_of(" "); // The structure is: share @username@ fileName
	size_t firstAtIdx = op.find("@");
	size_t secondAtIdx = op.find_last_of("@");
	string sharedUsername;
	string fileName;

	fileName = op.substr(lastSpaceIdx + 1, op.size() - lastSpaceIdx - 1);
	sharedUsername = op.substr(firstAtIdx + 1, secondAtIdx - firstAtIdx - 1);

	MainMsg msg;

	msg.header.type = Share;
	
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';

	msg.data = sharedUsername + "*" + fileName;
	msg.header.size = msg.data.size();

	client.snd(msg);

	cleanMsg(msg);
	client.receive(msg);

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
	{
		cerr << EXCEPTION_CODES[29];
		exit(29);
	}
	if (msg.header.type == Share && msg.data == SUCCESS)
	{
		printf("(cloud) %s is shared successfully with %s", fileName.c_str(), sharedUsername.c_str());
	}
	else
	{
		printf("(cloud) Problem occurred. Check that the user and the file exist.");
	}
}

void UI::remove(string &op)
{
	size_t spaceIdx = op.find(" ");
	string inputFileName = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);

	MainMsg msg;

	msg.header.type = Remove;
	memset(msg.header.auth, 0, 9);
	strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
	msg.header.auth[8] = '\0';

	msg.data = inputFileName;
	msg.header.size = msg.data.size();

	client.snd(msg);

	//memset(&msg, 0, sizeof(MainMsg));
	cleanMsg(msg);
	client.receive(msg);
	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
	{
		cerr << EXCEPTION_CODES[22] << "\n";
		exit(22);
	}
	if (msg.header.type == Remove && msg.data == (string)SUCCESS)
	{
		printf("(cloud) %s removed successfully!", inputFileName.c_str());
	}
	else
	{
		printf("(cloud) Problem occurred, please try again and check if the name of the file is correct.");
	}
}