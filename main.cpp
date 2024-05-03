#ifndef UI_H_
#include "ui.h"
#endif
#ifndef USER_H_
#include "user.h"
#endif
#ifndef MESSAGE_H_
#include "message.h"
#endif
#ifndef COMMON_H_
#include "common.h"
#endif
#ifndef CLIENT_H_
#include "client.h"
#endif

int main()
{
	printf("Size of main_msg struct: %zu\n", sizeof(main_msg));

	UI ui;
	ui.run();

	return 0;
}