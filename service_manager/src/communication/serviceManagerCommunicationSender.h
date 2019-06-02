#ifndef __SERVICE_MANAGER_COMMUNICATION_SENDER_H__
#define __SERVICE_MANAGER_COMMUNICATION_SENDER_H__

#include "serviceManagerCommonPublic.h" 
#include "serviceManagerCommunicationIpcWrapper.h"
#include <list>

//=======================================================================
// macro define
#define 	LOW_PRIO       		1                   	// priority low
#define 	NORMAL_PRIO     	10                 	 	// priority normal
#define 	HIGH_PRIO       	31  					// priority high

// return value
#define 	D_NAVISYS_ERR		-1						// Error
#define 	D_NAVISYS_OK		0						// OK
//=======================================================================
using namespace ServiceManager;

class NaviSysSender 
{
public:	
	// brief Send msg to System
	// 1.Adata: parameter
	// 2.Apriority:LOW_PRIO/NORMAL_PRIO/HIGH_PRIO
	// return:true/false
	bool sendMsg(void * data, int priority);

	//get object
	static NaviSysSender* getInstance();

	//delete by shenghao for exit timeout
	// clase free
	//void releaseInstance();

	void setNaviStartStatus(bool status);

	SendData* naviServiceGetRequest();

	bool naviServiceHasRequest();

	friend int senderMessageProcess(void* para);

private:
	NaviSysSender();

	virtual ~NaviSysSender();
	//Message Process
	bool naviServiceClearRequestList();

	void startSenderMessageProcess();

	std::list<SendData*> m_requestList;

	static NaviSysSender* mpInstance;

	NaviSysIPCWrapper* m_ipcWrapper;
};

#endif /* __SERVICE_MANAGER_COMMUNICATION_SENDER_H__ */
