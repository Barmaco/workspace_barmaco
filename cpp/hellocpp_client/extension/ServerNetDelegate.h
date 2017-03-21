/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#pragma once
#include "BaseNetDelegate.h"
#include "Buffer.h"

namespace extension
{


class ServerNetDelegate : public BaseNetDelegate
{
protected:
	ServerNetDelegate();
	virtual ~ServerNetDelegate();

private:
	int m_nport;

	static void* runmainthread(void *context);
	void mainthread();

public:
	void startserver(int port);
};

}