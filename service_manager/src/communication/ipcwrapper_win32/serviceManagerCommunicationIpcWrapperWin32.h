#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_WIN32_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_WIN32_H__


#include "stdafx_serviceManager.h" 
#include "serviceManagerCommonPublic.h" 
#include "serviceManagerCommunicationIpcWrapper.h"
#include <list>
class CThreadPool;

//=======================================================================

class NaviSysIPCWrapperWin32 : public NaviSysIPCWrapper
{
public:

	NaviSysIPCWrapperWin32();

	virtual ~NaviSysIPCWrapperWin32();
	// brief Send msg to Service Manager IPCWrapper
	// 1.Adata: parameter
	// 2.Apriority:LOW_PRIO/NORMAL_PRIO/HIGH_PRIO
	// return:true/false
	// clase free
	void releaseInstance();
	bool sendMsg(void *data, int priority);
	void startIpcWrappereProcess();
	SendData* getRequreList();
	bool ipcWrapperHasRequest();
	void ipcWrapperProcess(SendData * param);

	virtual void addListener(NaviSysIPCWrapperListener *listerner);
	virtual void removeListener();
	//virtual void terminateAll();

	friend class IPCWrapperJob;

	static NaviSysIPCWrapperWin32* getInstance();

	

	void callbackProc(void *pdata);

	void getVolume();

	void setVolume(void* pdata);

	void updataVolume(int volume);

	void getBacklightMode();
	void updataBacklightMode(
		BackLightMode mode,
		BackLightType type);

	// ??¡§¡§?¡§o?¨¤????¡§o? true?¨ºo24-hour clock?¨º?false?¨ºo12-hour clock.
	void getTimeFormat();
	void updateTimeFormat(bool svpBool);

	void getLanguageID();
	void updateLanguageID(uint32 languageId);

	void isGPSUpdateTimeEnabled();
	void responseGPSUpdateTimeEnabled(bool svpBool);

	void backToHost();
	void showHostMenu();
	void requestAudioGuidancePlay();
	void responseAudioGuidancePlay(bool svpBool);
	void releaseAudioGuidancePlay();
	void OnStopAudioGuidancePlayRequested(bool bPause);

	void isCarPlayNaviActive();
	void responseisCarPlayNaviActive(bool svpBool);
	void OnCarPlayNaviStatusChanged(bool bStart);
	void setRouteStatus(void* data);

	void OnMapCardChanged(bool uiAttached);

	void reqBeep();
	void repAccOffSucess();
	void setVersion(void* data);

	friend int getVolumeProcess(void* pama);
	friend int reqBeepProcess(void* pama);

public:	
	uint32* m_Volume;
	std::list<SendData*> m_SendList;
	Handle m_mutex;
	BOOL m_shouldExit;
	Handle m_threadHandle;
	
private:
	static NaviSysIPCWrapperWin32* m_IpcWrapperInstance;
	NaviSysIPCWrapperListener *m_listerner;
	CThreadPool*    m_Pool;
};

#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_WIN32_H__ */
