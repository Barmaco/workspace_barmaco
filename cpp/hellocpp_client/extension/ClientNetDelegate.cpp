/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#include "ClientNetDelegate.h"
#include "Buffer.h"

namespace extension
{

ClientNetDelegate::ClientNetDelegate()
: m_uport(8080)
{
	memset(m_pip, 0, sizeof(m_pip));
}

ClientNetDelegate::~ClientNetDelegate()
{
	
}

void ClientNetDelegate::startserver(const char* ip, const int port)
{
	strcpy(m_pip, ip);
	m_uport = port;

	pthread_t m_pmainthread;
	pthread_create(&m_pmainthread, NULL, &ClientNetDelegate::runmainthread, this);
	pthread_detach(m_pmainthread);
}

void* ClientNetDelegate::runmainthread(void *context)
{
	ClientNetDelegate* obj = (ClientNetDelegate*)(context);
	obj->mainthread();
	return NULL;
}

void ClientNetDelegate::mainthread()
{
	m_nclientst = socket_connect(m_pip, m_uport);
	if (m_nclientst == 0)
	{
		return;
	}

	struct ps psobj;
	psobj.obj = this;
	psobj.st = m_nclientst;
	pthread_t receivethread;
	pthread_create(&receivethread, NULL, &ClientNetDelegate::runrecvthread, &psobj);
	pthread_detach(receivethread);	

	pthread_t m_psendthread;
	pthread_create(&m_psendthread, NULL, &ClientNetDelegate::runsendthread, this);
	pthread_join(m_psendthread, NULL);

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
	closesocket(m_nclientst);
	WSACleanup();
#else
	close(m_nclientst);
#endif
}

}
