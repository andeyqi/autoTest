#include "stdafx_serviceManager.h"
#include "CWorkerThread.h"
#include "CThreadPool.h"
#define ThreadPool	"SM_ThreadPool"

/**
*	@brief
*		线程执行函数
*/
int runProcess(void* para)
{
	CWorkerThread* work = (CWorkerThread*)para;
	
	if (work->m_threadHandle != invalidHandle)
		Mapbar_setThreadName(work->m_threadHandle, ThreadPool);

	while (!work->m_isEnd)
	{
		CQ_LOG_INFO("[ThreadPool] CThreadPool::runProcess().\n");
		if (work->m_isEnd)
		{
			CQ_LOG_INFO("[ThreadPool] CThreadPool::runProcess() m_isEnd == true");
			break;
		}
			
		if (work->m_job == NULL)
		{
			Mapbar_waitEvent(work->m_jobCond);
			continue;
		}
		//else
		//{	
		//	//此处调用Mapbar_setEvent时，下次Mapbar_waitEvent不生效。
		//	Mapbar_setEvent(work->m_jobCond);
		//}
		work->m_job->run();

		//delete(work->m_job);
		CJob * job = work->m_job;
		work->m_job = NULL;
		
		//Mapbar_unlockMutex(work->m_workMutex);
//		work->m_threadPool->moveToIdleList(work);

		if (work->getListener() != NULL)
			work->getListener()->onJobFinished(work, job);

		//if (work->m_threadPool->m_IdleList.size() > 5) //work->m_threadPool->GetAvailHighNum())
		//{
		//	work->m_threadPool->deleteIdleThread(work->m_threadPool->m_IdleList.size() - 5);//work->m_threadPool->GetInitNum());
		//}


	}
	return D_NAVISYS_OK;
}

IWorkerThreadListener* CWorkerThread::getListener()
{
	return m_listener;
}

void CWorkerThread::addListener(IWorkerThreadListener* listener)
{
	m_listener = listener;
}

void CWorkerThread::removeListener()
{
	m_listener = NULL;
}

CWorkerThread::CWorkerThread() :
m_isBusy(false),
m_threadWaitCnt(0)
{
	m_job = NULL;
	m_isEnd = false;
	
	m_jobCond = Mapbar_createEvent(FALSE, FALSE);
	m_workMutex = Mapbar_createMutex();
	m_jobMutex = Mapbar_createMutex();
	m_threadHandle = invalidHandle;
	m_busyMutex = Mapbar_createMutex();
	m_cntMutex = Mapbar_createMutex();
	m_listener = NULL;
}

CWorkerThread::~CWorkerThread()
{
	CQ_LOG_INFO("[ThreadPool]  CWorkerThread::~CWorkerThread(). \n");

	m_isEnd = true;
	m_job = NULL;

	Mapbar_destroyMutex(m_workMutex);
	Mapbar_destroyMutex(m_jobMutex);
	Mapbar_destroyEvent(m_jobCond);
	Mapbar_destroyMutex(m_busyMutex);
	Mapbar_destroyMutex(m_cntMutex);

}

void CWorkerThread::start()
{
	m_threadHandle = Mapbar_createThread(runProcess, this);
	//Mapbar_setThreadName(m_threadHandle, ThreadPool);
}

void CWorkerThread::destroy()
{
	CQ_LOG_INFO("[ThreadPool]  CWorkerThread::Destroy(). \n");
	m_isEnd = true;
	// 通知线程解除Wait
	Mapbar_setEvent(m_jobCond);
	Mapbar_waitThread(m_threadHandle);
	Mapbar_closeThread(m_threadHandle);
	m_threadHandle = invalidHandle;
}

void CWorkerThread::execute(CJob* job)
{
	addWaitCnt();
	while (getWorkStatus() == true)
	{
		Mapbar_sleep(200);		
	};
	setWorkStatus(true);
	decWaitCnt();
	CQ_LOG_INFO("[ThreadPool]  CWorkerThread::setJob(). \n");
	Mapbar_lockMutex(m_jobMutex);
	m_job = job;
	Mapbar_unlockMutex(m_jobMutex);
	Mapbar_setEvent(m_jobCond);
}

void CWorkerThread::executeSelf(CJob* job)
{
	Mapbar_lockMutex(m_jobMutex);
	m_job = job;
	Mapbar_unlockMutex(m_jobMutex);
}

CJob* CWorkerThread::getWorkingJob(void)
{
	Mapbar_lockMutex(m_jobMutex);
	CJob* job = m_job;
	Mapbar_unlockMutex(m_jobMutex);
	
	return job;
}
void CWorkerThread::setWorkStatus(bool isBusy)
{
	Mapbar_lockMutex(m_busyMutex);
	m_isBusy = isBusy;
	Mapbar_unlockMutex(m_busyMutex);
}
bool CWorkerThread::getWorkStatus()
{
	return m_isBusy;
}
bool CWorkerThread::getThreadWaitStatus()
{
	return m_threadWaitCnt;
}
void CWorkerThread::addWaitCnt()
{
	Mapbar_lockMutex(m_cntMutex);
	m_threadWaitCnt++;
	Mapbar_unlockMutex(m_cntMutex);
}
void CWorkerThread::decWaitCnt()
{
	Mapbar_lockMutex(m_cntMutex);
	m_threadWaitCnt--;
	Mapbar_unlockMutex(m_cntMutex);
}