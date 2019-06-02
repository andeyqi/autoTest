#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_H__
#include "stdafx_serviceManager.h"
#include "serviceManagerCommunicationIpcWrapper.h"
#include "serviceManagerCommunicationIpcWrapperToyotaDbusServer.h"
#include "c_hashmap.h"
#define MSG_MAX_SIZE 256
#define MSGID_MAX_SIZE 9
class CThreadPool;

class NaviSysIPCWrapperToyota : public NaviSysIPCWrapper, public ToyotaDbusServiceListener
{
public:
	NaviSysIPCWrapperToyota();
	virtual ~NaviSysIPCWrapperToyota();

	void initHashMap();
	// �ṩ��Senderʹ�ã����ڹ�����Ϣ������signal
	virtual bool sendMsg(void * data, int priority);
	// �ṩ��Receiverʹ��
	virtual void addListener(NaviSysIPCWrapperListener *listerner);
	virtual void removeListener();
	// �ṩ��dbus serviceʹ�ã�������Ϣ�򵼺�֪ͨ
	virtual void notifyMsg(void* data);
	//delete by shenghao for exit timeout
	//virtual void terminateAll();

	static NaviSysIPCWrapperToyota* getInstance();

	friend class IPCWrapperJob;

	CThreadPool* getThreadPool();
	NaviSysIPCWrapperListener* getListener();
	BOOL getNaviAccStatus();
	void setNaviAccStatus(BOOL isAccOff);
	BOOL getNaviIsReady();
	void setNaviIsReady(BOOL isNaviReady);

private:
	NaviSysIPCWrapperListener* mpListener;
	CThreadPool*    m_Pool;
	static NaviSysIPCWrapperToyota* m_IpcWrapperInstance;
	map_t m_RevHashMap;
	map_t m_SendHashMap;
	BOOL m_isAccOff;
	BOOL m_isNaviReady;
};

#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_H__ */




