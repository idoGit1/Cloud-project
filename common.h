#pragma once
#undef UNICODE
#define COMMON_H_
#define WIN32_LEAN_AND_MEAN


#include <iostream>
#include <fstream>
#include <thread>
#include <cstdint>
#ifndef MACROS
#define MACROS
#define PORT 1111
#define IP "10.100.102.25"
#define SIZE (int)1e9
#define FAMILY AF_INET
#define TYPE SOCK_STREAM
#endif

using namespace std;

typedef enum { Logout, Signup, Login, Download, Upload, Share, Status, Null } Operation;
