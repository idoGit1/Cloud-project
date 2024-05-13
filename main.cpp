#ifndef UI_H_
#include "ui.h"
#endif
#ifndef USER_H_
#include "user.h"
#endif
#ifndef MESSAGE_H_
#include "message.h"
#endif
#ifndef MY_PROJECT_HEADER_H_
#include "d:/Cloud project/Header/my_project_header.h"
#endif
#ifndef CLIENT_H_
#include "client.h"
#endif

int main()
{
	printf("Size of main_msg struct: %zu\n", sizeof(MainMsg));

	UI ui = UI();
	ui.run();

	return 0;
}