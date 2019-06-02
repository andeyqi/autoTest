#ifndef __SERVICE_MANAGER_LISTENER_H__
#define __SERVICE_MANAGER_LISTENER_H__

#include <set>

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
class ServiceMangerListener
{
public:
	ServiceMangerListener()
	{
		m_mutex = Mapbar_createMutex();
	}
	
	virtual ~ServiceMangerListener()
	{
		Mapbar_destroyMutex(m_mutex);
	}
	
	virtual void addListerner(T* listener)
	{
		Mapbar_lockMutex(m_mutex);
		m_listerners.insert(listener);
		Mapbar_unlockMutex(m_mutex);
		
		OnListenerAdded(listener);
	}
	
	virtual void removeListener(T* listener)
	{
		Mapbar_lockMutex(m_mutex);
		m_listerners.erase(listener);
		Mapbar_unlockMutex(m_mutex);
	
		OnListenerRemoved(listener);	
	}
	
	virtual std::set<T*>* getListerners()
	{
		return &m_listerners;
	}	
	
	virtual int getlistenerNum()
	{
		return m_listerners.size();
	}
	
protected:
	virtual void OnListenerAdded(T* listener)
	{
		printf("ServiceMangerListener OnListenerAdded\n");
		UNUSED_VAR(listener);
	}

	virtual void OnListenerRemoved(T* listener)
	{
		printf("ServiceMangerListener OnListenerRemoved\n");
		UNUSED_VAR(listener);
	}
	
	
private:
	std::set <T*> m_listerners; 
	size_t m_mutex;
};

#endif // __SERVICE_MANAGER_LISTENER_H__
