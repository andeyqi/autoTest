#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_H__

#include "serviceManagerCommonPublic.h" 

//=======================================================================

class NaviSysIPCWrapperListener
{
public:
	virtual void messageListener(void *data) = 0;
};

class NaviSysIPCWrapper
{
public:
	NaviSysIPCWrapper(){};

	virtual ~NaviSysIPCWrapper(){};

	// brief Send msg to Service Manager IPCWrapper
	// 1.Adata: parameter
	// 2.Apriority:LOW_PRIO/NORMAL_PRIO/HIGH_PRIO
	// return:true/false
	virtual bool sendMsg(void * data, int priority)=0;

	virtual void addListener(NaviSysIPCWrapperListener *listerner)=0;
	virtual void removeListener()=0;
	//virtual void terminateAll() = 0;
};
#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_H__ */
