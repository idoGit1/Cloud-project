#include "user.h"

bool User::operator==(User const &obj) const
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

User::User(const std::string username, const std::string password)
{
	this->username = username;
	this->password = password;
}

std::string User::toString() const
{
	return username + " " + password;
}