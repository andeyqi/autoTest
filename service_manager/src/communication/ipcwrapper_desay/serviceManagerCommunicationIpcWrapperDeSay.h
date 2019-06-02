#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_DESAI_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_DESAI_H__

#include "stdafx_serviceManager.h" 
#include "serviceManagerCommonPublic.h" 
#include "serviceManagerCommunicationIpcWrapper.h"
#include <list>
#include "ExtNavi.h"
#include "NotifyID.h"

//=======================================================================

class NaviSysIPCWrapperDeSay : public NaviSysIPCWrapper
{
public:
	NaviSysIPCWrapperDeSay();
	virtual ~NaviSysIPCWrapperDeSay();

	// brief Send msg to Service Manager IPCWrapper
	// 1.Adata: parameter
	// 2.Apriority:LOW_PRIO/NORMAL_PRIO/HIGH_PRIO
	// return:true/false
	bool sendMsg(void * data, int priority);
	void startIpcWrappereProcess();
	bool ipcWrapperHasRequest();
	SendData* naviServiceGetSend();
	void ipcWrapperProcess(SendData* param);

	static NaviSysIPCWrapperDeSay* getInstance();
	// clase free
	void releaseInstance();

	void setCallback(void *pfun);

	virtual void addListener(NaviSysIPCWrapperListener *listerner);
	virtual void removeListener();

	void callbackProc(void *pdata);
	// Machine ID
	void getMachineID();
	void updataMachineID(string machineID);

	// Volume
	void getVolume();
	void setVolume(void* pdata);
	void updataVolume(int volume);

	// Backlight
	void getBacklightMode();
	void updataBacklightMode(
		uint8 mode,
		uint8 type,
		uint8 level);
	
	// 获取时间格式 true：24-hour clock，false：12-hour clock.
	void getTimeFormat();
	void updateTimeFormat(bool svpBool);

	//void backToHost();

	//void showHostMenu();
	
	// 请求Guidance声音是否可以播放
	//void requestAudioGuidancePlay();
	//void responseAudioGuidancePlay(SVPBool sBool);

	// 释放Guidance声音播放权限
	//void releaseAudioGuidancePlay();
	// 获取当前语言ID
	void getLanguageID();
	void updateLanguageID(uint8 languageId);
	
	//void isGPSUpdateTimeEnabled();
	//void responseGPSUpdateTimeEnabled(SVPBool sBool);
	
	void setRouteStatus(bool bStart);

	//void isCarPlayNaviActive();
	//void responseisCarPlayNaviActive(SVPBool sBool);

	void reqBeep(BEEP_TYPE beepType);
	void reqBeep(const std::string& player_id);

	// ISVPExtNaviListener
	//virtual SVPVoid OnVolumeChanged(SVPUint8 uiVolume);
	virtual void OnBacklightChanged(
		uint8 mode,
		uint8 type,
		uint8 level);
	//virtual SVPVoid OnStopAudioGuidancePlayRequested(SVPBool bPause);
	//virtual SVPVoid OnMapCardChanged(SVPBool uiAttached);
	//virtual SVPVoid OnCarPlayNaviStatusChanged(SVPBool bStart);

	// NaviGuidance
	//void setGuidance(SVPUint32 uiID, SVPUint32 uiDistance);
	//void nextRoadName(char* pUTF8RoadName, SVPUint32 uiLen);
	//void startGuidance();
	//void stopGuidance();

	//设置导航软件版本号及地图版本号 (版本号字符长度<30字节)
	void setVersion(void* pdata);

	// 获取Wifi信号
	void getWifiStatus();
	void updateWifiStatus(int32 wifistatus);

	friend int IpcWrapperMessageProcess(void * pdata);
	
private:
	friend void DataReceiveCallBack(uint32 evt_id, int argc, void *argv[], int *args, int fromID);
private:
	//class object
	static NaviSysIPCWrapperDeSay* mpInstance;
	CExtNavi* m_pExtNavi;
	NaviSysIPCWrapperListener *m_listerner;
	//ISVPExtNaviGuidance* m_naviGuidance;
	std::list<SendData*> m_SendList;
	Handle m_mutex;
	BOOL m_shouldExit;
	Handle m_threadHandle;
};
#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_DESAI_H__ */
