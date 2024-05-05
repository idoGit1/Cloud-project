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
	// Speed up input and output
	/*cin.tie(0);
	ios_base::sync_with_stdio(0);*/
	string op;
	Client client = Client();

	printf("(cloud) Enter commands in lower letters. For help, type h");
	while (true)
	{
		if (currentUser.username != "")
			printf("\n(%s)>>", currentUser.username.c_str());
		else
			printf("\n>>");
		getline(cin, op);
		printf("\n");
		size_t spaceIdx = op.find(" ");
		if (op == "h" && spaceIdx == string::npos)
		{
			printf("(cloud) h - help\nlogin\nsignup\nquit"
				"(to quit Account)\nstatus\ndownload filename.type\nupload"
				" filename.type\nshare @username@ filename.type\n\n");
		}
		else if (op == "login" && spaceIdx == string::npos)
		{
			User user;
			printf("(cloud) Username: ");
			cin >> user.username;
			printf("\n(cloud) Password: ");
			cin >> user.password;
			
			//Message *msg = new Message();
			/*msg->setAuth(BLANKAUTH);
			msg->setType(Login);
			msg->setData(user.toString());*/
			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));

			strncpy_s(msg.header.auth, 9, BLANKAUTH, 8);
			msg.header.type = Login;
			msg.data = user.toString();
			msg.header.size = msg.data.size();
			client.snd(msg);

			memset(&msg, 0, sizeof(main_msg)); // Init for respond.
			client.receive(msg);
			if (msg.data == SUCCESS)
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
		else if (op == "signup" && spaceIdx == string::npos)
		{
			User user;
			printf("(cloud) Enter username: ");
			cin >> user.username;
			printf("\n(cloud) Enter password: ");
			cin >> user.password;
			printf("\n");

			//Message *msg = new Message();

			/*msg->setAuth(BLANKAUTH);
			msg->setType(Signup);
			msg->setData(user.toString());*/
			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));

			strncpy_s(msg.header.auth, 9, BLANKAUTH, 8);
			msg.header.auth[8] = '\0';
			msg.header.type = Signup;
			msg.data = user.toString();
			msg.header.size = (msg.data).size();

			client.snd(msg);
			memset(&msg, 0, sizeof(main_msg)); // Respond
			client.receive(msg);
			if (msg.data == (string)SUCCESS)
			{
				printf("(cloud) %s signed up successfully!", user.username.c_str());
				//currentUser = User(user);
			}
			else
			{
				printf("(cloud) Problem occurred! Username is not unique.");
			}
		}
		else if (op == "quit" && spaceIdx == string::npos)
		{
			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));
			msg.header.type = Quit;
			strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
			msg.header.auth[8] = '\0';
			msg.data = (string)SUCCESS;
			msg.header.size = 1;

			client.snd(msg);

			memset(&msg, 0, sizeof(main_msg)); // Respond
			
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
		else if (op == "exit" && spaceIdx == string::npos)
		{
			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));
			msg.header.type = Exit;
			strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
			msg.header.auth[8] = '\0';
			msg.data = (string)SUCCESS;
			msg.header.size = 1;
			client.snd(msg);
			sleep_for(seconds(1));
			exit(0);
		}
		else if (op == "status" && spaceIdx == string::npos)
		{
			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));

			msg.header.type = Status;
			strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
			msg.header.auth[8] = '\0';
			msg.data = (string)SUCCESS;
			msg.header.size = 1;

			client.snd(msg);

			memset(&msg, 0, sizeof(main_msg));
			client.receive(msg);

			if (msg.data == (string)FAILURE)
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
		else if (op.substr(0, spaceIdx) == "download")
		{

		}
		else if (op.substr(0, spaceIdx) == "upload")
		{
			string inputFilePath = op.substr(spaceIdx + 1, op.size() - spaceIdx - 1);
			string inputFileName; // Without path (\\)
			//cerr << "File name: " << inputFilePath;
			ifstream inputFile;
			inputFile.open(inputFilePath, ios::binary);
			if (!inputFile.is_open())
			{
				printf("(cloud) No such file.");
				continue;
			}
			//inputFile >> fileContent;
			string fileContent(istreambuf_iterator<char>(inputFile), {});

			inputFile.close();
			cerr << "File size is: " << fileContent.size() << "\n";
			cerr << "File.data() size is " << strlen(fileContent.data()) << "\n";
			cerr << "File.c_str() size is " << strlen(fileContent.c_str()) << "\n";
			size_t namePos = inputFilePath.find_last_of("\\");
			size_t namePos2 = inputFilePath.find_last_of("/");
			if (namePos != string::npos)
				inputFileName = inputFilePath.substr(namePos + 1, inputFilePath.size() - namePos - 1);
			else if (namePos2 != string::npos)
				inputFileName = inputFilePath.substr(namePos2 + 1, inputFilePath.size() - namePos - 1);
			else
				inputFileName = inputFilePath;

			const char *fileDataErr = fileContent.c_str();

			//printf("UI::execute- file binary:%s", fileDataErr);

			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));

			msg.header.type = Upload;
			strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
			msg.header.auth[8] = '\0';
	
			msg.data = inputFileName + "*";
			msg.data += fileContent;
			msg.header.size = msg.data.size();
			printf("in UI: %lld", msg.data.size());
			client.snd(msg);

			memset(&msg, 0, sizeof(main_msg));
			client.receive(msg);

			if (msg.data == (string)SUCCESS)
			{
				printf("(cloud) %s uploaded successfully!", inputFilePath.c_str());
			}
			else
			{
				printf("(cloud) Problem occurred, please try again and check that the file location you provided is correct.");
			}

		}
		else if (op.substr(0, spaceIdx) == "share")
		{

		}
		else
		{
			printf("Unknown command. For help type h.\n");
		}
	}
}