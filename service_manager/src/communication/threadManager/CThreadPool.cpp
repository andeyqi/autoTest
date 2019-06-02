#include "CThreadPool.h"
#include "CWorkerThread.h"


//static bool m_Exit = false;

CThreadPool::CThreadPool(int threadNum, int maxNum)
{
	CQ_LOG_INFO("[ThreadPool] CThreadPool::CThreadPool().\n");
	m_threadNum = threadNum;
	if (maxNum > threadNum)
	{
		m_maxNum = maxNum;
	}
	else
	{
		m_maxNum = threadNum;
	}
	m_createNum = 1;
	//m_availLow = availIdleLow;
	//v = availIdleHigh;

	initCommon();
}

void CThreadPool::initCommon()
{
	CQ_LOG_INFO("[ThreadPool] CThreadPool::CThreadPool  m_maxNum = %d  m_threadNum = %d\n", m_maxNum, m_threadNum);
	m_busyMutex = Mapbar_createMutex(); //when visit busy list,use m_busyMutex to lock and unlock 
	m_idleMutex = Mapbar_createMutex();    //when visit idle list,use m_idleMutex to lock and unlock 
	m_jobMutex = Mapbar_createMutex(); //when visit job list,use m_jobMutex to lock and unlock 

	m_busyCond = Mapbar_createEvent(FALSE, FALSE); //m_busyCond is used to sync busy thread list 
	m_idleCond = Mapbar_createEvent(FALSE, FALSE); //m_idleCond is used to sync idle thread list 
	m_idleJobCond = Mapbar_createEvent(FALSE, FALSE);
	m_maxNumCond = Mapbar_createEvent(FALSE, FALSE);

	m_busyList.clear();
	m_IdleList.clear();
	m_JobList.clear();

	for (unsigned int i = 0; i < m_threadNum; i++){
		CWorkerThread* thr = new CWorkerThread();
		thr->addListener(this);
		thr->start();
		appendToIdleList(thr);
	}
}

CThreadPool::~CThreadPool()
{
//#ifdef MAPBAR_WIN32
//	terminateAll();
//#endif
	CQ_LOG_INFO("[ThreadPool] CThreadPool::~CThreadPool() start.\n");
	m_maxNum = 0;
	m_threadNum = 0;
	m_createNum = 0;
	//m_availLow = 0;
	//m_availHigh = 0;

	m_busyList.clear();
	m_IdleList.clear();
	m_JobList.clear();

	Mapbar_destroyMutex(m_busyMutex);
	Mapbar_destroyMutex(m_idleMutex);
	Mapbar_destroyMutex(m_jobMutex);

	Mapbar_destroyEvent(m_busyCond);
	Mapbar_destroyEvent(m_idleCond);
	Mapbar_destroyEvent(m_idleJobCond);
	Mapbar_destroyEvent(m_maxNumCond);
	CQ_LOG_INFO("[ThreadPool] CThreadPool::~CThreadPool() Stop.\n");
}

void CThreadPool::terminateAll()
{	
	///** 避免重复调用 */
	//if (m_Exit)
	//{
	//	return;
	//}
	//m_Exit = true;

	//for (int i = 0; i < m_ThreadList.size(); i++) {
	//	CWorkerThread* thr = m_ThreadList[i];
	//	thr->destroy();
	//	delete thr;
	//	thr->Join();
	//}
	//return;
	//清空忙碌数组 退出线程
	CQ_LOG_INFO("[ThreadPool] CThreadPool::terminateAll() BeforeBusysize = %d \n ", m_busyList.size());
	Mapbar_lockMutex(m_busyMutex);
	for (unsigned int i = 0; i < m_busyList.size(); i++) {
		CWorkerThread* thr = m_busyList[i];
		thr->destroy();
		delete thr;
	}
	m_busyList.clear();
	Mapbar_unlockMutex(m_busyMutex);
	CQ_LOG_INFO("[ThreadPool] CThreadPool::terminateAll() AfterBusysize = %d \n ", m_busyList.size());
	//清空空闲数组 退出线程
	CQ_LOG_INFO("[ThreadPool] CThreadPool::terminateAll() BeforeIdlesize = %d \n ", m_IdleList.size());
	Mapbar_lockMutex(m_idleMutex);
	for (unsigned int i = 0; i < m_IdleList.size(); i++) {
		CWorkerThread* thr = m_IdleList[i];
		thr->destroy();
		delete thr;
	}
	m_IdleList.clear();
	Mapbar_unlockMutex(m_idleMutex);
	CQ_LOG_INFO("[ThreadPool] CThreadPool::terminateAll() AfterIdlesize = %d \n ", m_IdleList.size());
	m_JobList.clear();
	return;
}

CWorkerThread* CThreadPool::getIdleThread(void)
{
	CQ_LOG_INFO("[ThreadPool] CThreadPool::getIdleThread() Idlesize = %d \n ", m_IdleList.size());

	Mapbar_lockMutex(m_idleMutex);

	while (m_IdleList.size() == 0)
		Mapbar_waitEvent(m_idleCond);

	if (m_IdleList.size() > 0)
	{
		CWorkerThread* thr = (CWorkerThread*)m_IdleList.front();
		Mapbar_unlockMutex(m_idleMutex);
		return thr;
	}
	Mapbar_unlockMutex(m_idleMutex);

	return NULL;
}

//add an idle thread to idle list 
void CThreadPool::appendToIdleList(CWorkerThread* jobthread)
{
	CQ_LOG_INFO("[ThreadPool] CThreadPool::appendToIdleList(). \n ");
	Mapbar_lockMutex(m_idleMutex);
	m_IdleList.push_back(jobthread);
	//m_ThreadList.push_back(jobthread);
	Mapbar_unlockMutex(m_idleMutex);
}

//move and idle thread to busy thread 
void CThreadPool::moveToBusyList(CWorkerThread* idlethread)
{
	Mapbar_lockMutex(m_busyMutex);
	CQ_LOG_INFO("[ThreadPool] CThreadPool::moveToBusyList() Before m_IdleList size = %d \n. ", m_IdleList.size());
	m_busyList.push_back(idlethread);
//	m_AvailNum--;
	Mapbar_unlockMutex(m_busyMutex);

	Mapbar_lockMutex(m_idleMutex);
	
	std::vector<CWorkerThread*>::iterator pos;
	for (pos = m_IdleList.begin(); pos != m_IdleList.end();)
		{
			if (*pos == idlethread)
			{
				m_IdleList.erase(pos);    //删除元素，返回值指向已删除元素的下一个位置
				break;
			}
			else
			{
				++pos;
			}
		}
	CQ_LOG_INFO("[ThreadPool] CThreadPool::moveToBusyList() After m_IdleList size = %d \n. ", m_IdleList.size());
	Mapbar_unlockMutex(m_idleMutex);
}

void CThreadPool::onJobFinished(CWorkerThread* busythread, CJob* job)
{
	Mapbar_lockMutex(m_idleMutex);
	//if (this->m_IdleList.size() > (unsigned int)this->getAvailHighNum())
	//{
	//	this->deleteIdleThread(this->m_IdleList.size() - this->getAvailHighNum());
	//}
	//printf("[onJobFinished] this = %p, all = %d, busySize = %d, m_IdleList = %d \n", this, getAllNum(), m_busyList.size(), m_IdleList.size());
	if (this->getAllNum() > m_threadNum && this->m_IdleList.size() > 0)
	{
		if (this->m_busyList.size() > m_threadNum)
		{
			this->deleteIdleThread(this->getAllNum() - this->m_busyList.size());
		}
		else
		{
			this->deleteIdleThread(this->getAllNum() - m_threadNum);
		}

	}

	Mapbar_unlockMutex(m_idleMutex);

	if (m_listener != NULL)
	{
		m_listener->onJobFinished(job);
	}

	if (this->getAllNum() >= m_maxNum)
	{
		CJob * newJob = getJob();
		if (newJob != NULL)
		{
			busythread->executeSelf(newJob);
			return;
		}
		else
		{
		if (busythread->getThreadWaitStatus() > 0)
			{
				busythread->setWorkStatus(false);
				return;
			}
		}
	}

	busythread->setWorkStatus(false);

	Mapbar_lockMutex(m_idleMutex);
	CQ_LOG_INFO("[ThreadPool] CThreadPool::onJobFinished() Before m_busyList size = %d \n. ", m_busyList.size());
	bool isInIdleList = false;
	std::vector<CWorkerThread*>::iterator index;
	for (index = m_IdleList.begin(); index != m_IdleList.end();)
	{
		if (*index == busythread)
		{
			isInIdleList = true; 
			break;
		}
		else
		{
			++index;
		}
	}
	Mapbar_unlockMutex(m_idleMutex);
	if (!isInIdleList)
	{
		Mapbar_lockMutex(m_idleMutex);
		m_IdleList.push_back(busythread);
		//	m_AvailNum++;
		Mapbar_unlockMutex(m_idleMutex);

		Mapbar_lockMutex(m_busyMutex);
		std::vector<CWorkerThread*>::iterator pos;
		for (pos = m_busyList.begin(); pos != m_busyList.end();)
		{
			if (*pos == busythread)
			{
				m_busyList.erase(pos);    //删除元素，返回值指向已删除元素的下一个位置
				break;
			}
			else
			{
				++pos;
			}
		}
		CQ_LOG_INFO("[ThreadPool] CThreadPool::onJobFinished() After m_busyList size = %d \n. ", m_busyList.size());
		Mapbar_unlockMutex(m_busyMutex);
	}
	Mapbar_setEvent(m_idleCond);
	Mapbar_setEvent(m_maxNumCond);
	
}

//create num idle thread and put them to idlelist 
void CThreadPool::createIdleThread(int num)
{
	CQ_LOG_INFO("[ThreadPool] CThreadPool::createIdleThread() TotalList size = %d \n. ", (m_busyList.size() + m_IdleList.size()));
	CQ_LOG_INFO("[ThreadPool] CThreadPool::createIdleThread() CreateNum = %d \n", num);
	for (int i = 0; i<num; i++){
		CWorkerThread* thr = new CWorkerThread();
		thr->addListener(this);
		thr->start();
		appendToIdleList(thr);
//		Mapbar_lockMutex(m_varMutex);
//		m_AvailNum++;
//		Mapbar_unlockMutex(m_varMutex);
	}
}

void CThreadPool::deleteIdleThread(int num)
{
	CQ_LOG_INFO("[ThreadPool] CThreadPool::deleteIdleThread() DeleteNum = %d \n", num);
	CQ_LOG_INFO("[ThreadPool] CThreadPool::deleteIdleThread() TotalList size = %d \n. ", (m_busyList.size() + m_IdleList.size()));
	for (int i = 0; i<num; i++){
		CWorkerThread* thr = NULL;
		if (m_IdleList.size() > 0){
			thr = (CWorkerThread*)m_IdleList.front();
		}
		if (thr == NULL)
		{
			return;
		}

		std::vector<CWorkerThread*>::iterator pos;
		for (pos = m_IdleList.begin(); pos != m_IdleList.end();)
		{
			if (*pos == thr)
			{
				pos = m_IdleList.erase(pos);    //删除元素，返回值指向已删除元素的下一个位置
				thr->destroy();

				delete thr;
				break;
			}
			else
			{
				++pos;
			}
		}
//		m_AvailNum--;
	}
	CQ_LOG_INFO("[ThreadPool] CThreadPool::DeleteIdleThread() TotalList size = %d \n. ", (m_busyList.size() + m_IdleList.size()));
}

void CThreadPool::setJob(CJob* job)
{
	Mapbar_lockMutex(m_jobMutex);
	m_JobList.push_back(job);
	Mapbar_unlockMutex(m_jobMutex);
}

CJob* CThreadPool::getJob()
{
	Mapbar_lockMutex(m_jobMutex);
	if (0 == m_JobList.size())
	{
		Mapbar_unlockMutex(m_jobMutex);
		return NULL;
	}
	CJob* job = m_JobList.front();
	m_JobList.pop_front();
	Mapbar_unlockMutex(m_jobMutex);

	return job;
}

size_t CThreadPool::getJobSize()
{
	Mapbar_lockMutex(m_jobMutex);
	size_t size = m_JobList.size();
	Mapbar_unlockMutex(m_jobMutex);

	return size;
}

void CThreadPool::run(CJob* job)
{
	if (getAllNum() >= m_maxNum)
	{
		setJob(job);
		return;
	}
	//if the busy thread num adds to m_maxNum,so we should wait 
	//(getBusyNum() == m_maxNum)
	//		Mapbar_waitEvent(m_maxNumCond);
	//if (m_IdleList.size()<m_availLow)
	//{
	//	if (getAllNum() + m_threadNum - m_IdleList.size() < m_maxNum)
	//		createIdleThread(m_threadNum - m_IdleList.size());
	//	else
	//		createIdleThread(m_maxNum - getAllNum());
	//}
	//printf("[run] this = %p, all = %d, busySize = %d, m_IdleList = %d \n", this, getAllNum(), m_busyList.size(), m_IdleList.size());
	if (m_IdleList.size() < m_createNum && getAllNum() < m_maxNum)
	{
		createIdleThread(m_createNum);
	}

	Mapbar_lockMutex(m_idleMutex);
	if (m_IdleList.size() > 0)
	{
		//Mapbar_unlockMutex(m_idleMutex);
		//CJob * job = getJob();

		//job返回空时，造成线程一直处于waitevent状态，并且无法解除。
		if (NULL == job)
		{
			Mapbar_unlockMutex(m_idleMutex);
			return;
		}

		CWorkerThread*  idlethr = getIdleThread();
		if (idlethr != NULL)
		{
			moveToBusyList(idlethr);
			Mapbar_unlockMutex(m_idleMutex);
			idlethr->execute(job);
		}
	}
	else
	{
		Mapbar_unlockMutex(m_idleMutex);
	}
}

IThreadPoolListener* CThreadPool::getListener()
{
	return m_listener;
}

void CThreadPool::addListener(IThreadPoolListener* listener)
{
	m_listener = listener;
}

void CThreadPool::removeListener()
{
	m_listener = NULL;
}

void CThreadPool::setMaxNum(int maxnum)
{ 
	if (maxnum >= getThreadNum())
	{
		m_maxNum = maxnum;
		return;
	}
	return;
}

void CThreadPool::setThreadNum(int initnum)
{
	if (initnum <= m_maxNum)
	{
		m_threadNum = initnum;
		return;
	}
	return;
}