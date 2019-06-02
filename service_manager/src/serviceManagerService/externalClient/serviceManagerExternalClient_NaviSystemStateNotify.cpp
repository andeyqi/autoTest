#include "stdafx_serviceManagerClient.h"
//#include "serviceManagerExternalClient_NaviSystemStateNotify.h"
//#include "mbapi/mbapi_system_setting.h"
//#include "mbapi/mbapi_value.h"
#include "mapdal/message_loop.h"
//#include "mbapi/mbapi_app_service.h"
//#include "cq_stdlib/jansson/jansson.h"
//#include "mbapi/mbapi_navi_config.h"
//#include "serviceManagerEngine_NaviSystemStateNotify.h"
//delete by sheng Mbapi删除
//#include "mbapi/mbapi_update_manager.h"
#include "serviceManagerEngine_Voice.h"
//#include <sys/time.h>
namespace ServiceManager
{
	//=====================================================//
	static NaviSystemStateNotifySvcClient* g_NaviSystemStateNotifySvcClient = NULL;
	static Handle failsafeTimer = invalidHandle;	// failsafeTimer控制每45秒发送一次live状态
	// add by liubo 
	static BOOL g_NaviSystemStateNotifyStateFlag = FALSE;  // 模块状态标志: TRUE 表示模块启动完成；FALSE 表示模块退出完成
	//=====================================================//
#if 1
		void NaviSystemStateNotifySvcClient_SendStopSuc();
#endif

	// 函数声明
	void NaviSystemStateNotifySvcClient_NotifyNaviLiveStatus(void* data);
	//delete by shenghao for exit timeout 20180711
	//void clientSendNaviStopSuc();

	/**
	*	@brief
	*		NaviSystemStateNotifySvcClient初期化
	*	@return
	*		void
	*	@remarks
	*		向Recieve模块和Mbapi模块注册Callback函数
	*/
	void NaviSystemStateNotifySvcClient_Initialize()
	{
		// add by liubo
		// 判断模块状态
		if (TRUE == g_NaviSystemStateNotifyStateFlag)
		{
			CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient is running.\n");
			return;
		}

		// 向MbApi添加Listener
		if (NULL == g_NaviSystemStateNotifySvcClient)
		{
			g_NaviSystemStateNotifySvcClient = new NaviSystemStateNotifySvcClient();
			g_NaviSystemStateNotifySvcClient->initialize();
		}

		//向Recieve模块进行Callback函数注册
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_reqACCOFF, NaviSystemStateNotifySvcClient_ReqACCOFF);

		//模块初期化时主动通知车机Navi启动完成
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
		
		//模块初期化时主动通知车机Navi GPS受信正常
		if (MbApi::Gps_getInfo()->m_valid)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_GPSRcvSuc;
			paramData->data = (void*)NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}

		// add by liubo
		// 模块启动完成
		g_NaviSystemStateNotifyStateFlag = TRUE;
		// add by renxs 通知EngineVoice可以显示音量条
		VoiceSvcEngine::getInstance()->setClientInitFlag(true);
		return;
	}


	//delete by shenghao for exit timeout 20180711
	/**
	*	@brief
	*		NaviSystemStateNotifySvcClient清空
	*	@param [in]
	*		void
	*	@return
	*		void
	*/
	//void NaviSystemStateNotifySvcClient_CleanUp(void)
	//{
	//	 add by liubo
	//	 模块状态判断
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

	//	 导航结束后，停止发送心跳
	//	if (invalidHandle != failsafeTimer)
	//	{
	//		MbApi::Stop_timer(failsafeTimer);
	//		failsafeTimer = invalidHandle;
	//	}

	//	 add by liubo
	//	 模块退出
	//		g_NaviSystemStateNotifyStateFlag = FALSE;
	//	CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient_CleanUp.\n");
	//	return;
	//}

	
	/**
	*	@brief
	*		构造函数
	*/
	NaviSystemStateNotifySvcClient::NaviSystemStateNotifySvcClient()
	{
	}

	
	/**
	*	@brief
	*		析构函数
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
	*		通知导航live状态
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
	/*========================2.1 导航启动/GPS受信============================*/
	/**
	*	@brief
	*		导航向车机推送Navi启动完成的通知
	*	@return
	*	 	BOOL
	*/
	BOOL NaviSystemStateNotifySvcClient::onGeneralEventNotify(MbApi::MessageDistribution::GeneralEvent event, void* data)
	{
		UNUSED_VAR(data);
		switch (event)
		{
			case MbApi::MessageDistribution::GeneralEvent_mbapiInitComplete://MbApi初始化完毕，Initialize过程完毕。
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
	*		导航向车机推送GPS受信正常的通知
	*	@return
	*	 	BOOL
	*/
	BOOL NaviSystemStateNotifySvcClient::onGpsEventNotify(MbApi::MessageDistribution::GpsEvent event, void* data)
	{
		UNUSED_VAR(data);
		switch (event)
		{
			case MbApi::MessageDistribution::GpsEvent_connected://GPS连接通知
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
	/*========================2.1 导航启动/GPS受信============================*/
	//
	//
	//
	/*========================2.4 ACC OFF信号============================*/
	/**
	*	@brief
	*		车机通知导航ACC OFF需要数据保存
	*	@return
	*	 	void
	*/
	void NaviSystemStateNotifySvcClient_ReqACCOFF(void *data)
	{
		CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcClient_ReqACCOFF\n");
		UNUSED_VAR(data);
		// 保存导航数据
		//MbApi::Config_saveConfig();
		//MbApi::UpdateManager_saveKeyMember();
		//MbApi::ConfigManager::getInstance()->saveToFile();
		//RecordCollection::recorderCollectionSave();
		// 调用导航退出

		//UnixTimestamp time = Mapbar_getCurrentTimeMillis();
		//uint32 hours = (time / 1000 / 60 / 60 + 8) % 24;
		//uint32 minutes = time / 1000 / 60 % 60;
		//uint32 seconds = time / 1000 % 60;
		//uint32 milliseconds = time % 1000;
		//printf("DATE: %02d%02d%02d%02d %s \n", hours, minutes, seconds, milliseconds, "ACCOFF start!\n");

		MbApi::AppService::getInstance()->exit(0);
		return;
	}
	/*==================== 2.4 ACC OFF信号 ============================ */
	//delete by shenghao for exit timeout 20180711
	/**
	*	@brief
	*		导航退出时，发送stop success
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

