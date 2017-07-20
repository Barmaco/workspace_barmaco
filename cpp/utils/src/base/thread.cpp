#include "thread.h"

#ifdef WIN32

#pragma comment( lib, "Advapi32.lib" )

//////////////////////////////////////////////////////////////////////////////
// initialize_sa
//
bool initialize_sa(SECURITY_ATTRIBUTES& sa)
{
#ifndef WINCE
	if (!InitializeSecurityDescriptor(sa.lpSecurityDescriptor,
		SECURITY_DESCRIPTOR_REVISION))
	{
		MCS_ASSERT(false);
		return false;
	}

	//
	// The default security setting for NT services and applications
	// is different. By default, the securable objects created in
	// NT services are not accessable from other services and applications
	//
	// Add a NULL ACL to the DACL, make it can be accessed
	// by other services and applications
	//
	if (!SetSecurityDescriptorDacl(sa.lpSecurityDescriptor,
		TRUE, (PACL)NULL, FALSE))
	{
		MCS_ASSERT(false);
		return false;
	}
#endif
	return true;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// WIN32 security utility function
//
#define DECLARE_SA(sa)                                         \
	SECURITY_ATTRIBUTES sa;                                    \
	char pSD_##sa[SECURITY_DESCRIPTOR_MIN_LENGTH];             \
	sa.nLength = sizeof(sa);                                   \
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)pSD_##sa;  \
	sa.bInheritHandle = TRUE;

/////////////////////////////////////////////////////////////////////////////
// ThreadMutex
//

ThreadMutex::ThreadMutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_mutex);
#endif

#ifdef UNIX
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	int ret_val = pthread_mutex_init(&m_mutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
	MCS_ASSERT(ret_val == 0);
#endif
}

ThreadMutex::~ThreadMutex()
{
#ifdef WIN32
	DeleteCriticalSection(&m_mutex);
#endif

#ifdef UNIX
	int ret_val = pthread_mutex_destroy(&m_mutex);
	MCS_ASSERT(ret_val == 0);
#endif
}

int ThreadMutex::acquire()
{
#ifdef WIN32
	EnterCriticalSection(&m_mutex);
	return 0;
#endif

#ifdef UNIX
	int ret_val = pthread_mutex_lock(&m_mutex);
	MCS_ASSERT(ret_val == 0);

	if (ret_val == 0) return 0;
	return -1;
#endif
}

int ThreadMutex::try_acquire()
{
#ifdef WIN32
	return TryEnterCriticalSection(&m_mutex);
#endif

#ifdef UNIX
	int err = pthread_mutex_trylock(&m_mutex);
	if (err == 0) return 0;
	return -1;
#endif
}

int ThreadMutex::release()
{
#ifdef WIN32
	LeaveCriticalSection(&m_mutex);
	return 0;
#endif

#ifdef UNIX
	pthread_mutex_unlock(&m_mutex);
	return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// T120_Lock
//
T120_Lock::T120_Lock(ThreadMutex& cs):cs_reference(cs)
{
	cs_reference.acquire();
}

T120_Lock::~T120_Lock()
{
	cs_reference.release();
}

/////////////////////////////////////////////////////////////////////////////
// ThreadEvent
//
ThreadEvent::ThreadEvent()
{
#ifdef WIN32
	m_event_handle = NULL;
#endif

#ifdef UNIX
	int ret = pthread_mutex_init(&m_cv_mutex, NULL);
	MCS_ASSERT(ret == 0);

	ret = pthread_cond_init(&m_cv, NULL);
	MCS_ASSERT(ret == 0);
	flag = 0;
#endif
}

ThreadEvent::~ThreadEvent()
{
#ifdef WIN32
	if (m_event_handle)
		CloseHandle(m_event_handle);
	m_event_handle = NULL;
#endif

#ifdef UNIX
	int ret = pthread_mutex_destroy(&m_cv_mutex);
	MCS_ASSERT(ret == 0);
	ret = pthread_cond_destroy(&m_cv);
	MCS_ASSERT(ret == 0);
#endif
}

bool ThreadEvent::create()
{
#ifdef WIN32
	MCS_ASSERT(!m_event_handle);
	if (!m_event_handle)
	{
		DECLARE_SA(sa);
		initialize_sa(sa);
		m_event_handle = CreateEvent(&sa, true, false,
			NULL);
	}
	return m_event_handle != NULL;
#endif

#ifdef UNIX
	return true;
#endif
}

void ThreadEvent::set_event()
{
#ifdef WIN32
	MCS_ASSERT(m_event_handle);
	if (m_event_handle)
		::SetEvent(m_event_handle);
#endif

#ifdef UNIX
	int ret = pthread_mutex_lock(&m_cv_mutex);
	MCS_ASSERT(ret == 0);

	if (flag == 0)
	{
		flag = 1;
		//pthread_cond_broadcast(&cv);
		pthread_cond_signal(&m_cv);
	}

	pthread_mutex_unlock(&m_cv_mutex);
#endif
}

void ThreadEvent::reset_event()
{
#ifdef WIN32
	MCS_ASSERT(m_event_handle);
	if (m_event_handle)
		::ResetEvent(m_event_handle);
#endif

#ifdef UNIX
#endif
}

bool ThreadEvent::wait(uint32 time_out)
{
#ifdef WIN32
	MCS_ASSERT(m_event_handle);
	if (WaitForSingleObject(m_event_handle, time_out) == WAIT_OBJECT_0)
		return true;
	return false;
#endif

#ifdef UNIX
	int ret = pthread_mutex_lock(&m_cv_mutex);
	MCS_ASSERT(ret == 0);
	//Henry add
	if(1 == flag)
	{
		flag = 0;
		pthread_mutex_unlock(&m_cv_mutex);
		return true;
	}
	//end
	if (time_out == INFINITE)
	{
		ret = pthread_cond_wait(&m_cv, &m_cv_mutex);
	}
	else
	{
		struct timeval curtime;
		struct timespec abstime;

		gettimeofday(&curtime, NULL);
		curtime.tv_usec += time_out*1000;
		while (curtime.tv_usec >= 1000000)
		{
			curtime.tv_sec++;
			curtime.tv_usec -= 1000000;
		}
		abstime.tv_sec = curtime.tv_sec;
		abstime.tv_nsec = curtime.tv_usec*1000;

		ret = pthread_cond_timedwait(&m_cv, &m_cv_mutex,
			&abstime);
	}
	//Henry add
	flag = 0;
	//end
	pthread_mutex_unlock(&m_cv_mutex);

	switch (ret)
	{
	case 0 :
		break;

	case ETIME :
	case ETIMEDOUT :
		return false;

	case EFAULT:
		MCS_ASSERT(false);
		break;

	case EINVAL:
		MCS_ASSERT(false);
		break;

	default :
		MCS_ASSERT(false);
		//CM_ERRTRACE("GCM_Event::wait errno = "<<errno);
		break;
	}

	return true;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CCmThread
//

CCmThread::CCmThread()
{
	m_thread_handle = (THREAD_HANDLE)NULL;
	m_stop_flag = false;
	bool ret = m_stop_event.create();
	MCS_ASSERT(ret);

	ret = m_start_event.create();;
	MCS_ASSERT(ret);

#ifdef WIN32
	m_thread_id = 0;
#endif
}

CCmThread::~CCmThread()
{
	stop();
}

bool CCmThread::create()
{
	MCS_ASSERT(m_thread_handle == (THREAD_HANDLE)NULL);
	m_stop_flag = false;
	m_stop_event.reset_event();
	m_start_event.reset_event();
#ifdef WIN32
	m_thread_handle = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)thread_proc,
		(LPVOID)this,
		0,
		&m_thread_id);

	MCS_ASSERT(m_thread_handle);
	MCS_ASSERT(m_thread_id != 0);

	return m_thread_handle != NULL;
#endif

#ifdef UNIX
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int ret = pthread_create(&m_thread_handle, &attr,
		CCmThread::thread_proc, this);
	pthread_attr_destroy(&attr);

	MCS_ASSERT(ret == 0);
	MCS_ASSERT(m_thread_handle);
	return m_thread_handle != (THREAD_HANDLE)NULL;
#endif
}

bool CCmThread::wait_ready(uint32 time_out)
{
	return m_start_event.wait(time_out);
}

bool CCmThread::stop(uint32 time_out)
{
	MCS_ASSERT(!is_self_thread());
	if (is_self_thread() && time_out == INFINITE)
		return false;

	bool ret = false;
	if (m_thread_handle != (THREAD_HANDLE)NULL)
	{
		m_stop_flag = true;
		if (!m_stop_event.wait(time_out))
		{
#ifdef WIN32
			MCS_ASSERT(false);
			TerminateThread(m_thread_handle, 1);
			CloseHandle(m_thread_handle);
#endif

#ifdef UNIX
			pthread_cancel(m_thread_handle);
			m_stop_event.wait(time_out);
#endif
		}
		else
			ret = true;

		m_thread_handle = (THREAD_HANDLE)NULL;
		usleep(5000);//5毫秒
		return ret;
	}

	return true;
}

bool CCmThread::is_self_thread()
{
#ifdef WIN32
	return m_thread_id == GetCurrentThreadId();
#endif

#ifdef UNIX
	return m_thread_handle == pthread_self();
#endif
}

THREAD_HANDLE CCmThread::get_thread_handle()
{
	return m_thread_handle;
}

uint32 CCmThread::get_thread_id()
{
#ifdef WIN32
	return m_thread_id;
#endif

#ifdef UNIX
	return (uint32)m_thread_handle;
#endif
}

#ifdef WIN32
uint32 WINAPI CCmThread::thread_proc(void* param)
{
	MCS_ASSERT(param);
	CCmThread * my_thread = (CCmThread*)param;

	my_thread->m_start_event.set_event();
	my_thread->thread_run();

	if (my_thread->m_thread_handle)
		CloseHandle(my_thread->m_thread_handle);

	my_thread->m_thread_handle = (THREAD_HANDLE)NULL;
	my_thread->m_thread_id = 0;
	my_thread->m_stop_event.set_event();

	my_thread->thread_cleanup(); //尝试解决关闭应用报错的问题 TODO 黄山

	return 0;
}
#endif

#ifdef UNIX
void* CCmThread::thread_proc(void* param)
{
	MCS_ASSERT(param);
	CCmThread* my_thread = (CCmThread*)param;

	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
	pthread_cleanup_push(CCmThread::thread_cleanup_proc, my_thread);

	my_thread->m_start_event.set_event();
	my_thread->thread_run();
	pthread_cleanup_pop(1);

	my_thread->m_thread_handle = (THREAD_HANDLE)NULL;

	my_thread->m_stop_event.set_event();

	return 0;
}

void CCmThread::thread_cleanup_proc(void* param)
{
	MCS_ASSERT(param);
	CCmThread* my_thread = (CCmThread*)param;

	my_thread->m_thread_handle = (THREAD_HANDLE)NULL;
	my_thread->m_stop_event.set_event();

	my_thread->thread_cleanup();
}
#endif