#include "user.h"

bool User::operator==(User const &obj)
{
	return (this->username == obj.username && this->password == obj.password);
}
User::User()
{
	username = "";
	password = "";
}
User::User(const User &user)
{
	username = user.username;
	password = user.password;
}

string User::toString()
{
	return username + " " + password;
}