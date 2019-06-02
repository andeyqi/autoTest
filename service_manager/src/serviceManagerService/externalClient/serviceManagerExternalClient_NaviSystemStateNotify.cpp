#include "stdafx_serviceManagerClient.h"
//#include "serviceManagerExternalClient_NaviSystemStateNotify.h"
//#include "mbapi/mbapi_system_setting.h"
//#include "mbapi/mbapi_value.h"
#include "mapdal/message_loop.h"
//#include "mbapi/mbapi_app_service.h"
//#include "cq_stdlib/jansson/jansson.h"
//#include "mbapi/mbapi_navi_config.h"
//#include "serviceManagerEngine_NaviSystemStateNotify.h"
//delete by sheng Mbapiɾ��
//#include "mbapi/mbapi_update_manager.h"
#include "serviceManagerEngine_Voice.h"
//#include <sys/time.h>
namespace ServiceManager
{
	//=====================================================//
	static NaviSystemStateNotifySvcClient* g_NaviSystemStateNotifySvcClient = NULL;
	static Handle failsafeTimer = invalidHandle;	// failsafeTimer����ÿ45�뷢��һ��live״̬
	// add by liubo 
	static BOOL g_NaviSystemStateNotifyStateFlag = FALSE;  // ģ��״̬��־: TRUE ��ʾģ��������ɣ�FALSE ��ʾģ���˳����
	//=====================================================//
#if 1
		void NaviSystemStateNotifySvcClient_SendStopSuc();
#endif

	// ��������
	void NaviSystemStateNotifySvcClient_NotifyNaviLiveStatus(void* data);
	//delete by shenghao for exit timeout 20180711
	//void clientSendNaviStopSuc();

	/**
	*	@brief
	*		NaviSystemStateNotifySvcClient���ڻ�
	*	@return
	*		void
	*	@remarks
	*		��Recieveģ���Mbapiģ��ע��Callback����
	*/
	void NaviSystemStateNotifySvcClient_Initialize()
	{
		// add by liubo
		// �ж�ģ��״̬
		if (TRUE == g_NaviSystemStateNotifyStateFlag)
		{
			CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient is running.\n");
			return;
		}

		// ��MbApi���Listener
		if (NULL == g_NaviSystemStateNotifySvcClient)
		{
			g_NaviSystemStateNotifySvcClient = new NaviSystemStateNotifySvcClient();
			g_NaviSystemStateNotifySvcClient->initialize();
		}

		//��Recieveģ�����Callback����ע��
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_reqACCOFF, NaviSystemStateNotifySvcClient_ReqACCOFF);

		//ģ����ڻ�ʱ����֪ͨ����Navi�������
		if (MbApi::AppService::getInstance()->isInitCompleted())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_NaviStartSuc;
			paramData->data = (void*)NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

			// failsafe
			if (invalidHandle == failsafeTimer)
			{
				failsafeTimer = MbApi::Start_timer(invalidHandle, TIMER_NAVI_IS_LIVE, NaviSystemStateNotifySvcClient_NotifyNaviLiveStatus, NULL);
			}
		}
		
		//ģ����ڻ�ʱ����֪ͨ����Navi GPS��������
		if (MbApi::Gps_getInfo()->m_valid)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_GPSRcvSuc;
			paramData->data = (void*)NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}

		// add by liubo
		// ģ���������
		g_NaviSystemStateNotifyStateFlag = TRUE;
		// add by renxs ֪ͨEngineVoice������ʾ������
		VoiceSvcEngine::getInstance()->setClientInitFlag(true);
		return;
	}


	//delete by shenghao for exit timeout 20180711
	/**
	*	@brief
	*		NaviSystemStateNotifySvcClient���
	*	@param [in]
	*		void
	*	@return
	*		void
	*/
	//void NaviSystemStateNotifySvcClient_CleanUp(void)
	//{
	//	 add by liubo
	//	 ģ��״̬�ж�
	//	if (FALSE == g_NaviSystemStateNotifyStateFlag)
	//	{
	//		CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient is not running.\n");
	//		return;
	//	}

	//	clientSendNaviStopSuc();
	//	delete by shenghao for exit timeout
	//	if (NULL != g_NaviSystemStateNotifySvcClient)
	//	{
	//		delete g_NaviSystemStateNotifySvcClient;
	//		g_NaviSystemStateNotifySvcClient = NULL;
	//	}

	//	 ����������ֹͣ��������
	//	if (invalidHandle != failsafeTimer)
	//	{
	//		MbApi::Stop_timer(failsafeTimer);
	//		failsafeTimer = invalidHandle;
	//	}

	//	 add by liubo
	//	 ģ���˳�
	//		g_NaviSystemStateNotifyStateFlag = FALSE;
	//	CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient_CleanUp.\n");
	//	return;
	//}

	
	/**
	*	@brief
	*		���캯��
	*/
	NaviSystemStateNotifySvcClient::NaviSystemStateNotifySvcClient()
	{
	}

	
	/**
	*	@brief
	*		��������
	*/
	NaviSystemStateNotifySvcClient::~NaviSystemStateNotifySvcClient()
	{
	}

	
	/**
	*	@brief
	*	
	*/	
	void NaviSystemStateNotifySvcClient::initialize()
	{
		MbApi::MessageDistribution::getInstance()->addGpsEventListener(MbApi::MessageDistribution::Level_medium, this);
		MbApi::MessageDistribution::getInstance()->addGeneralEventListener(MbApi::MessageDistribution::Level_medium, this);
		return;
	}

	/*=============================2.6 FailSafe===============================*/
	/**
	*	@brief
	*		֪ͨ����live״̬
	*	@return
	*	 	void
	*/
	void NaviSystemStateNotifySvcClient_NotifyNaviLiveStatus(void* data)
	{
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:Send navi live status.\n", __FUNCTION__, __LINE__);
		UNUSED_VAR(data);
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->funcId = ServiceManager::FuncID_Send_NotifyNaviLiveStatus;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return;
	}
	/*=============================2.6 FailSafe===============================*/
	//
	//
	//
	/*========================2.1 ��������/GPS����============================*/
	/**
	*	@brief
	*		�����򳵻�����Navi������ɵ�֪ͨ
	*	@return
	*	 	BOOL
	*/
	BOOL NaviSystemStateNotifySvcClient::onGeneralEventNotify(MbApi::MessageDistribution::GeneralEvent event, void* data)
	{
		UNUSED_VAR(data);
		switch (event)
		{
			case MbApi::MessageDistribution::GeneralEvent_mbapiInitComplete://MbApi��ʼ����ϣ�Initialize������ϡ�
				{
					ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
					paramData->funcId = ServiceManager::FuncID_Send_NaviStartSuc;
					paramData->data = (void*)NULL;
					NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

					// failsafe
					if (invalidHandle == failsafeTimer)
					{
						failsafeTimer = MbApi::Start_timer(invalidHandle, TIMER_NAVI_IS_LIVE, NaviSystemStateNotifySvcClient_NotifyNaviLiveStatus, NULL);
					}
				}
				break;
			default:
				break;
		}

		return true;
	}

	
	/**
	*	@brief
	*		�����򳵻�����GPS����������֪ͨ
	*	@return
	*	 	BOOL
	*/
	BOOL NaviSystemStateNotifySvcClient::onGpsEventNotify(MbApi::MessageDistribution::GpsEvent event, void* data)
	{
		UNUSED_VAR(data);
		switch (event)
		{
			case MbApi::MessageDistribution::GpsEvent_connected://GPS����֪ͨ
				{
					if (MbApi::Gps_getInfo()->m_valid)
					{
						ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
						paramData->funcId = ServiceManager::FuncID_Send_GPSRcvSuc;
						paramData->data = (void*)NULL;
						NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
					}
				}
				break;
			default:
				break;
		}

		return true;
	}
	/*========================2.1 ��������/GPS����============================*/
	//
	//
	//
	/*========================2.4 ACC OFF�ź�============================*/
	/**
	*	@brief
	*		����֪ͨ����ACC OFF��Ҫ���ݱ���
	*	@return
	*	 	void
	*/
	void NaviSystemStateNotifySvcClient_ReqACCOFF(void *data)
	{
		CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient_ReqACCOFF\n");
		UNUSED_VAR(data);
		// ���浼������
		//MbApi::Config_saveConfig();
		//MbApi::UpdateManager_saveKeyMember();
		//MbApi::ConfigManager::getInstance()->saveToFile();
		//RecordCollection::recorderCollectionSave();
		// ���õ����˳�

		//UnixTimestamp time = Mapbar_getCurrentTimeMillis();
		//uint32 hours = (time / 1000 / 60 / 60 + 8) % 24;
		//uint32 minutes = time / 1000 / 60 % 60;
		//uint32 seconds = time / 1000 % 60;
		//uint32 milliseconds = time % 1000;
		//printf("DATE: %02d%02d%02d%02d %s \n", hours, minutes, seconds, milliseconds, "ACCOFF start!\n");

		MbApi::AppService::getInstance()->exit(0);
		return;
	}
	/*==================== 2.4 ACC OFF�ź� ============================ */
	//delete by shenghao for exit timeout 20180711
	/**
	*	@brief
	*		�����˳�ʱ������stop success
	*/
	//void clientSendNaviStopSuc(void)
	//{
	//	CQ_LOG_INFO("[ServiceManager] sendStopSuc FuncID_Send_NaviStopSuc\n");
	//	ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
	//	paramData->funcId = ServiceManager::FuncID_Send_NaviStopSuc;
	//	paramData->data = (void*)NULL;
	//	NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
	//	SystemNotify::getInstance()->setSendStopMsgFlag(true);
	//	return;
	//}
}

