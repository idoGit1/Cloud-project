#ifndef SERVER_H_
#include "server.h"
#endif
#ifndef UM_H_
#include "um.h"
#endif
// All variables used for error handling end with E.


int main()
{
	//UM::createDatabase(); // ONLY ONCE, WITH FIRST RUN.

	Server server = Server();
	server.run();
	cin.get();
	return 0;
}
