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
		cin >> op;
		printf("\n");
		int spaceIdx = op.find(" ");
		if (op == "h")
		{
			printf("(cloud) h - help\nlogin\nsignup\nquit"
				"(to quit Account)\nstatus\ndownload 'filename.type'\nupload"
				" (filename.type)\nshare @username@ 'filename.type'\n\n");
		}
		else if (op == "login")
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
		else if (op == "signup")
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
		else if (op == "quit")
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
		else if (op == "exit")
		{
			main_msg msg;
			memset(&msg, 0, sizeof(main_msg));
			msg.header.type = Exit;
			strncpy_s(msg.header.auth, 9, authentication.c_str(), 8);
			msg.header.auth[8] = '\0';
			msg.data = (string)SUCCESS;
			msg.header.size = 1;
			client.snd(msg);

			exit(0);
		}
		else if (op == "status")
		{

		}
		else if (op.substr(0, spaceIdx) == "download")
		{

		}
		else if (op.substr(0, spaceIdx) == "upload")
		{

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