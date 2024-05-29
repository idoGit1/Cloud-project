#pragma once
#ifndef USER_H_
#define USER_H_
#include "d:/Cloud project/Additional/my_project_header.h"

struct User
{
	std::string username;
	std::string password;

	bool operator==(User const &) const noexcept;
	User();
	explicit User(const std::string username, const std::string password);
	User(const User &);

	std::string toString() const;

	bool operator !=(const User &obj) const noexcept
	{
		return !operator==(obj);
	}

	bool operator <(const User &obj) const noexcept
	{
		return (username.compare(obj.username) < 0) ? false : true;
	}

	bool operator >(const User &obj) const noexcept
	{
		return !(operator<(obj) || operator==(obj));
	}

	bool operator <=(const User &obj) const noexcept
	{
		return operator<(obj) || operator==(obj);
	}
	bool operator >=(const User &obj) const noexcept
	{
		return operator>(obj) || operator==(obj);
	}

};

const User ADMIN("admin", "idofinkel112358");
#endif