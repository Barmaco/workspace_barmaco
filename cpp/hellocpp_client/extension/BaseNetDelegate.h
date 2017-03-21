/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#pragma once

#include <stdio.h>
#include "netmacros.h"
#include "pthread.h"
#include "Buffer.h"
#include <list>

namespace extension
{

class BaseNetDelegate;

struct ps
{
public:
	BaseNetDelegate* obj;
	SOCKET st;
};

enum SocketStatus
{
	eSocketConnected = 1,
	eSocketConnecting = 2,
	eSocketDisconnected = 3,
	eSocketConnectFailed = 4,
	eSocketConnectTimeout = 5,
	eSocketReceive = 6,
	eSocketIoClosed = 0,
	eSocketIoError = -1,
	eSocketCreateFailed = -2,
};

class BaseNetDelegate
{
protected:
	struct SENDBUFFER
	{
		char* pBuffer;       // the data
		int nOffset;         // the send data offset
		int nLength;         // data's length
		SOCKET st;
	};

protected:
	SOCKET m_nclientst;

	SocketStatus m_eStatus;

	std::list<SENDBUFFER>	m_lSendBuffers;
	pthread_mutex_t			m_lSendBuffersMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t			m_lSendBuffersCond = PTHREAD_COND_INITIALIZER;

	BaseNetDelegate();
	virtual ~BaseNetDelegate();

	static void* runsendthread(void *context);
	void sendthread();
	static void* runrecvthread(void *context);
	void recvthread(SOCKET st);

protected:
	int initsocket();
	SOCKET socket_accept(SOCKET listen_st);
	SOCKET socket_create(int port);
	SOCKET socket_connect(const char* hostname, int port);

public:
	void sendm(SOCKET st, char* pBuffer, unsigned int uLen);
	void sendm(SOCKET st, Buffer& pBuffer);
	void sendm(char* pBuffer, unsigned int uLen);
	void sendm(Buffer& pBuffer);

	virtual void onMessageReceived(SOCKET st, Buffer& buffer) = 0;
};

}