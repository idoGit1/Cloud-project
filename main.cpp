#include "ui.h"

int main(int argc, char **argv)
{

	UI ui = UI();
	if (argc == 1)
		ui.run();
	else if (argv[1] == "local_host")
		ui.run();
	else if (argv[1] == "extern")
		ui.run(EXTERN_IP);

	return 0;
}