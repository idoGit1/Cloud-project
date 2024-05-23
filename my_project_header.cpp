#include "my_project_header.h"

std::string configLocation(const char *funcName, const char *fileName) 
{
	return "In file: " + std::string(fileName) + " in function: " + std::string(funcName);
}

