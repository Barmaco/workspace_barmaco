/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#pragma once

#define TARGET_PLATFORM_WIN 0
#define TARGET_PLATFORM_LINUX 1

#ifndef TARGET_PLATFORM
#define TARGET_PLATFORM TARGET_PLATFORM_WIN
#endif

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
#include <windows.h>
#elif (TARGET_PLATFORM == TARGET_PLATFORM_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#endif

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
#pragma comment(lib, "Ws2_32.lib")
#elif (TARGET_PLATFORM == TARGET_PLATFORM_LINUX)
#endif

#if (TARGET_PLATFORM == TARGET_PLATFORM_LINUX)
typedef unsigned int SOCKET;
#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (SOCKET)(~0)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif
#endif

#ifndef SOCKET_READ_BUFFER_SIZE
#define SOCKET_READ_BUFFER_SIZE 8192
#endif
#ifndef BUFFER_ORGINAL_SIZE
#define BUFFER_ORGINAL_SIZE 512
#endif
#ifndef HEAD_SIZE
#define HEAD_SIZE 4
#endif

#ifndef SINGLE_DELEGATE_INSTANCE_FUNC
#define SINGLE_DELEGATE_INSTANCE_FUNC(_CLASS_) \
static _CLASS_* sharedDelegate() \
{ \
	static _CLASS_* pRet = NULL; \
	if (!pRet) pRet = new _CLASS_(); \
	return pRet; \
}
#endif //SINGLE_DELEGATE_INSTANCE_FUNC