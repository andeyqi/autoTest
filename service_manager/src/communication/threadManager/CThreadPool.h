#ifndef __CTHREAD_POOL_H  
#define __CTHREAD_POOL_H  

#include <vector>
#include <list>

#include "cq_stdlib.h"
#include "mapdal/mapbar_dal_pub.h"

#include "CJob.h"
#include "CWorkerThread.h"

using namespace std;

class IThreadPoolListener
{
public:
	virtual void onJobFinished(CJob* job)
	{
		delete(job);
	}
};

class CThreadPool : public IWorkerThreadListener
{
public:
	//CThreadPool(int threadNum = 5, int maxNum = 50, int availIdleLow = 5, int availIdleHigh = 10);
	CThreadPool(int threadNum = 5, int maxNum = 50);
	~CThreadPool();
	// 
	void    setMaxNum(int maxnum);
	int     getMaxNum(void){ return m_maxNum; }

	//void    setAvailLowNum(int minnum){ m_availLow = minnum; }
	//int     getAvailLowNum(void){ return m_availLow; }

	//void    setAvailHighNum(int highnum){ m_availHigh = highnum; }
	//int     getAvailHighNum(void){ return m_availHigh; }

	int     getAllNum(void){ return m_busyList.size() + m_IdleList.size(); }
	int     getBusyNum(void){ return m_busyList.size(); }

	void    setThreadNum(int initnum);
	int     getThreadNum(void){ return m_threadNum; }

	void    terminateAll(void);
	void    run(CJob* job);

	IThreadPoolListener* getListener();
	void addListener(IThreadPoolListener* listener);
	void removeListener();

public:
	void onJobFinished(CWorkerThread* busythread, CJob* job);
	friend int runProcess(void* para);

private:
	void    createIdleThread(int num);
	void    deleteIdleThread(int num);
	void	initCommon(void);

	CWorkerThread* getIdleThread(void);
	void    appendToIdleList(CWorkerThread* jobthread);
	void    moveToBusyList(CWorkerThread* idlethread);

	void setJob(CJob* job);
	CJob* getJob();
	size_t getJobSize();

	Handle  m_busyCond; //m_busyCond is used to sync busy thread list 
	Handle  m_idleCond; //m_idleCond is used to sync idle thread list 
	Handle  m_idleJobCond; 
	Handle  m_maxNumCond;

	Handle m_busyMutex;
	Handle m_idleMutex;
	Handle m_jobMutex;

	std::list<CJob* >	m_JobList;      //Job List
	std::vector<CWorkerThread*>   m_busyList;     //Thread List 
	std::vector<CWorkerThread*>   m_IdleList; //Idle List 

	unsigned int m_maxNum;   //the max thread num that can create at the same time 
	//unsigned int m_availLow; //The min num of idle thread that shoule kept 
	//int m_availHigh;    //The max num of idle thread that kept at the same time 
	//	unsigned int m_AvailNum; //the normal thread num of idle num; 
	unsigned int m_threadNum;  //Normal thread num;
	unsigned int m_createNum; //create thread num
	IThreadPoolListener * m_listener;
};
#endif