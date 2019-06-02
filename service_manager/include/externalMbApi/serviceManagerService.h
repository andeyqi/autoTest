#ifndef __SERVICE_MANAGER_SERVICE_H__
#define __SERVICE_MANAGER_SERVICE_H__

#include "serviceManagerListener.h"

extern "C"
{
	typedef size_t Handle;
	
	Handle Mapbar_createMutex(void);
	void Mapbar_lockMutex(Handle h);
	BOOL Mapbar_tryLockMutex(Handle h);
	void Mapbar_unlockMutex(Handle h);
	void Mapbar_destroyMutex(Handle h);
}


template <class T>
class ServiceManagerService : public ServiceMangerListener<T>
{
public:

	ServiceManagerService()
	:m_isStarted(false)
	{
		m_mutex = Mapbar_createMutex();
	}
	
	virtual ~ServiceManagerService()
	{
		Mapbar_destroyMutex(m_mutex);
		m_isStarted = false;
	}
	
	virtual bool startService()
	{
		Mapbar_lockMutex(m_mutex);

		if (!m_isStarted)
			m_isStarted = InitInstance();

		Mapbar_unlockMutex(m_mutex);

		return m_isStarted;
	}
	
	virtual bool stopService()
	{
		Mapbar_lockMutex(m_mutex);

		if (m_isStarted)
			m_isStarted = ExitInstance();

		Mapbar_unlockMutex(m_mutex);

		return !m_isStarted;
	}
	
	virtual bool isStarted() { return m_isStarted; }
	
protected:

	virtual bool InitInstance() = 0;
	virtual bool ExitInstance() = 0;

// protected:
// 
// 	virtual void OnListenerAdded(T* listener)
// 	{
// 		printf("ServiceManagerService OnListenerAdded\n");
// 		//ServiceMangerListener<T> OnListenerAdded(listener);
// 	}
// 
// 	virtual void OnListenerRemoved(T* listener)
// 	{
// 		printf("ServiceManagerService OnListenerRemoved\n");
// 		//ServiceMangerListener<T> OnListenerRemoved(listener);
// 	}
	
private:
	bool m_isStarted;
	Handle m_mutex;
};

#endif // __SERVICE_MANAGER_SERVICE_H__
