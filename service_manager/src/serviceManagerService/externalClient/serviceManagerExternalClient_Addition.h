#ifndef __SERVICE_MANAGER_EXTERNAL_CLIENT_ADDITION_H__
#define __SERVICE_MANAGER_EXTERNAL_CLIENT_ADDITION_H__

namespace ServiceManager
{
#define HISTORYCLEARSUCCESS 0
#define HISTORYCLEARFAILED  -1

	// initialize
	void AdditionSvcClient_Initialize(void);

	//delete by shenghao for exit timeout
	// cleanup
	//void AdditionSvcClient_CleanUp(void);

	// 不需要头文件中声明
	//int AdditionSvcClient_Gps_getInfo(void* params);
	//void AdditionSvcClient_ReqGetGPSTime(void *data);

	enum EN_SETTINGSVCCLIENT_FUNCTIONINMAINTHREAD
	{
		EN_SETTINGSVCCLIENT_FUNCTIONINMAINTHREAD_OK,
		EN_SETTINGSVCCLIENT_FUNCTIONINMAINTHREAD_NG,
	};
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_ADDITION_H__ */
