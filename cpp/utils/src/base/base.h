#pragma once

#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
using namespace std;

//define base data types
#ifndef uint32
typedef unsigned long	uint32;
#endif

#ifndef int32
typedef  long			int32;
#endif

#ifndef uint16
typedef unsigned short	uint16;
#endif

#if !defined(int32_t)
typedef signed int		int32_t;
#endif

//win32 define
#ifdef WIN32

#define THREAD_HANDLE HANDLE
#define snprintf _snprintf
#define usleep(x)    Sleep((x) / 1000)

#endif

//不为类定义拷贝构造函数或者赋值操作符
//DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_EVIL_CONSTRUCTORS(TypeName)  \
	TypeName(const TypeName&);                \
	TypeName& operator=(const TypeName&)

#define MCS_ASSERT(x)	if (!(x)) printf("%s\n", #x);