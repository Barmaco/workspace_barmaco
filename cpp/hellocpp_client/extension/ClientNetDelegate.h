/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#pragma once
#include "BaseNetDelegate.h"
#include <vector>
#include <list>
#include "Buffer.h"

namespace extension
{

class ClientNetDelegate : public BaseNetDelegate
{
protected:
	ClientNetDelegate();
	virtual ~ClientNetDelegate();

private:
	char					m_pip[32];
	unsigned int			m_uport;

	static void* runmainthread(void *context);
	void mainthread();
public:
	void startserver(const char* ip, const int port);
};

}
