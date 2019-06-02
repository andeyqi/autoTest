#include "stdafx_serviceManagerClient.h"
//#include "serviceManagerExternalClient_Addition.h"
#include "serviceManagerEngine_Addition.h"
//#ifdef MAPBAR_TOYOTA
//#include <unistd.h>
//#endif
namespace ServiceManager
{
	// ��������
	static void AdditionServiceClient_ReqHistoryClear(void* data);
	void AdditionSvcClient_ReqGetGPSTime(void *data);
	void AdditionSvcClient_SetVersionToSystem(void *data);

	/**
	*	@brief
	*		��ʼ��
	*/
	void AdditionSvcClient_Initialize(void)
	{
		// ��Receiverע��callback(��ͬ��)
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_HistoryClear, AdditionServiceClient_ReqHistoryClear);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_reqGetGPSTimeValue, AdditionSvcClient_ReqGetGPSTime);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_ReqNaviVersion, AdditionSvcClient_SetVersionToSystem);
		return;
	}

	/**
	*	@brief
	*		cleanup
	*/
	//delete by shenghao for exit timeout
	//void AdditionSvcClient_CleanUp(void)
	//{
	//	CQ_LOG_INFO("[ServiceManager] AdditionSvcClient_CleanUp.\n");
	//	return;
	//}

	/*=============================�����ʷ��¼============================*/
	/**
	*	@brief
	*		�����ʷ��¼
	*	@param [in] void
	*	@return int
	*		HISTORYCLEARSUCCESS������ɹ���
	*		HISTORYCLEARFAILED�����ʧ��
	*/
	static int MemoryClear_proc(void* data)
	{
		UNUSED_VAR(data);
		//ȡ������
		//MbApi::Navi_reqCancelNavigation();
		//MbApi::Navi_reqForceSpeakerStop();
		//MbApi::Map_removeAllRoutes();
		////������ʷ
		//MbApi::Config_deleteUserData(HISTORY_KEY_STORE_FILENAME);
		//MbApi::Config_deleteUserData(HISTORY_CROSS_STORE_FILENAME);
		//MbApi::Config_deleteUserData(HISTORY_ADDRESS_STORE_FILENAME);
		//MbApi::Config_deleteUserData(HISTORY_TOTAL_STORE_FILENAME);
		//MbApi::Config_deleteUserData(HISTORY_INTIAL_STORE_FILENAME);
		//// ��ʷĿ�ĵ�
		//MbApi::HistoryDestination_clearAllItems();
		//// ������ʷ��¼
		//MbApi::HistorySearch_clearAllItems();
		//// �ղ���ʷ
		//FAVORITE_SERVICE()->deleteAllFavorite();
		//// ������
		//CAMERA_SERVICE()->deleteAllCustomCamera();
		//// ���Ŀ�ĵ�
		//MbApi::MapFormData* mapData = MbApi::GetMapFormData();
		//MbApi::Config_deleteUserData(L"userdata/navi_shortcuts.bin");
		//SHORTCUT_SERVICE()->deleteAllFavorite();
		//mapData->m_curCommonAdrIndex = -1;
		MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
		*pCmdBackSig = MBAPI_VR_COMMAND_DID_SUCESS;
		MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_MEMORY_CLEAR, NULL, pCmdBackSig);
		
/*#ifdef MAPBAR_TOYOTA
		sync();
#endif	*/	
		if (AdditionSvcEngine::getInstance()->getMemoryClearFlag())
		{
			SendData* paramData = (SendData*)malloc(sizeof(SendData));
			paramData->funcId = FuncID_Send_HistoryClear;
			paramData->data = (void*)NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			AdditionSvcEngine::getInstance()->setMemoryClearFlag(FALSE);
		}
		free(pCmdBackSig);
		pCmdBackSig = NULL;
		//MbApi::AppService::getInstance()->exit(0);
		return HISTORYCLEARSUCCESS;
	}

	/**
	*	@brief
	*		�������������ʷ��¼
	*	@param [in] void
	*	@return void
	*/
	static void AdditionServiceClient_ReqHistoryClear(void* data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->setMemoryClearFlag(TRUE);
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:request history clear.\n", __FUNCTION__, __LINE__);
		int retValue = MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)MemoryClear_proc, NULL);
		CQ_LOG_INFO("[ServiceManager]:Return value is %d.\n", retValue);
		// ����ɹ�֪ͨ�����ʧ�ܲ�֪ͨ
		if (HISTORYCLEARSUCCESS == retValue)
		{
			/*SendData* paramData = (SendData*)malloc(sizeof(SendData));
			paramData->funcId = FuncID_Send_HistoryClear;
			paramData->data = (void*)NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);*/
		}

		return;
	}
	/*=============================�����ʷ��¼============================*/
	//
	//
	//
	/*===========================2.5 GPSʱ��ȡ��============================*/
	/**
	*	@brief
	*		��ȡGPSʱ��
	*	@return
	*	 	-
	*/
	int AdditionSvcClient_Gps_getInfo(void* params)
	{
		memcpy(params, MbApi::Gps_getInfo(), sizeof(MbApi::GpsInfo));

		return (int)ServiceManager::EN_SETTINGSVCCLIENT_FUNCTIONINMAINTHREAD_OK;
	}

	/**
	*	@brief
	*		����֪ͨ������ȡGPSʱ��
	*	@return
	*	 	void
	*/
	void AdditionSvcClient_ReqGetGPSTime(void *data)
	{
		UNUSED_VAR(data);
		MbApi::GpsInfo mbapiGpsInfo;
		memset(&mbapiGpsInfo, 0, sizeof(MbApi::GpsInfo));
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)AdditionSvcClient_Gps_getInfo, &mbapiGpsInfo);

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (mbapiGpsInfo.m_valid)
		{
			paramData->funcId = ServiceManager::FuncID_Send_CurrentGPSTimeValue;
			paramData->data = (UnixTimestamp*)malloc(sizeof(UnixTimestamp));
			*((UnixTimestamp*)paramData->data) = (DateTime_toUnixTimestamp((const DateTime *)&mbapiGpsInfo.m_dateTime)/1000) + (8*3600);
		}
		else
		{
			paramData->funcId = ServiceManager::FuncID_Send_CurrentGPSTimeValueFail;
			paramData->data = NULL;
		}

		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		return;
	}
	/*========================2.5 GPSʱ��ȡ��============================*/

	/**
	*	@brief
	*		���õ����汾�š���ͼ�汾�Ÿ�����
	*/
	void AdditionSvcClient_SetVersionToSystem(void *data)
	{
		UNUSED_VAR(data);
		MapbarVersion *ver = (MapbarVersion*)malloc(sizeof(MapbarVersion));
		ver->naviVersion = NULL;
		ver->mapVersion = NULL;
		// �洢����汾��Ϣ
		const cqWCHAR *naviVersion = NULL;
		NaviDataCenter* dc = MbApi::Session_getDataCenter();
		// �洢��ͼ�汾��Ϣ
		cqWCHAR mapVersion[128];
		memset(mapVersion, 0x00, sizeof(mapVersion));
		
		//��ȡ����汾
#ifdef MAPBAR_LINUX_GYRO
		naviVersion = MbApi::IniFile_getValue(dc->m_iniFile, L"custom", L"DataVersion_DR");
#else
		naviVersion = MbApi::IniFile_getValue(dc->m_iniFile, L"custom", L"DataVersion");
#endif
		if (naviVersion)
		{
			cqWCHAR tmp[128] = { 0 };
			cq_wcscpy(tmp, naviVersion);
			ver->naviVersion = (cqWCHAR*)malloc(sizeof(tmp));
			cq_wcscpy_s(ver->naviVersion, sizeof(tmp), tmp);
		}
		else
		{
			SendData* paramData = (SendData*)malloc(sizeof(SendData));
			paramData->funcId = FuncID_Send_GetVersionFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			if (ver != NULL)
			{
				free(ver);
				ver = NULL;
			}
			return;
		}
		
		// ��ȡ��ͼ�汾
		if (MbApi::App_getNaviVersionData(L"cn/base.dat", mapVersion, element_of(mapVersion)))
		{
			cqWCHAR tmp[128] = { 0 };
			cq_wcscpy(tmp, mapVersion);
			ver->mapVersion = (cqWCHAR*)malloc(sizeof(tmp));
			cq_wcscpy_s(ver->mapVersion, sizeof(tmp), tmp);
		}
		else
		{
			SendData* paramData = (SendData*)malloc(sizeof(SendData));
			paramData->funcId = FuncID_Send_GetVersionFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			if (ver != NULL)
			{
				if (ver->naviVersion != NULL)
				{
					free(ver->naviVersion);
					ver->naviVersion = NULL;
				}
				free(ver);
				ver = NULL;
			}
			return;
		}

		// ����SendData ������
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_GetVersionSuccess;
		paramData->data = ver;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}
}
