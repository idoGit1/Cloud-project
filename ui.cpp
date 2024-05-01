#include "ui.h"


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
			
			Message *msg = new Message();
			msg->setAuth(BLANKAUTH);
			msg->setType(Login);
			msg->setData(user.toString());
			client.snd(msg);
			msg = new Message(); // Respond
			client.receive(msg);
			if (msg->getData() == SUCCESS)
			{
				authentication = msg->getAuth();
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

			Message *msg = new Message();

			msg->setAuth(BLANKAUTH);
			msg->setType(Signup);
			msg->setData(user.toString());

			client.snd(msg);
			msg = new Message(); // Respond
			client.receive(msg);
			if (msg->getData() == SUCCESS)
			{
				printf("(cloud) %s signed up successfully!", user.username.c_str());
			}
			else
			{
				printf("(cloud) Problem occurred! Username is not unique.");
			}
		}
		else if (op == "quit")
		{

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