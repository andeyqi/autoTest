#ifndef __SERVICE_MANAGER_COMMUNICATION_RECEIVER_H__
#define __SERVICE_MANAGER_COMMUNICATION_RECEIVER_H__

#include "serviceManagerCommonPublic.h"
#include "serviceManagerCommunicationIpcWrapper.h"

typedef struct STU_NAVI_SYS_CALLBACK
{
	ReceiverCallback callback;							// CallBack Function Pointer
	bool isRealTime;									// Ture£ºRealTime false£ºdefault 
}STU_NAVI_SYS_CALLBACK;

//=======================================================================
// macro define
#define 	LOW_PRIO       		1                   	// priority low
#define 	NORMAL_PRIO     	10                 	 	// priority normal
#define 	HIGH_PRIO       	31  					// priority high

// return value
#define 	D_NAVISYS_ERR		-1						// Error
#define 	D_NAVISYS_OK		0						// OK

//=======================================================================


class NaviSysReceiver : public NaviSysIPCWrapperListener
{
public:
	//get object
	static NaviSysReceiver* getInstance();

	// clase free
	//delete by shenghao for exit timeout
	//void releaseInstance();

	void setCallback(int funcId, ReceiverCallback pfun);

	void setRealTimeCallback(int funcId, ReceiverCallback pfun);

	void setNaviStartStatus(bool status);

	virtual void messageListener(void *data);
	void callbackProc(void *pdata);

	void doAsynCallback(ReceiveData *data);

	bool naviServiceHasReceive();

	ReceiveData* naviServiceGetReceive();

	friend int receiverMessageProcess(void* para);

	//get IpcWrapper
	NaviSysIPCWrapper* getIpcWrapper();

private:
	NaviSysReceiver();

	virtual ~NaviSysReceiver();

	bool naviServiceClearReceiveList();

	void startReceiverMessageProcess();

	std::list<ReceiveData*> m_receiveList;
	// brief Send msg to msgQuene
	// 1.Adata: parameter
	// 2.Apriority:LOW_PRIO/NORMAL_PRIO/HIGH_PRIO
	bool sendMsgQuene(void * data, int priority);

	//class object
	static NaviSysReceiver* mpInstance;

	// Callback array
	STU_NAVI_SYS_CALLBACK m_CallBackArray[FuncID_Receive_Max];
	// ipc Wrapper instance
	NaviSysIPCWrapper* m_ipcWrapper;
};

#endif /* __SERVICE_MANAGER_COMMUNICATION_RECEIVER_H__ */
