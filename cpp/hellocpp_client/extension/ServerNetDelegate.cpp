/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#include "ServerNetDelegate.h"

namespace extension
{

ServerNetDelegate::ServerNetDelegate()
: m_nport(8080)
{

}

ServerNetDelegate::~ServerNetDelegate()
{

}

void ServerNetDelegate::startserver(int port)
{
	m_nport = port;

	pthread_t m_pmainthread;
	pthread_create(&m_pmainthread, NULL, &ServerNetDelegate::runmainthread, this);
	pthread_detach(m_pmainthread);
}

void* ServerNetDelegate::runmainthread(void *context)
{
	ServerNetDelegate* obj = (ServerNetDelegate*)(context);
	obj->mainthread();
	return NULL;
}

void ServerNetDelegate::mainthread()
{
	SOCKET listen_st = socket_create(m_nport);
	if (listen_st == 0)
	{
		return;
	}

	pthread_t m_psendthread;
	pthread_create(&m_psendthread, NULL, &ServerNetDelegate::runsendthread, this);
	pthread_detach(m_psendthread);

	while (true)
	{
		SOCKET st = socket_accept(listen_st);
		if (st == 0)
		{
			return;
		}

		struct ps psobj;
		psobj.obj = this;
		psobj.st = st;

		pthread_t receivethread;
		pthread_create(&receivethread, NULL, &ServerNetDelegate::runrecvthread, &psobj);
		pthread_detach(receivethread);				//设置线程为可分离

		/*
		#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
		closesocket(st);
		#else
		close(st);
		#endif
		*/
	}

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
	closesocket(listen_st);
	WSACleanup();
#else
	close(listen_st);
#endif
}

}