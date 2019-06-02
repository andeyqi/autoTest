#ifndef __SERVICE_MANAGER_EXTERNAL_CLIENT_DISPLAY_H__
#define __SERVICE_MANAGER_EXTERNAL_CLIENT_DISPLAY_H__

namespace ServiceManager
{
	enum SLIENT_MODE
	{
		SLIENT_MODE_OFF = 0,	// 非静默模式
		SLIENT_MODE_ON = 1,		// 静默模式
	};
	void DisplaySvcClient_Initialize();
	//delete by shenghao for exit timeout
	//void DisplaySvcClient_CleanUp();
	void DisplaySvcClient_ReqShowNavi(void* data);
	void DisplaySvcClient_ReqFormShow(void* data);
	void DisplaySvcClient_ReqHideNavi(void* data);
	void DisplaySvcClient_ReqFormHide(void* data);
	void DisplaySvcClient_ReqRefreshNavi(void* data);
	void DisplaySvcClient_RefreshNavi(void* data);
	//void DisplaySvcClient_LanguageSwitchChanged(void* data);
	void LanguageSwitchNotify(void* data);
	void DisplaySvcClient_SilentModeChanged(void* data);
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_DISPLAY_H__ */