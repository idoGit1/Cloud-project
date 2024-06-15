#include "ui.h"


UI::UI()
{
	// Constructor, initializing attempts counters and authentication.
	loginAttemptsCounter = 0;
	signupsCounter = 0;
	authentication = BLANKAUTH;

}



std::string ManageFiles::fileIntoString(const std::string &path)
{
	// Getting a file into a binary string.
	using namespace std;
	// Inputting the file into a binary string.
	ifstream inputFile;
	inputFile.open(path, ios::binary);

	if (!inputFile.is_open())
		throw UserInputError("No such file.");

	// Getting the whole file into fileContent (iterating through the file byte by byte.
	// {} means until the end.
	string fileContent(istreambuf_iterator<char>(inputFile), {});

	inputFile.close();
	// If file is too big to handle
	if (fileContent.size() > MAX_FILE_SIZE)
		throw UserInputError("File is too large. Notice that the size of the file"
			"do not go over 80,000,000 bytes.");
	return fileContent;
}


void ManageFiles::stringIntoFile(const std::string &content, const std::string &name)
{
	// Getting a binary string into a file.
	using namespace std;
	ofstream outFile("Downloaded/" + name, ios::binary);

	if (outFile)
	{
		// If file opened.
		outFile.write(content.data(), content.size());
		outFile.close();
		printf("(cloud) %s downloaded successfully!", name.c_str());
	}
	else
		throw FileHandlingError("File did not open properly.", MY_LOCATION);
}


std::string UI::inputCommand()
{
	// Inputting the user's command
	using namespace std;

	string op;
	// If there is a user logged in (looking like (username)>>):
	if (authentication != BLANKAUTH)
		printf("\n(%s)>>", currentUser.username.c_str());
	// If there is not user logged in:
	else
		printf("\n>>");
	// Inputting the whole command (including spaces)
	getline(cin, op);
	// Checking that there is not an accedential null from before
	if (op == "")
		getline(cin, op);
	// Removing unnecessery spaces
	Local::removeStartEndSpaces(op);
	size_t spaceIdx = op.find(" ");

	return op;
}


bool UI::handleNoArgCommands(const std::string &op)
{
	// Handle all commands that have no arguments
	using namespace std;

	size_t spaceIdx = op.find(" ");


	if (op == "h" && spaceIdx == string::npos)
		help();

	else if (op == "login" && spaceIdx == string::npos)
	{
		// Against dos
		if (loginAttemptsCounter == 5)
		{
			printf("(cloud) Too many failed logins, program is terminated...");
			return false; // Terminate
		}
		if (authentication != BLANKAUTH)
		{
			printf("(cloud) To login to another use, first quit this user.");
			return true; // Wrong command- just continue
		}
		login();
	}
	else if (op == "signup" && spaceIdx == string::npos)
	{
		// Against dos- counting the time it takes to do 5 signups
		// if it is less than 10 seconds it is probably a bot.
		static std::chrono::steady_clock::time_point startTime;
		static std::chrono::steady_clock::time_point endTime;
		if (signupsCounter == 0)
			startTime = std::chrono::high_resolution_clock::now();
		if (signupsCounter == 5)
		{
			endTime = std::chrono::high_resolution_clock::now();

			std::chrono::seconds duration =
				std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

			if (duration <= std::chrono::seconds(10))
			{
				printf("(cloud) Bot detected. Terminating program...");
				return false; // Terminate
			}
		}
		if (authentication != BLANKAUTH)
		{
			printf("(cloud) To create another user, first quit this one");
			return true; // Continue
		}
		signup();
	}
	else if (op == "quit" && spaceIdx == string::npos)
	{
		if (authentication == BLANKAUTH)
		{
			// No user logged in

			printf("(cloud) No user logged in.");
			return true; // Continue
		}
		quit();
	}
	else if (op == "exit" && spaceIdx == string::npos)
	{
		exit_();
		return false; // Need to exit program.
	}
	else if (op == "status" && spaceIdx == string::npos)
	{
		if (authentication == BLANKAUTH)
		{
			// No user logged in

			printf("(cloud) No user logged in.");
			return true; // Continue
		}
		status();
	}
	else
	{
		printf("Unknown command. For help type h.\n");
	}
	return true;
}

bool UI::handle1ArgCommands(const std::string &op)
{
	// Handle all commands with an argument.
	using namespace std;
	size_t spaceIdx = op.find(" ");

	if (op.substr(0, spaceIdx) == "download")
	{
		if (authentication == BLANKAUTH)
		{
			// No user logged in

			printf("(cloud) No user logged in.");
			return true; // Continue
		}
		download(op);
	}
	else if (op.substr(0, spaceIdx) == "upload")
	{
		if (authentication == BLANKAUTH)
		{
			// No user logged in

			printf("(cloud) No user logged in.");
			return true; // Continue
		}
		upload(op);
	}
	else if (op.substr(0, spaceIdx) == "share")
	{
		if (authentication == BLANKAUTH)
		{
			// No user logged in

			printf("(cloud) No user logged in.");
			return true; // Continue
		}
		share(op);
	}
	else if (op.substr(0, spaceIdx) == "remove")
	{
		if (authentication == BLANKAUTH)
		{
			// No user logged in

			printf("(cloud) No user logged in.");
			return true; // Continue
		}
		remove(op);
	}
	else if (currentUser.username == "admin" && op.substr(0, spaceIdx) == "removeuser")
	{
		// Admin only command
		printf("(cloud) Are you sure? (y/n)");
		char ans;
		cin >> ans;
		if (ans == 'n')
		{
			printf("(cloud) Aborted.");
			return true; // Continue
		}

		removeUser(op);
	}
	else
	{
		printf("Unknown command. For help type h.\n");
	}
	return true;
}

bool UI::handleCommand(const std::string &op)
{
	// Handle the command given by the user, and sort it to the right function.
	using namespace std;

	size_t spaceIdx = op.find(" ");
	if (spaceIdx == string::npos)
		return handleNoArgCommands(op);

	return handle1ArgCommands(op);
	
}


void UI::run()
{
	// The main function of the client side- getting user input, analyzing it and
	// Calling the right function to proceed.
	using namespace std;

	string op;

	bool isFatalError;

	printf("(cloud) Enter commands in lower letters. For help, type h.\n Notice, you can upload "
		"files with size no greater then 80,000,000 bytes! (~80 Megabytes)");

	while (true)
	{
		op = inputCommand();
		
		try
		{
			// true- continue. false- terminate
			isFatalError = !handleCommand(op);
			if (isFatalError)
				return;
		}
		// Handling errors:
		catch (UserInputError &ex)
		{
			printf("(cloud) %s", ex.whatForUser());
		}
		catch (CommunicationError &ex)
		{
			cerr << "Fatal error- " << ex.whatForUser() << "\nTerminating program...";
			return;
		}
		catch (ContentError &ex)
		{
			printf("(cloud) %s", ex.whatForUser());
		}
		catch (FileHandlingError &ex)
		{
			printf("(cloud) %s", ex.whatForUser());
		}
		// Catching errors unlisted
		catch (std::exception &ex)
		{
			printf("(cloud) General error- %s", ex.what());
		}

	}
}


void UI::help()
{
	// Prints a help message
	printf("(cloud) h - help\nlogin\nsignup\nquit"
		"(to quit Account)\nstatus\ndownload filename.type\nupload"
		" filename.type\nshare @username@ filename.type\nremove filename.type"
		"\nexit\n");
}


void UI::login()
{
	// Logging in a new user

	User user;
	printf("(cloud) Username: ");
	std::cin >> user.username;
	printf("\n(cloud) Password: ");
	std::cin >> user.password;

	// Checking if there is * or ' that might fail the program or be a sql injection.
	if (user.username.find("'") != std::string::npos ||
		user.password.find("'") != std::string::npos ||
		user.username.find("*") != std::string::npos ||
		user.password.find("*") != std::string::npos)
	{
		printf("(cloud) Chars \"'\" and \"*\" are not allowed.");
		return;
	}
	// Sending a message to server
	MainMsg msg(Login, authentication.c_str(), true, user.toString());
	client.snd(msg);
	// Cleaning message
	msg.clean();

	// Receiving messgae
	client.receive(msg);


	if (msg.header.success 
		&& strncmp(msg.header.auth, msg.data.c_str(), 8) == 0)
	{
		// Success
		authentication = msg.header.auth;
		printf("(cloud) Welcome %s! Logged in successfully!", user.username.c_str());
		currentUser = User(user);
	}
	else
	{
		// Failure
		loginAttemptsCounter++;
		throw UserInputError(msg.data.c_str());
	}
	
}

void UI::signup()
{
	// Signing up new user
	User user;
	printf("(cloud) Enter username: ");
	std::cin >> user.username;
	printf("\n(cloud) Enter password: ");
	std::cin >> user.password;
	printf("\n");
	
	// Checking if there is * or ' that might fail the program or be a sql injection.
	if (user.username.find("'") != std::string::npos ||
		user.password.find("'") != std::string::npos ||
		user.username.find("*") != std::string::npos ||
		user.password.find("*") != std::string::npos)
	{
		printf("(cloud) Chars \"'\" and \"*\" are not allowed.");
		return;
	}
	// Sending messgae
	MainMsg msg(Signup, authentication.c_str(), true, user.toString());

	client.snd(msg);

	msg.clean();
	client.receive(msg);

	if (msg.header.success)
		printf("(cloud) %s signed up successfully!", user.username.c_str());
	else
		throw UserInputError("Username is not unique.");
}


void UI::removeUser(const std::string &op)
{
	// Getting the whole command, and removing the user with the given username.
	// Only for admins.

	using std::string;
	size_t spaceIdx = op.find(' ');
	// Extracting the username.
	string username = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);

	MainMsg msg(RemoveUser, authentication.c_str(), true, username);

	client.snd(msg);

	msg = MainMsg();

	client.receive(msg);

	if (msg.header.success)
		printf("(cloud) User removed successfully.");
	else
		printf("Error: %s", msg.data.c_str());
}


void UI::quit()
{
	// Quiting- logging out of the user.
	if (authentication == BLANKAUTH)
	{
		printf("(cloud) No active user.");
		return;
	}

	MainMsg msg(Quit, authentication.c_str(), true, "");

	client.snd(msg);

	msg.clean();

	client.receive(msg);
	if (msg.header.success)
	{
		// Success
		printf("(cloud) %s Quitted successfully!", currentUser.username.c_str());
		authentication = BLANKAUTH;
		currentUser = User();
	}
	// Failure
	else
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);
}

void UI::exit_()
{
	// Exiting the system- shutting down.
	MainMsg msg;

	msg = MainMsg(Exit, authentication.c_str(), true, "");
	client.snd(msg);

	// Let the message to pass safely through the socket.
	std::this_thread::sleep_for(std::chrono::seconds(1));
	// I can just exit because everything has a destructor and they will each clean up its own 
	// responsibility.
	return;
}

void UI::status()
{
	// Getting status about files, sahred file, and files the user is sharing.
	
	MainMsg msg(Status, authentication.c_str(), true, "");

	client.snd(msg);
	
	msg.clean();
	client.receive(msg);


	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code.", MY_LOCATION);
	else if (!msg.header.success)
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);
	else
		printf("(cloud) Status:\n%s", msg.data.c_str());
}

void UI::download(const std::string &op)
{
	// Downloading the file specified in op.
	using namespace std;

	size_t spaceIdx = op.find(" ");
	string inputFileName = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);

	Local::removeStartEndSpaces(inputFileName);
	if (spaceIdx == string::npos || inputFileName == "")
	{
		// Did not give file.
		printf("Please specify the name of the file you would"
			"like to download, For example: download myfile.txt is a valid command.");
		return;
	}
	// Check if there is use with the char *.
	if (op.find("*") != std::string::npos)
	{
		// Protecting from system failure
		printf("(cloud) char \"*\" is not allowed");
		return;
	}
	printf("(cloud) Downloading...\n");

	MainMsg msg(Download, authentication.c_str(), true, inputFileName);

	client.snd(msg);

	msg.clean();
	client.receive(msg);

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code.", MY_LOCATION);
	if (msg.header.success)
	{
		// Getting the file name and the file content that are seprated by a star.
		size_t starPos = msg.data.find("*");
		string outFileName = msg.data.substr(0, starPos);
		string fileContent = msg.data.substr(starPos + 1, msg.data.size() - starPos);

		ManageFiles::stringIntoFile(fileContent, outFileName);
		
	}
	else
		throw UserInputError("No such file.");

}


std::string UI::getFileNameFromPath(const std::string &path)
{
	// Get path (with / or \) and config the file name from it.

	using namespace std;

	string inputFileName; // Without path (\\)
	// There are two options: using \ and using /. namePos is for \ and namePos2 is for /

	size_t namePos = path.find_last_of("\\");
	size_t namePos2 = path.find_last_of("/");

	if (namePos != string::npos) // Using \.
		inputFileName = path.substr(namePos + 1, path.size() - namePos - 1);

	else if (namePos2 != string::npos)// Using /
		inputFileName = path.substr(namePos2 + 1, path.size() - namePos - 1);

	else // In the same directory...
		inputFileName = path;

	return inputFileName;
}


void UI::upload(const std::string &op)
{
	// Uploading file specified in op.
	using namespace std;

	size_t spaceIdx = op.find(" ");

	// Getting the full path of the file without spaces
	string inputFilePath = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);
	Local::removeStartEndSpaces(inputFilePath);

	if (spaceIdx == string::npos || inputFilePath == "")
	{
		// No file given
		printf("Please specify the path of the file you would"
			"like to upload, For example: upload C:/desktop/myfile.txt is a valid command.");
		return;
	}

	// Check if there is use with the char *.
	if (op.find("*") != string::npos)
	{
		printf("(cloud) char \"*\" is not allowed");
		return;
	}

	string inputFileName = getFileNameFromPath(inputFilePath);
	
	string fileContent = ManageFiles::fileIntoString(inputFilePath);
	
	printf("(cloud) Uploading...\n");

	MainMsg msg(Upload, authentication.c_str(), true, inputFileName + "*" + fileContent);

	client.snd(msg);

	msg.clean();
	client.receive(msg);

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code.", MY_LOCATION);
	if (msg.header.success)
		printf("(cloud) %s uploaded successfully!", inputFilePath.c_str());
	else
		throw CommunicationError(msg.data.c_str(), MY_LOCATION);

}

void UI::share(const std::string &op)
{
	// Sharing specified file with specified user.
	// 
	// Removing unwanted spaces from both edges of the string.

	size_t lastSpaceIdx = op.find_last_of(" "); // The structure is: share @username@ fileName
	size_t firstAtIdx = op.find("@");
	size_t secondAtIdx = op.find_last_of("@");
	std::string sharedUsername;
	std::string fileName;

	// Protecting the system from invaild input.
	if (firstAtIdx == std::string::npos || firstAtIdx == secondAtIdx ||
		lastSpaceIdx == std::string::npos
		|| lastSpaceIdx < secondAtIdx)
	{
		printf("(cloud) Unknown form of command. Notice, the form of share command is:\n"
			"share @<username>@ <file name>");
		return;
	}
	// Extracting file name and shared username
	fileName = op.substr(lastSpaceIdx + 1, op.size() - lastSpaceIdx - 1);
	sharedUsername = op.substr(firstAtIdx + 1, secondAtIdx - firstAtIdx - 1);

	MainMsg msg(Share, authentication.c_str(), true, sharedUsername + "*" + fileName);

	client.snd(msg);

	msg.clean();
	client.receive(msg);

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code", MY_LOCATION);
	if (msg.header.success)
		printf("(cloud) %s is shared successfully with %s", fileName.c_str(), sharedUsername.c_str());
	else
		throw ContentError(msg.data.c_str(), MY_LOCATION);
}

void UI::remove(const std::string &op)
{
	// Removing given file.

	size_t spaceIdx = op.find(" ");
	// Checking if file is really given.
	if (spaceIdx == std::string::npos)
	{
		printf("(cloud) File name is required");
		return;
	}
	// Extracting file's name
	std::string inputFileName = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);

	MainMsg msg(Remove, authentication.c_str(), true, inputFileName);

	client.snd(msg);

	msg .clean();
	client.receive(msg);

	if (strncmp(msg.header.auth, authentication.c_str(), 8) != 0)
		throw CommunicationError("Wrong authentication code", MY_LOCATION);
	if (msg.header.success)
		printf("(cloud) %s removed successfully!", inputFileName.c_str());
	else
		throw ContentError(msg.data.c_str(), MY_LOCATION);
}