#ifndef __CWORKER_THREAD_H  
#define __CWORKER_THREAD_H  

#include "cq_stdlib.h"
#include "mapdal/mapbar_dal_pub.h"

#include "CJob.h"

class CWorkerThread;

class IWorkerThreadListener
{
public:
	virtual void onJobFinished(CWorkerThread* busythread, CJob* job)
	{
		UNUSED_VAR(busythread);
		delete(job);
	}
};

class CWorkerThread
{
public:
	Handle	m_workMutex;

	CWorkerThread();
	virtual ~CWorkerThread();
	//void	Run();
	void    execute(CJob* job);
	void    executeSelf(CJob* job);
	CJob*   getWorkingJob(void);
	void	destroy();
	friend int runProcess(void* para);

	IWorkerThreadListener* getListener();
	void addListener(IWorkerThreadListener* listener);
	void removeListener();

	void start();

	void setWorkStatus(bool isBusy);
	bool getWorkStatus();
	bool getThreadWaitStatus();
	void addWaitCnt();
	void decWaitCnt();
private:
	CJob* m_job;

	Handle m_threadHandle;
	Handle m_jobMutex;

	Handle m_jobCond;

	bool m_isEnd;
	IWorkerThreadListener* m_listener;
	Handle m_busyMutex;
	Handle m_cntMutex;
	bool m_isBusy;
	int32 m_threadWaitCnt;
};



#endif