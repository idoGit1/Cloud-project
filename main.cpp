#ifndef SERVER_H_
#include "server.h"
#endif
#ifndef UM_H_
#include "um.h"
#endif
// All variables used for error handling end with E.



int main(int argc, char **argv)
{
	//Data::createDatabase(); // ONLY ONCE, WITH FIRST RUN.
	try
	{
		Server server = Server();
		server.run();
		std::cin.get();
	}
	catch (CommunicationError &ex)
	{
		std::cerr << ex.what();
	}
	catch (SqlError &ex)
	{
		std::cerr << ex.what();
	}
	return 0;
}
