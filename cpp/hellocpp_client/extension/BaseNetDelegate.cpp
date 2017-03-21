/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#include "BaseNetDelegate.h"

namespace extension
{

BaseNetDelegate::BaseNetDelegate()
: m_nclientst(0)
{
}

BaseNetDelegate::~BaseNetDelegate()
{
	pthread_mutex_lock(&m_lSendBuffersMutex);
	while (!m_lSendBuffers.empty())
	{
		delete[] m_lSendBuffers.front().pBuffer;
		m_lSendBuffers.pop_front();
	}
	pthread_mutex_unlock(&m_lSendBuffersMutex);
}

int BaseNetDelegate::initsocket()
{
#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
	// 加载socket动态链接库(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// 这结构是用于接收Wjndows Socket的结构信息的
	int err;

	wVersionRequested = MAKEWORD(1, 1);	// 请求1.1版本的WinSock库

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;			// 返回值为零的时候是表示成功申请WSAStartup
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		// 检查这个低字节是不是1，高字节是不是1以确定是否我们所请求的1.1版本
		// 否则的话，调用WSACleanup()清除信息，结束函数
		WSACleanup();
		return -1;
	}
#endif
	return 0;
}

SOCKET BaseNetDelegate::socket_connect(const char* hostname, int port)
{
	if (initsocket() == -1)
	{
		return 0;
	}

	SOCKET st = socket(AF_INET, SOCK_STREAM, 0);

	if (st == 0)
	{
		return 0;
	}

	struct sockaddr_in addr;					//定义一个IP地址的结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;					//设置结构地址类型为TCP/IP地址
	addr.sin_port = htons(port);				//指定一个端口号：8080，htons:将short类型从host字节类型到net字节类型转化
	addr.sin_addr.s_addr = inet_addr(hostname);	//将字符串类型的IP地址转化为int，赋给addr结构成员.

	//调用connect连接到结构addr指定的IP地址和端口号
	if (connect(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("connect failed %s\n", strerror(errno));
		return 0;
	}

	return st;
}

SOCKET BaseNetDelegate::socket_create(int port)
{
	if (initsocket() == -1)
	{
		return 0;
	}

	SOCKET st = socket(AF_INET, SOCK_STREAM, 0);

	if (st == 0)
	{
		return 0;
	}

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
	const char on = 0;
#else
	int on = 0;
#endif

	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		return 0;
	}

	struct sockaddr_in addr;					//定义一个IP地址结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;					//将addr结构的属性定位为TCP/IP地址
	addr.sin_port = htons(port);				//将本地字节顺序转化为网络字节顺序。
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	//INADDR_ANY代表这个server上所有的地址

	//将IP与server程序绑定
	if (bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("bind failed %s\n", strerror(errno));
		return 0;
	}

	//server端开始listen，
	if (listen(st, 20) == -1)
	{
		printf("listen failed %s\n", strerror(errno));
		return 0;
	}

	printf("listen %d success\n", port);

	return st;
}

SOCKET BaseNetDelegate::socket_accept(SOCKET listen_st)
{
	SOCKET client_st = 0;					//client端socket
	struct sockaddr_in client_addr;		//表示client端的IP地址
	memset(&client_addr, 0, sizeof(client_addr));

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
	int len = 0;
#else
	unsigned int len = 1;
#endif
	len = sizeof(client_addr);

	//accept会阻塞，直到有客户端连接过来，accept返回client的socket描述符
	client_st = accept(listen_st, (struct sockaddr *)&client_addr, &len);
	if (client_st == -1)
	{
		printf("accept failed %s\n", strerror(errno));
		return 0;
	}

	printf("accept by %s\n", inet_ntoa(client_addr.sin_addr));
	return client_st;
}

void BaseNetDelegate::sendm(SOCKET st, char* pBuffer, unsigned int uLen)
{
	if (!pBuffer || uLen == 0)
		return;

	Buffer pBuf;
	pBuf.writeData(pBuffer, uLen);
	sendm(st, pBuf);
}

void BaseNetDelegate::sendm(SOCKET st, Buffer& pBuffer)
{
	if (pBuffer.empty())
		return;

	unsigned int u_len = pBuffer.getContentSize();
	pBuffer.moveRight(sizeof(unsigned int));
	pBuffer.moveWriterIndexToFront();
	pBuffer.writeUInt(u_len);

	pBuffer.moveReaderIndexToFront();
	char* pData = pBuffer.readWholeData();	//需要delete
	int nLength = (int)pBuffer.getContentSize();
	pBuffer.moveReaderIndexToFront();

	SENDBUFFER tBuf;
	tBuf.pBuffer = pData;
	tBuf.nLength = nLength;
	tBuf.nOffset = 0;
	tBuf.st = st;

	pthread_mutex_lock(&m_lSendBuffersMutex);
	m_lSendBuffers.push_back(tBuf);
	pthread_cond_signal(&m_lSendBuffersCond);
	pthread_mutex_unlock(&m_lSendBuffersMutex);
}

void BaseNetDelegate::sendm(char* pBuffer, unsigned int uLen)
{
	sendm(m_nclientst, pBuffer, uLen);
}

void BaseNetDelegate::sendm(Buffer& pBuffer)
{
	sendm(m_nclientst, pBuffer);
}

void* BaseNetDelegate::runsendthread(void *context)
{
	BaseNetDelegate* obj = (BaseNetDelegate*)(context);
	obj->sendthread();
	return NULL;
}

void BaseNetDelegate::sendthread()
{
	while (true)
	{
		pthread_mutex_lock(&m_lSendBuffersMutex);
		/*
		需要注意的一点是在thread4中使用的while (iCount < 100),
		而不是if (iCount < 100)。
		这是因为在pthread_cond_singal()和pthread_cond_wait()返回之间有时间差，
		假如在时间差内，thread3又将iCount减到了100以下了，
		那么thread4就需要在等待条件为真了。
		*/
		while (m_lSendBuffers.empty())
		{
			pthread_cond_wait(&m_lSendBuffersCond, &m_lSendBuffersMutex);
		}
		SENDBUFFER& tBuffer = m_lSendBuffers.front();

		int nRet = send(tBuffer.st, tBuffer.pBuffer + tBuffer.nOffset, tBuffer.nLength - tBuffer.nOffset, 0);

		if (nRet == -1)
		{
			break;
		}
		else if (nRet == tBuffer.nLength - tBuffer.nOffset)
		{
			delete[] tBuffer.pBuffer;
			m_lSendBuffers.pop_front();
		}
		else
		{
			tBuffer.nOffset += nRet;
		}

		pthread_mutex_unlock(&m_lSendBuffersMutex);
	}
}

void* BaseNetDelegate::runrecvthread(void *context)
{
	struct ps *psobj = (struct ps *)context;

	BaseNetDelegate *obj = (BaseNetDelegate *)psobj->obj;
	obj->recvthread(psobj->st);
	return NULL;
}

void BaseNetDelegate::recvthread(SOCKET st)
{
	char readbuf[SOCKET_READ_BUFFER_SIZE];
	extension::Buffer databuf;

	while (1)
	{
		memset(readbuf, 0, sizeof(readbuf));
		int rc = recv(st, readbuf, sizeof(readbuf), 0);
		if (rc <= 0)				//如果recv返回小于等于0，代表socket已经关闭或者出错了
			break;

		databuf.writeData(readbuf, (unsigned int)rc);

		//检查数据包
		while (databuf.isReadable(HEAD_SIZE))
		{
			int n_head_len = databuf.readInt();
			if (n_head_len <= 0)
			{
				databuf.moveLeft(sizeof(int));
				printf("invalidate head length");
				break;
			}

			int contentlen = databuf.getContentSize();

			if (contentlen - sizeof(int) >= n_head_len)
			{
				databuf.moveLeft(sizeof(int));
				Buffer pData;
				databuf.readData(&pData, n_head_len);
				databuf.moveLeft(n_head_len);
				databuf.moveReaderIndexToFront();
				databuf.moveWriterIndexToBack();

				onMessageReceived(st, pData);
			}
			else
			{
				databuf.moveReaderIndexToFront();
				break;
			}
		}
	}

#if (TARGET_PLATFORM == TARGET_PLATFORM_WIN)
	closesocket(st);
#else
	close(st);
#endif
}

}