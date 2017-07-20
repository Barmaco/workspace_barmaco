#pragma once

#include "base.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// ThreadMutex
//
class ThreadMutex
{
public:
	ThreadMutex();
	~ThreadMutex();

	int   acquire();
	int   try_acquire();
	int   release();

protected:
	DISALLOW_EVIL_CONSTRUCTORS(ThreadMutex);

protected :
#if defined(WIN32)
	CRITICAL_SECTION	m_mutex;
#endif

#if defined(UNIX)
	pthread_mutex_t		m_mutex;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// T120_Lock
//
class T120_Lock
{
public :
	T120_Lock(ThreadMutex &cs);
	virtual ~T120_Lock();

protected :
	ThreadMutex& cs_reference;
};

/////////////////////////////////////////////////////////////////////////////
// ThreadEvent
//
class ThreadEvent
{
public :
	ThreadEvent();
	~ThreadEvent();

public :
	bool create();
	void set_event();
	void reset_event();

	bool wait(uint32 time_out = INFINITE);

protected :
#ifdef WIN32
	THREAD_HANDLE	m_event_handle;
#endif

#ifdef UNIX
	pthread_mutex_t m_cv_mutex;
	pthread_cond_t  m_cv;
	int             flag;
#endif
};

/*工作线程基类 
核心是thread_run 方法
*/
class  CCmThread
{
public :
	CCmThread();
	virtual ~CCmThread();

public :
	bool create();
	bool stop(uint32 time_out = INFINITE);
	bool is_self_thread();

	THREAD_HANDLE get_thread_handle();
	uint32 get_thread_id();
	bool   wait_ready(uint32 time_out);

public :
	virtual uint32 thread_run() = 0;
	virtual uint32 thread_cleanup() { return 0; }

#ifdef WIN32
	static uint32 WINAPI thread_proc(void* param);
#endif

#ifdef UNIX
	static void* thread_proc(void* param);
	static void thread_cleanup_proc(void* param);
#endif

protected :
	THREAD_HANDLE        m_thread_handle;
	bool                m_stop_flag;
	ThreadEvent          m_stop_event;
	ThreadEvent          m_start_event;

#ifdef WIN32
	uint32               m_thread_id;
#endif
};