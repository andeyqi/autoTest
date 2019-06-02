#include "stdafx_serviceManager.h"
//#include "cq_types_basic.h"
#include "serviceManagerCommunicationIpcWrapper.h"
//#include "serviceManagerCommonPublic.h"
//#include "serviceManagerCommunicationIpcWrapperWin32.h"
//#include "serviceManagerOutput.h"
//=====================================================//
NaviSysIPCWrapperWin32* NaviSysIPCWrapperWin32::m_IpcWrapperInstance = NULL;
//NaviSysIPCWrapperWin32* NaviSysIPCWrapperWin32::mpInstance = NULL;
using namespace ServiceManager;

class IPCWrapperJob : public CJob
{
public:
	IPCWrapperJob(void *data) : CJob(data)
	{

	}

	virtual void run()
	{
		ReceiveData* data = (ReceiveData*)getJobdata();
		if (NULL != NaviSysIPCWrapperWin32::getInstance())
		{
			if (NaviSysIPCWrapperWin32::getInstance()->m_shouldExit)
			{
				return;
			}
			//printf("m_job->run FUNID = %d \n", paramData->funcId);
			NaviSysIPCWrapperWin32::getInstance()->m_listerner->messageListener(data);
		}
	}
};



/**
*	@brief
*		NaviSysIPCWrapper构造
*/
NaviSysIPCWrapperWin32::NaviSysIPCWrapperWin32()
{
	m_mutex = Mapbar_createMutex();
	m_shouldExit = FALSE;
	m_threadHandle = invalidHandle;
	startIpcWrappereProcess();
	uint32 vol = 0;
	m_Volume = (uint32 *)malloc(sizeof(uint32));
	if (m_Volume == NULL)
	{
		return;
	}
	*m_Volume = vol;
	// FIXME
	m_IpcWrapperInstance = this;

	m_Pool = new CThreadPool(5);
	m_Pool->addListener(new IThreadPoolListener());
}

/**
*	@brief
*		NaviSysIPCWrapper析构
*/
NaviSysIPCWrapperWin32::~NaviSysIPCWrapperWin32()
{
	Mapbar_destroyMutex(m_mutex);

	// add by liubo
	if (NULL != m_Volume)
	{
		free(m_Volume);
		m_Volume = NULL;
	}

	if (NULL != m_listerner)
	{
		free(m_listerner);
		m_listerner = NULL;
	}
	// 析构线程池 add by renxs
	

	if (m_threadHandle != invalidHandle)
	{
		m_shouldExit = TRUE;
		Mapbar_waitThread(m_threadHandle);
		Mapbar_closeThread(m_threadHandle);
		m_threadHandle = invalidHandle;
	}
	delete m_Pool;
	m_Pool = NULL;
}

NaviSysIPCWrapperWin32* NaviSysIPCWrapperWin32::getInstance()
{
	if (NULL != m_IpcWrapperInstance)
		return m_IpcWrapperInstance;
	return NULL;
}

//delete by shenghao for exit timeout
//void NaviSysIPCWrapperWin32::terminateAll()
//{
//	m_Pool->terminateAll();
//}
/**
*	@brief
*		提供给Receiver发送消息接口
*	@return
*		bool true：成功 false：失败
*	@param pdata
*		data：发送的msg priority：优先级
*/

bool NaviSysIPCWrapperWin32::sendMsg(void *data, int priority)
{
	UNUSED_VAR(priority);

	Mapbar_lockMutex(m_mutex);
	
	m_SendList.push_back((SendData*)data);

	Mapbar_unlockMutex(m_mutex);

	return true;
}


SendData* NaviSysIPCWrapperWin32::getRequreList()
{
	Mapbar_lockMutex(m_mutex);

	if (m_SendList.size() <= 0)
	{
		Mapbar_unlockMutex(m_mutex);
		return NULL;
	}

	SendData* msg = m_SendList.front();
	m_SendList.pop_front();

	Mapbar_unlockMutex(m_mutex);
	return msg;
}
/**
*	@brief
*		获取消息队列msg

SendData* NaviSysIPCWrapperWin32::naviServiceGetSend()
{
	Mapbar_lockMutex(m_mutex);

	SendData* msg = m_SendList.front();
	m_SendList.pop_front();

	Mapbar_unlockMutex(m_mutex);

	return msg;
}
*/

/**
*	@brief
*		向车机系统请求线程
*/
int IpcWrapperMessageProcess(void* para)
{
	NaviSysIPCWrapperWin32* ipcWrapper = (NaviSysIPCWrapperWin32*)para;
	if (ipcWrapper->m_shouldExit)
	{
		ipcWrapper->m_shouldExit = true;
	}
	while (!ipcWrapper->m_shouldExit)
	{
		if (!ipcWrapper->ipcWrapperHasRequest())
		{
			Mapbar_sleep(100);
			continue;
		}

		SendData* request = ipcWrapper->getRequreList();

		//call IPC Function
		if (request != NULL)
			ipcWrapper->ipcWrapperProcess(request);
	};
	return D_NAVISYS_OK;
}

/**
*	@brief
*		判断消息队列是否有msg
*	@return
*		bool true：有 false：无
*/
bool NaviSysIPCWrapperWin32::ipcWrapperHasRequest()
{
	Mapbar_lockMutex(m_mutex);

	int size = m_SendList.size();

	Mapbar_unlockMutex(m_mutex);

	return (size != 0);
}

/**
*	@brief
*		启动IpcWrapper消息处理线程
*	@return
*		void
*/
void NaviSysIPCWrapperWin32::startIpcWrappereProcess()
{
	m_threadHandle = Mapbar_createThread(IpcWrapperMessageProcess, this);
	return;
}

/**
*	@brief
*		IpcWrapper消息处理函数
*	@return
*		void
*/

void NaviSysIPCWrapperWin32::ipcWrapperProcess(SendData * param)
{
	if (param == NULL)
	{
		return;
	}

	// 实际处理位置
	SendData* paramData = (SendData *)param;
	switch (paramData->funcId)
	{
	case FuncID_Send_GetVolume:
	{
		getVolume();
	}
		break;

	case FuncID_Send_SetVolume:
	{
		setVolume(paramData->data);
	}
		break;
	case FuncID_Send_GetBackLight:
	{
		getBacklightMode();
	}
		break;
	case FuncID_Send_GetLanguageID:
	{
		getLanguageID();
	}
		break;
		//isGPSUpdateTimeEnabled
	case FuncID_Send_IsGPSUpdateTimeEnabled:
	{
		isGPSUpdateTimeEnabled();
	}
		break;
	case FuncID_Send_BackToHost:
	{
		backToHost();
	}
		break;
	case FuncID_Send_ShowHostMenu:
	{
		showHostMenu();
	}
		break;
	case FuncID_Send_RequestAudioGuidancePlay:
	{
		requestAudioGuidancePlay();
	}
		break;
	case FuncID_Send_ReleaseAudioGuidancePlay:
	{
		releaseAudioGuidancePlay();
	}
		break;
	case FuncID_Send_SetRouteStatus:
	{
		if (NULL != paramData->data)
		setRouteStatus(paramData->data);
	}
		break;
	case FuncID_Send_IsCarPlayNaviActive:
	{
		isCarPlayNaviActive();
	}
		break;
	case FuncID_Send_ReqPlayBeepSound:
	{
		reqBeep();
	}
		break;
	case FuncID_Send_NaviACCOFFSuc:
	{
		repAccOffSucess();
	}
		break;
	case FuncID_Send_MeterDirectionInfo:
	{
	}
		break;
	case FuncID_Send_MeterNextTBTInfo:
	{
	}
		break;
	case FuncID_Send_RunStatusRequest:
	{
		// 执行回调
		if (NULL != m_listerner)
		{   
			//delete by shenghao   threadpool test code
			//for (size_t i = 0; i < 10; i++)
			//{
			//	ServiceManager::ReceiveData* paramData_t = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
			//	paramData_t->funcId = ServiceManager::FuncID_Receive_RunStatusChanged;
			//	int runstatus = 0;
			//	paramData_t->data = (int*)malloc(sizeof(int));
			//	memcpy(paramData_t->data, &runstatus, sizeof(int));

			//	
			//	CJob* job = new CJob();
			//	m_Pool->run(job, paramData_t);
			//}
			ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
			if (paramData == NULL)
			{
				return;
			}
			paramData->funcId = ServiceManager::FuncID_Receive_RunStatusChanged;
			int runstatus = 0;
			paramData->data = (int*)malloc(sizeof(int));
			if (paramData->data == NULL)
			{
				free(paramData);
				paramData = NULL;
				return;
			}
			memcpy(paramData->data, &runstatus, sizeof(int));


			// 执行回调
			if (NULL != m_listerner)
			{
				IPCWrapperJob* job = new IPCWrapperJob(paramData);
				m_Pool->run(job);
				//m_listerner->messageListener(paramData);
			}
		}
	}
		break;
	case FuncID_Send_getUUID:
	{	
        //delete by shenghao   threadpool test code
	    // 先测试回调wifi状态 test
		// 执行回调
		//if (NULL != m_listerner)
		//{
		//	ServiceManager::ReceiveData* paramData1 = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
		//	paramData1->funcId = ServiceManager::FuncID_Receive_WifiStatusChanged;
		//	paramData1->data = (uint32*)malloc(sizeof(uint32));
		//	((uint32*)paramData1->data)[0] = (uint32)EN_ADDITIONSVCENGINE_WIFI_STATE_WIFICONNECTED;
		//	printf("return wifi FuncID_Receive_WifiStatusChanged \n");
		//	CJob* job = new CJob();
		//	m_Pool->run(job, paramData1);
		//	//m_listerner->messageListener(paramData1);
		//}

		ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Receive_getUUID;
		paramData->data = (char*)malloc(sizeof(char)* 128);
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		strcpy((char*)paramData->data, "12024OEM0D0001D26118");

		//ServiceManager::NaviData naviData;
		//cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, (cqCHAR*)paramData->data);
		//ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);

		// 执行回调
		if (NULL != m_listerner)
		{
			IPCWrapperJob* job = new IPCWrapperJob(paramData);
			m_Pool->run(job);
			//m_listerner->messageListener(paramData);
		}
	}
		break;
	case FuncID_Send_ReqCheckILLState:
	{
		ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Receive_sgnILLStateChanged;
		paramData->data = (BOOL*)malloc(sizeof(BOOL));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		*((BOOL*)paramData->data) = FALSE;

		// 执行回调
		if (NULL != m_listerner)
		{
			printf("return ILL FuncID_Receive_sgnILLStateChanged \n");
			IPCWrapperJob* job = new IPCWrapperJob(paramData);
			m_Pool->run(job);
			//m_listerner->messageListener(paramData);
		}
	}
		break;
	//case FuncID_Send_NaviStopSuc:
	//{
	//	ReceiveData* paramData = NULL;
	//	paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	//	memset(paramData, 0, sizeof(ReceiveData));
	//	paramData->funcId = FuncID_Receive_AnswerSendStopSucess;

	//	printf("return NaviStop FuncID_Receive_AnswerSendStopSucess \n");
	//	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	//	m_Pool->run(job);
	//	//m_listerner->messageListener(paramData);
	//}
	//	break;

	case FuncID_Send_ReqUsbStatus:
	{
		 ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		 if (paramData == NULL)
		 {
			 return;
		 }
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_GetUsbStatus;
		paramData->data = (BOOL*)malloc(sizeof(BOOL));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		*((BOOL*)paramData->data) = TRUE;
		//paramData->data = NULL;
		//mpListener->messageListener(paramData);
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
	}
		break;
	//	case FuncID_Send_getRevStatus:
	//{
	//	uint32 revStatus = 1;
	//	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_MUTE index is %d.\n", __FUNCTION__, __LINE__, revStatus);
	//	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	//	memset(paramData, 0, sizeof(ReceiveData));
	//	paramData->funcId = FuncID_Receive_RevStatusChanged;
	//	paramData->data = (uint32*)malloc(sizeof(uint32));
	//	memcpy(paramData->data, &revStatus, sizeof(uint32));
	//	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	//	m_Pool->run(job);
	//}
	//	break;
	case FuncID_Send_LanguageSwitchFailure:
	{
		uint32 languageStatus = 0;
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_LanguageSwitchChanged;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &languageStatus, sizeof(uint32));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//printf("return FuncID_Receive_LanguageSwitchChanged \n");
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_LanguageSwitchChanged;
		//paramData->data = 0;
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//m_Pool->run(job);
	}
		break;
	case	FuncID_Send_LanguageSwitchSuccess:
	{
		uint32 languageStatus = 0;
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_LanguageSwitchChanged;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &languageStatus, sizeof(uint32));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
	}
		break;
	
	case FuncID_Send_ReqHideMusicWidget:
	{
		ReceiveData* paramData = NULL;
		paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_MusicWidgeHideChanged;
		paramData->data = (BOOL*)malloc(sizeof(BOOL));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		*((BOOL*)paramData->data) = TRUE;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//cqCHAR message[] = "20000022:1,11642434,3995241,东航酒店";
		//VRPassPointInfo* poiInfo = NULL;
		//if (':' == message[8] && '\0' != message[9])
		//{
		//	poiInfo = (VRPassPointInfo*)malloc(sizeof(VRPassPointInfo));
		//	cqCHAR cqtype = message[9];
		//	poiInfo->type = cq_atoi(&cqtype);
		//	if (poiInfo->type == 0 && message[10] == ',')
		//	{
		//		int indexLon = 0;
		//		int indexLat = 0;
		//		int indexName = 0;
		//		cqCHAR lon[9] = { 0 };
		//		cqCHAR lat[8] = { 0 };
		//		cqCHAR name[64] = { 0 };

		//		// 经度
		//		for (; message[indexLon + 11] != '\0' && indexLon < 8; indexLon++)
		//			lon[indexLon] = message[indexLon + 11];
		//		lon[indexLon] = '\0';
		//		poiInfo->lon = cq_atoi(lon);
		//		// 纬度
		//		for (; message[indexLat + 20] != '\0' && indexLat < 7; indexLat++)
		//			lat[indexLat] = message[indexLat + 20];
		//		lat[indexLat] = '\0';
		//		poiInfo->lat = cq_atoi(lat);
		//		// 名称
		//		for (; message[indexName + 28] != '\0' && indexName < 64; indexName++)
		//			name[indexName] = message[indexName + 28];
		//		name[indexName] = '\0';
		//		poiInfo->name = name;
		//	}

		//}
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_VRPassingPoint;
		//paramData->data = poiInfo;
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//m_Pool->run(job);
	}
		break;

	case FuncID_Send_ReqShowMusicWidget:
	{
		ReceiveData* paramData = NULL;
		paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_MusicWidgeShowChanged;
		paramData->data = (BOOL*)malloc(sizeof(BOOL));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		*((BOOL*)paramData->data) = TRUE;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);

				//CQ_LOG_INFO("[ServiceManager] DBUS_WRAPPER_SIG_SILENTMODE_REQ message = %s \n", message);
		//cqCHAR message[] = "2000003D:1";
		//int32 silentMode = -1;

		//if (':' == message[8] && '\0' != message[9])
		//{
		//	silentMode = cq_atoi(&message[9]);
		//}
		////CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_SILENTMODE_REQ index is %d.\n", __FUNCTION__, __LINE__, silentMode);
		//ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_SilentModeChanged;
		//paramData->data = (uint32*)malloc(sizeof(uint32));
		//memcpy(paramData->data, &silentMode, sizeof(uint32));
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//m_Pool->run(job);

			//ReceiveData* paramData = NULL;
			//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
			//memset(paramData, 0, sizeof(ReceiveData));
			//paramData->funcId = FuncID_Receive_ReqHideNaviForm;
			////mpListener->messageListener(paramData);
			//IPCWrapperJob* job = new IPCWrapperJob(paramData);
			//m_Pool->run(job);

		//cqCHAR message[] = "20000024:1";
		//VR_COMMAND_ROUTE_PREFER routeprefer = VR_COMMAND_ROUTE_PREFER_INVALIDE;
		//if (':' == message[8] && '\0' != message[9])
		//{
		//	routeprefer = (VR_COMMAND_ROUTE_PREFER)cq_atoui_hex(&message[9]);
		//}
		//CQ_LOG_INFO("[ServiceManager]message[9] is %d routeprefer  is %d \n", message[9], routeprefer);
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_VRSwitchRoute;
		//paramData->data = (VR_COMMAND_ROUTE_PREFER*)malloc(sizeof(VR_COMMAND_ROUTE_PREFER));
		//memcpy(paramData->data, &routeprefer, sizeof(VR_COMMAND_ROUTE_PREFER));
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//CQ_LOG_INFO("[ServiceManager]function id [%x] data is [%d]\n", paramData->funcId, paramData->data);
		//m_Pool->run(job);
		//
		//// 等待3s
		//Mapbar_sleep(2000);
		//// 选单操作
		//cqCHAR message2[] = "20000028:1427895272,1435935683,东航酒店";
		//VRPoiInfo* poiInfo = NULL;
		//if (':' == message2[8] && '\0' != message2[9])
		//{
		//	poiInfo = (VRPoiInfo*)malloc(sizeof(VRPoiInfo));
		//	int indexLon = 0;
		//	int indexLat = 0;
		//	int indexName = 0;
		//	cqCHAR lon[12] = { 0 };
		//	cqCHAR lat[12] = { 0 };
		//	cqCHAR name[64] = { 0 };

		//	// 经度1
		//	int32 index = 9; //跳过':'
		//	for (; message2[index] != '\0' && message2[index] != ','; indexLon++, index++)
		//		lon[indexLon] = message2[index];
		//	lon[indexLon] = '\0';
		//	poiInfo->lon = cq_atoi(lon);

		//	// 纬度1
		//	index++; //跳过','
		//	for (; message2[index] != '\0' && message2[index] != ','; indexLat++, index++)
		//		lat[indexLat] = message2[index];
		//	lat[indexLat] = '\0';
		//	poiInfo->lat = cq_atoi(lat);

		//	// 名称1
		//	index++; //跳过','
		//	for (; message2[index] != '\0' && message2[index] != ','; indexName++, index++)
		//		name[indexName] = message2[index];
		//	name[indexName] = '\0';
		//	poiInfo->name = name;

		//}
		//ReceiveData* paramData2 = NULL;
		//paramData2 = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData2, 0, sizeof(ReceiveData));
		//paramData2->funcId = FuncID_Receive_VRItemSelect;
		//paramData2->data = poiInfo;
		//IPCWrapperJob* job2 = new IPCWrapperJob(paramData2);
		//m_Pool->run(job2);

		/*ReceiveData* paramData = NULL;
		paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_VRTrafficControl;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		*((uint32*)paramData->data) = 4;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);*/
		
		// 命令控制
		/*cqCHAR message[] = "20000028:4";
		CommandControlSig* commandStruct = NULL;
		if (':' == message[8] && '\0' != message[9])
		{
			commandStruct = (CommandControlSig*)malloc(sizeof(CommandControlSig));
			commandStruct->uiCommand = VR_COMMAND_INVALIDE;
			commandStruct->uiMapMode = VR_COMMAND_MAP_INVALIDE;

			cqCHAR comID = message[9];
			if (cq_atoi(&comID) < VR_COMMAND_INVALIDE)
				commandStruct->uiCommand = (VR_COMMAND_ID)cq_atoi(&comID);
			if (',' == message[10] && '\0' != message[11] && commandStruct->uiCommand == VR_COMMAND_CHANGE_MAP_TO)
			{
				cqCHAR modID = message[11];
				if (cq_atoi(&modID) < VR_COMMAND_MAP_INVALIDE)
					commandStruct->uiMapMode = (VR_COMMAND_MAP_MODE)cq_atoi(&modID);
			}
		}
		ReceiveData* paramData = NULL;
		paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_VRCommandControl;
		paramData->data = commandStruct;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);*/

		//cqCHAR message[] = "20000028:11642434,3995241,东航酒店,11641949,3994539,柏林寺";
		//VRViaAndDestPoiInfo* poiInfo = NULL;
		//if (':' == message[8] && '\0' != message[9])
		//{
		//	//poiInfo[0] = (VRPoiInfo*)malloc(sizeof(VRPoiInfo));
		//	poiInfo = (VRViaAndDestPoiInfo*)malloc(sizeof(VRViaAndDestPoiInfo));
		//	int index = 9;
		//	int indexLon = 0;
		//	int indexLat = 0;
		//	int indexName = 0;
		//	cqCHAR lon[9] = { 0 };
		//	cqCHAR lat[8] = { 0 };
		//	cqCHAR name[64] = { 0 };

		//	// 经度1
		//	for (; message[index] != '\0' && message[index] != ','; indexLon++, index++)
		//		lon[indexLon] = message[index];
		//	lon[indexLon] = '\0';
		//	poiInfo->viaLon = cq_atoi(lon);

		//	// 纬度1
		//	index++; //跳过':'
		//	for (; message[index] != '\0' && message[index] != ','; indexLat++, index++)
		//		lat[indexLat] = message[index];
		//	lat[indexLat] = '\0';
		//	poiInfo->viaLat = cq_atoi(lat);

		//	// 名称1
		//	index++; //跳过','
		//	for (; message[index] != '\0' && message[index] != ','; indexName++, index++)
		//		name[indexName] = message[index];
		//	name[indexName] = '\0';
		//	poiInfo->viaName = name;

		//	//poiInfo[1] = (VRPoiInfo*)malloc(sizeof(VRPoiInfo));
		//	int indexLon2 = 0;
		//	int indexLat2 = 0;
		//	int indexName2 = 0;
		//	cqCHAR lon2[9] = { 0 };
		//	cqCHAR lat2[8] = { 0 };
		//	cqCHAR name2[64] = { 0 };

		//	// 经度2
		//	index++; //跳过','
		//	for (; message[index] != '\0' && message[index] != ','; indexLon2++, index++)
		//		lon2[indexLon2] = message[index];
		//	lon2[indexLon2] = '\0';
		//	poiInfo->destLon = cq_atoi(lon2);

		//	// 纬度2
		//	index++; //跳过','
		//	for (; message[index] != '\0' && message[index] != ','; indexLat2++, index++)
		//		lat2[indexLat2] = message[index];
		//	lat2[indexLat2] = '\0';
		//	poiInfo->destLat = cq_atoi(lat2);

		//	// 名称2
		//	index++; //跳过','
		//	for (; message[index] != '\0' && message[index] != ','; indexName2++, index++)
		//		name2[indexName2] = message[index];
		//	name2[indexName2] = '\0';
		//	poiInfo->destName = name2;
		//}
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_VRPassPointAndDestPoint;
		//paramData->data = poiInfo;
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//m_Pool->run(job);




		/*cqCHAR message2[] = "20000023:1";
		CommandControlSig* commandStruct = NULL;;
		if (':' == message2[8] && '\0' != message2[9])
		{
		commandStruct = (CommandControlSig*)malloc(sizeof(CommandControlSig));
		cqCHAR comID = message2[9];
		commandStruct->uiCommand = (VR_COMMAND_ID)cq_atoi(&comID);
		if (',' == message2[10] && '\0' != message2[11] && commandStruct->uiCommand == VR_COMMAND_CHANGE_MAP_TO)
		{
		cqCHAR modID = message2[11];
		commandStruct->uiMapMode = (VR_COMMAND_MAP_MODE)cq_atoi(&modID);
		}
		else
		{
		commandStruct->uiMapMode = VR_COMMAND_MAP_INVALIDE;
		}
		}
		ReceiveData* paramData2 = NULL;
		paramData2 = (ReceiveData*)malloc(sizeof(ReceiveData));
		memset(paramData2, 0, sizeof(ReceiveData));
		paramData2->funcId = FuncID_Receive_VRCommandControl;
		paramData2->data = commandStruct;
		IPCWrapperJob* job2 = new IPCWrapperJob(paramData2);
		m_Pool->run(job2);*/
		
		
		 //途径点 0 删除途经地暖，1 查询途径点个数
		//cqCHAR message[] = "20000022:0";
		//int32 reqType = -1;
		//if (':' == message[8] && '\0' != message[9])
		//{
		//	cqCHAR cqtype = message[9];
		//	reqType = cq_atoi(&cqtype);
		//}
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_VRPassingPoint;
		////*((int32 *)paramData->data) = reqType;
		//paramData->data = (int32 *)malloc(sizeof(int32));
		//memcpy(paramData->data, &reqType, sizeof(int32));
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//m_Pool->run(job);

		//cqCHAR message[] = "20000022:0,11642434,3995241,东航酒店";
		//VRPassPointInfo* poiInfo = NULL;
		//if (':' == message[8] && '\0' != message[9])
		//{
		//	poiInfo = (VRPassPointInfo*)malloc(sizeof(VRPassPointInfo));
		//	cqCHAR cqtype = message[9];
		//	poiInfo->type = cq_atoi(&cqtype);
		//	if (poiInfo->type == 0 && message[10] == ',')
		//	{
		//		int indexLon = 0;
		//		int indexLat = 0;
		//		int indexName = 0;
		//		cqCHAR lon[9] = { 0 };
		//		cqCHAR lat[8] = { 0 };
		//		cqCHAR name[64] = { 0 };

		//		// 经度
		//		for (; message[indexLon + 11] != '\0' && indexLon < 8; indexLon++)
		//			lon[indexLon] = message[indexLon + 11];
		//		lon[indexLon] = '\0';
		//		poiInfo->lon = cq_atoi(lon);
		//		// 纬度
		//		for (; message[indexLat + 20] != '\0' && indexLat < 7; indexLat++)
		//			lat[indexLat] = message[indexLat + 20];
		//		lat[indexLat] = '\0';
		//		poiInfo->lat = cq_atoi(lat);
		//		// 名称
		//		for (; message[indexName + 28] != '\0' && indexName < 64; indexName++)
		//			name[indexName] = message[indexName + 28];
		//		name[indexName] = '\0';
		//		poiInfo->name = name;
		//	}

		//}
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_VRPassingPoint;
		//paramData->data = poiInfo;
		//IPCWrapperJob* job = new IPCWrapperJob(paramData);
		//m_Pool->run(job);
	}
		break;
	case FuncID_Send_SwitchRouteSucess:
	{
		printf("VR switch route sucess----- \n");
	}
		break;

	case FuncID_Send_CommandControlSucess:
	{
		printf("VR command map mode change sucess----- \n");
	}
		break;
	case FuncID_Send_CommandControlFailure:
	{
		printf("VR command map mode change failure----- \n");
	}
		break;
	case FuncID_Send_ReqSysHmiTime:
	{

		TimeInfo* timeInfo = NULL;
		timeInfo = (TimeInfo*)malloc(sizeof(TimeInfo));
		if (timeInfo == NULL)
		{
			return;
		}
		timeInfo->timeValue = 10000000;
		timeInfo->timeFormat = EN_DISPLAYSVCENGINE_TIME_FORMAT_24HOUR;

		ReceiveData* paramData = NULL;
		paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			free(timeInfo);
			timeInfo = NULL;
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_SysHmiTimeChanged;
		paramData->data = timeInfo;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
	}
		break;
	case FuncID_Send_CommandTrafficSucess:
		{
		//uint64 timeDValue = 10000000;
		//ReceiveData* paramData = NULL;
		//paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		//memset(paramData, 0, sizeof(ReceiveData));
		//paramData->funcId = FuncID_Receive_SysHmiTimeChanged;
		//paramData->data = (uint64*)malloc(sizeof(uint64));
		//memcpy(paramData->data, &timeDValue, sizeof(uint64));
		}
		break;
	case FuncID_Send_CommandTrafficFailure:
		{
			cqCHAR signalValueBuffer[80];
			VR_TRAFFIC_COMMAND_FAILED_TYPE type = *(VR_TRAFFIC_COMMAND_FAILED_TYPE*)paramData->data;
			
			if (type == VR_TRAFFIC_COMMAND_HAS_OPENED)
			{
				type = VR_TRAFFIC_COMMAND_HAS_OPENED;
			}
				/*cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_TRAFFIC_CONTROL_FAILURE);
			cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
			if (NULL != paramData->data)
			{
				cqWCHAR failedType[128];
				cq_swprintf(failedType, L"%x", type);
				cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
			}
			cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
			ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);*/	
		}
		break;
		/*========================3.18 查询距离============================*/
	case FuncID_Send_InquiryDistanceSuccess:
	{
        if (NULL == paramData->data)
		{
			
			CQ_LOG_ERROR("[ServiceManager]FuncID_Send_InquiryDistanceSuccess data is null\n");
			break;
		}
		//最终构造形式是："10000036:"
		//InquiryDistance* inquiry = (InquiryDistance*)paramData->data;
	}
		break;
	case FuncID_Send_InquiryDistanceFailure:
	{
		CQ_LOG_INFO("[ServiceManager]FuncID_Send_InquiryDistanceFailure\n");
	}
		break;
		/*========================3.18 查询距离============================*/

		/*=====================3.19 wifi/dcm连接状态=========================*/
	case FuncID_Send_getNetworkStatus:
	{									 
		// Mapbar_sleep(10000000);
		uint32 networkStatus = 1;
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_NetworkStatusChanged;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &networkStatus, sizeof(uint32));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
	}
		break;
/*=====================3.19 wifi/dcm连接状态=========================*/


/*========================3.19 G-BOOK信息=============================*/
	case FuncID_Send_getGbookInfo:
	{
		cqCHAR gbookInfo[128] = {"abcdefghizklmnopqrstuvwxyzabcdefghizklmnopqrstuvwxyzabcdefghizkl"};
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_GbookInfoChanged;
		paramData->data = (cqCHAR*)malloc(sizeof(cqCHAR)* (cq_strlen(gbookInfo) + 1));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		cq_strcpy((cqCHAR*)paramData->data, gbookInfo);
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
	}
		break;
/*========================3.19 G-BOOK信息============================*/
	case FuncID_Send_CollectionSuccess:
	{
		 uint32 collection = 0;
		 ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		 if (paramData == NULL)
		 {
			 return;
		 }
		 memset(paramData, 0, sizeof(ReceiveData));
		 paramData->funcId = FuncID_Receive_VRCollection;
		 paramData->data = (uint32*)malloc(sizeof(uint32));
		 if (paramData->data == NULL)
		 {
			 free(paramData);
			 paramData = NULL;
			 return;
		 }
		 memcpy(paramData->data, &collection, sizeof(uint32));
		 IPCWrapperJob* job = new IPCWrapperJob(paramData);
		 m_Pool->run(job);
	}
		break;
	case FuncID_Send_CollectionFailure:
	{
		CQ_LOG_INFO("[ServiceManager] FuncID_Send_CollectionFailure\n");
	}
		break;

	//case FuncID_Send_CommandControlFailure:
	//	{
	//		cqCHAR signalValueBuffer[80];
	//		VR_COMMAND_CONTROL_FAILED_TYPE type = *(VR_COMMAND_CONTROL_FAILED_TYPE*)paramData->data;
	//		//cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_COMMAND_CONRTOL_FAILURE);
	//		//cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	//		if (type == VR_COMMAND_CONTROL_FAILED_OTHER)
	//		{
	//			cqWCHAR failedType[128];
	//			cq_swprintf(failedType, L"%x", type);
	//			//cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
	//		}
	//		//cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//		//ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);	
	//	}
	//	break;
/*========================3.19 昼夜模式联动============================*/
	case FuncID_Send_GetDayNightMode:
	{
		ReceiveData* paramData = NULL;
		paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (paramData == NULL)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_SysDayNightLinkAge;
		char buff_temp[100] = {0};
		int temp_data = 0;
		sscanf("0,0","%s",buff_temp);
		paramData->data = malloc(strlen(buff_temp)+1);
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data,buff_temp,strlen(buff_temp)+1);
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		CQ_LOG_INFO("[ServiceManager]function id [%x] data is [%d]\n", paramData->funcId, paramData->data);
		m_Pool->run(job);
	}
		break;
/*========================3.19 昼夜模式联动============================*/
	default:
		break;

	}

	// free data
	if (paramData != NULL)
	{
		if (paramData->data != NULL)
		{
			free(paramData->data);
			paramData->data = NULL;
		}
		free(paramData);
		paramData = NULL;
	}
	return;
}


/**
*	@brief
*		Receiver向IPC注册添加Listener
*/
void NaviSysIPCWrapperWin32::addListener(NaviSysIPCWrapperListener *listerner)
{
	if (listerner != NULL)
		m_listerner = listerner;
	return;
}

/**
*	@brief
*		Receiver清除向IPC中注册的Listener
*/
void NaviSysIPCWrapperWin32::removeListener()
{
	if (m_listerner != NULL)
		m_listerner = NULL;
}

/**
*	@brief
*		获取音量
*/
void NaviSysIPCWrapperWin32::repAccOffSucess()
{
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	paramData->data = (void*)NULL;
	paramData->funcId = ServiceManager::FuncID_Receive_AnswerSendAccOffSucess;

	// 执行回调
	if (NULL != m_listerner)
	{
		printf("return AccOffSucess FuncID_Receive_AnswerSendAccOffSucess \n");
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//m_ReceiverCallback(paramData);
		//m_listerner->messageListener(paramData);
	}
	return;
}

/**
*	@brief
*		获取音量
*/
void NaviSysIPCWrapperWin32::getVolume()
{
	//uint32 volume = 10;
	updataVolume(*m_Volume);
	return;
}



/**
*	@brief
*		获取音量
*/
void NaviSysIPCWrapperWin32::setVolume(void* data)
{
	UNUSED_VAR(data);
	//uint32 * volume = (uint32 *)data;
	//*m_Volume = *volume;
	//updataVolume(*volume);

}

/**
*	@brief
*		通知Navi更新Volume信息
*	@return
*		int vol 音量值
*/
void NaviSysIPCWrapperWin32::updataVolume(int volume)
{
	// 构造参数
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	int mVolume;
	memset(&mVolume, 0, sizeof(int));
	mVolume = volume;
	paramData->data = (int*)malloc(sizeof(int));
	if (paramData->data == NULL)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &mVolume, sizeof(int));
	paramData->funcId = ServiceManager::FuncID_Receive_AnswerVolume;

	// 执行回调
	if (NULL != m_listerner)
	{
		printf("return updataVolume FuncID_Receive_AnswerVolume \n");
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//m_ReceiverCallback(paramData);
		//m_listerner->messageListener(paramData);
	}
	return;
}

/**
*	@brief
*		向车机获取背光信息
*/
void NaviSysIPCWrapperWin32::getBacklightMode()
{
	//BackLightMode mode;
	//BackLightType type;

	//m_pExtNavi->GetBacklightMode(mode, type);
}

/**
/************************************1.7 getLanguageID start**********************************/

/**
*	@brief
*	获取当前语言ID
	//uint32 languageId = m_pExtNavi->GetLanguageID();
	uint32 languageId = 2;
	updateLanguageID(languageId);
	return;
}

/**
*	@brief
*	通知navi更新语言ID
*/
void NaviSysIPCWrapperWin32::updateLanguageID(uint32 languageId)
{
	// 构造参数
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	uint32* mLanguageId = (uint32*)malloc(sizeof(uint32));
	if (mLanguageId == NULL)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*mLanguageId = languageId;
	paramData->data = mLanguageId;
	paramData->funcId = ServiceManager::FuncID_Receive_GetLanguageID;
	
	// 执行回调
	if (NULL != m_listerner)
	{	
		printf("return updateLanguageID FuncID_Receive_GetLanguageID \n");
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mLanguageId);
	}
	return;
}
/************************************1.7 getLanguageID end************************************/


/**
*	@brief
*	车机gps时间更新是否可用
*/
void NaviSysIPCWrapperWin32::isGPSUpdateTimeEnabled()
{
	//SVPBool sBool = m_pExtNavi->IsGPSUpdateTimeEnabled();
	bool sBool = true;
	responseGPSUpdateTimeEnabled(sBool);
	return;
}

/**
*	@brief
*	通知Navigps时间更新是否可用
*/
void NaviSysIPCWrapperWin32::responseGPSUpdateTimeEnabled(bool svpBool)
{
	// 构造参数
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	bool* mBool = (bool*)malloc(sizeof(bool));
	if (mBool == NULL)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*mBool = svpBool;
	paramData->data = mBool;
	paramData->funcId = ServiceManager::FuncID_Receive_IsGPSUpdateTimeEnabled;
	// 执行回调
	if (NULL != m_listerner)
	{
		printf("return GPSUpdateTimeEnabled FuncID_Receive_IsGPSUpdateTimeEnabled \n");
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mBool);
	}
	return;
}


/************************************1.4 backToHost start*************************************/

/**
*	@brief
*		Resume to the menu befor entering navigation APP
*/
void NaviSysIPCWrapperWin32::backToHost()
{
	//m_pExtNavi->BackToHost();
	return;
}

/************************************1.4 backToHost end***************************************/

/************************************1.5 showHostMenu start***********************************/
/**
*	@brief
*		Show host menu.
*/
void NaviSysIPCWrapperWin32::showHostMenu()
{
	//m_pExtNavi->ShowHostMenu();
	return;
}
/************************************1.5 showHostMenu end*************************************/

/************************************1.6 AudioGuidance start**********************************/

/**
*	@brief
*		请求Guidance声音是否可以播放
*/
void NaviSysIPCWrapperWin32::requestAudioGuidancePlay()
{
	//SVPBool svpBool = m_pExtNavi->RequestAudioGuidancePlay();
	bool svpBool = true;
	responseAudioGuidancePlay(svpBool);
	return;
}

/**
*	@brief
*	通知navi Guidance是否可以播放
*/
void NaviSysIPCWrapperWin32::responseAudioGuidancePlay(bool svpBool)
{
	// 构造参数
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	bool* mBool = (bool*)malloc(sizeof(bool));
	if (mBool == NULL)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*mBool = svpBool;
	paramData->data = mBool;
	paramData->funcId = ServiceManager::FuncID_Receive_ResponseAudioGuidancePlay;
	// 执行回调
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mBool);
	}
	return;
}

/************************************1.9 MapCard start****************************************/
/**
*	@brief
*	通知Navi SD card状态变化
*/
void NaviSysIPCWrapperWin32::OnMapCardChanged(bool uiAttached)
{
	// 构造参数
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	bool* mBool = (bool*)malloc(sizeof(bool));
	if (mBool == NULL)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*mBool = uiAttached;
	paramData->data = mBool;
	paramData->funcId = ServiceManager::FuncID_Receive_MapCardChanged;
	// 执行回调
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mBool);
	}
	return;
}
/**
*	@brief
*		Navi请求Beep音
*/
void NaviSysIPCWrapperWin32::reqBeep()
{
	// DeSai 新起线程 请求发声
	//Handle t = Mapbar_createThread(reqBeepProcess, this);
	//Mapbar_waitThread(t);
	//Mapbar_closeThread(t);
	return;
}

void NaviSysIPCWrapperWin32::getLanguageID()
{
	// 构造参数
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	if (paramData == NULL)
	{
		return;
	}
	int* languageId = (int*)malloc(sizeof(int));
	if (languageId == NULL)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*languageId = 1;
	paramData->data = languageId;
	paramData->funcId = ServiceManager::FuncID_Receive_GetLanguageID;
	// 执行回调
	if (NULL != m_listerner)
	{
		printf("return getLanguageID FuncID_Receive_GetLanguageID \n");
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		m_Pool->run(job);
		//m_listerner->messageListener(paramData);
	}
	else
	{
		free(languageId);
		free(paramData);
	}
	return;
}

void NaviSysIPCWrapperWin32::releaseAudioGuidancePlay()
{
	return;
}

void NaviSysIPCWrapperWin32::setRouteStatus(void * data)
{
	UNUSED_VAR(data);
	return;
}

void NaviSysIPCWrapperWin32::isCarPlayNaviActive()
{
	return;
}

//CJob::CJob(void)
//: m_JobNo(0)
//, m_JobName(NULL)
//{
//}

//CJob::~CJob(){
//	if (NULL != m_JobName)
//		free(m_JobName);
//}

//void CJob::SetJobName(char* jobname)
//{
//	if (NULL != m_JobName)    {
//		free(m_JobName);
//		m_JobName = NULL;
//	}
//	if (NULL != jobname)    {
//		m_JobName = (char*)malloc(strlen(jobname) + 1);
//		strcpy(m_JobName, jobname);
//	}
//}





//void CJob::run()
//{
//		ReceiveData* data = getJobdata();
//		if (NULL != NaviSysIPCWrapperWin32::getInstance())
//		{
//			if (NaviSysIPCWrapperWin32::getInstance()->m_shouldExit)
//			{
//				return;
//			}
//			//printf("m_job->run FUNID = %d \n", paramData->funcId);
//			NaviSysIPCWrapperWin32::getInstance()->m_listerner->messageListener(data);
//		}
		
//}




void NaviSysIPCWrapperWin32::setVersion(void* data)
{
	if (NULL != data)
	{
		MapbarVersion* ver = (MapbarVersion*)(data);
		if (NULL != ver->naviVersion || NULL != ver->mapVersion)
		{
			char * dBuf = NULL;
			int dSize = wcstombs(dBuf, ver->naviVersion, 0) + 1;
			dBuf = new char[dSize];
			memset(dBuf, 0, dSize);
			int nRet = wcstombs(dBuf, ver->naviVersion, dSize);
			if (nRet > 0)
			{
				printf("转换成功%d字符\n", nRet);
				printf("%s\n", dBuf);
				//m_pExtNavi->SetVersion(dBuf, ver->mapVersion);
			}
			
			delete []dBuf;
		}
	}

}
