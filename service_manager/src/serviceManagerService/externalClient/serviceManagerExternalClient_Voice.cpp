#include "stdafx_serviceManagerClient.h"
//#include "mbapi/mbapi_system_setting.h"
//#include "mbapi/mbapi_value.h"
#include "mapdal/message_loop.h"

namespace ServiceManager
{
	#define TIMER_VOICESTATECHK_10_SECOND	10000
	// delete by liubo
	// Handle voiceStateCHKTimer = invalidHandle;

	//=====================================================//
	//=====================================================//
	// 函数声明
	void VoiceSvcClient_ReqStopEMG(void* data);
	void VoiceSvcClient_ReqVoiceStateCHK(void* data);
	int VoiceSvcClient_SoundArbiter_ForceStop(void* data);

	/**
	*	@brief
	*		VoiceSvc初期化
	*	@return
	*		void
	*	@remarks
	*		向Recieve模块和Mbapi模块注册Callback函数
	*/
	void VoiceSvcClient_Initialize()
	{

		//向Recieve模块进行Callback函数注册
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_reqStopVoiceEMG, VoiceSvcClient_ReqStopEMG);//紧急发声结束要求
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_checkVoiceState, VoiceSvcClient_ReqVoiceStateCHK);//发声状态查询

		//启动时获取引擎音量，并发送给车机
		int32 volume = MbApi::SystemSound_getVolume();
		CQ_LOG_INFO("[ServiceManager]Func VoiceSvcClient_Initialize volume = %d.\n", volume);
		VoiceSvcClient_SetNaviVolume(&volume);

		return;
	}

	
	/**
	*	@brief
	*		VoiceSvc清除
	*	@return
	*		void
	*/
	//delete by shenghao for exit timeout
	//void VoiceSvcClient_CleanUp()
	//{
	//	CQ_LOG_INFO("[ServiceManager] VoiceSvcClient_CleanUp.\n");
	//	// delete by liubo
	//	// voiceStateCHKTimer = invalidHandle;
	//	return;
	//}
	
	/*========================2.2 导航诱导音 结束紧急发声============================*/
	/**
	*	@brief
	*		请求导航紧急发声结束
	*	@return
	*	 	-
	*/
	int VoiceSvcClient_SoundArbiter_ForceStop(void* data)
	{
		UNUSED_VAR(data);
		MbApi::SystemSound_forceStop();

		return (int)ServiceManager::EN_VOICESVCCLIENT_FUNCTIONINMAINTHREAD_OK;
	}

	/**
	*	@brief
	*		车机请求导航紧急发声结束
	*	@return
	*	 	void
	*/
	void VoiceSvcClient_ReqStopEMG(void* data)
	{
		// delete by liubo
		// Receiver中进行释放 
		UNUSED_VAR(data);
		/*
		if (data != NULL)
		{
			free(data);
			data = NULL;
		}
		*/
		
		MbApi::Mapbar_runFunctionInMainThread(VoiceSvcClient_SoundArbiter_ForceStop, NULL);//Mbapi:typedef int(*Mapbar_mainThreadFunc)(void* params);

		return;
	}
	/*========================2.2 导航诱导音 结束紧急发声============================*/
	//
	//
	//
	/*========================2.2 导航诱导音 导航查询发声状态============================*/
	/**
	*	@brief
	*		查询发声状态
	*	@return
	*	 	-
	*/
	int VoiceSvcClient_SoundArbiter_IsPlaying(void* data)
	{
		BOOL isPlaying = MbApi::SystemSound_isPlaying();

		*(BOOL*)data = isPlaying;

		return (int)ServiceManager::EN_VOICESVCCLIENT_FUNCTIONINMAINTHREAD_OK;
	}

	// delete by liubo
	/*
	BOOL Judge_SoundArbiter_IsPlaying()
	{
		BOOL state = FALSE;
		MbApi::Mapbar_runFunctionInMainThread(VoiceSvcClient_SoundArbiter_IsPlaying, (void*)&state);
		return state;
	}
	*/

	// delete by liubo
	/*
	void Send_VoiceStateCHKMsg(void* data)
	{
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->funcId = ServiceManager::FuncID_Send_CurrentVoiceState;
		paramData->data = (BOOL*)malloc(sizeof(BOOL));
		paramData->data = (void*)data;

		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
	}
	*/


	/**
	*	@brief
	*		车机请求导航查询发声状态
	*	@return
	*	 	void
	*/
	void VoiceSvcClient_ReqVoiceStateCHK(void* data)
	{
		// delete by liubo
		// Receiver中进行释放
		UNUSED_VAR(data);
		/*
		if (data != NULL)
		{
			free(data);
			data = NULL;
		}
		*/

		// fix by liubo
		// voiceStateCHKTimer = MbApi::Start_timer(invalidHandle, TIMER_VOICESTATECHK_10_SECOND, Send_VoiceStateCHKMsg, (void*)Judge_SoundArbiter_IsPlaying());
		BOOL state = FALSE;
		MbApi::Mapbar_runFunctionInMainThread(VoiceSvcClient_SoundArbiter_IsPlaying, (void*)&state);
		CQ_LOG_INFO("[ServiceManager]Got sound state, %d.\n", state);

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->funcId = ServiceManager::FuncID_Send_CurrentVoiceState;
		paramData->data = (BOOL*)malloc(sizeof(BOOL));
		memcpy(paramData->data, &state, sizeof(BOOL));
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return;
	}
	/*========================2.2 导航诱导音 导航查询发声状态============================*/

	
	/**
	*	@brief
	*		导航向车机推送声音音量变化
	*	@return
	*	 	void
	
	BOOL VoiceSvcClient::onGeneralEventNotify(MbApi::MessageDistribution::GeneralEvent event, void* data)
	{
		UNUSED_VAR(data);
		switch (event)
		{
			case MbApi::MessageDistribution::GeneralEvent_volumeChanged:
				{
					VoiceSvcClient_SetNaviVolume(data);
				}
				break;
			default:
				break;
		}

		return true;
	}
	*/

	void VoiceSvcClient_SetNaviVolume(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] volume value is NULL!\n");
			return;
		}
			
		CQ_LOG_INFO("[ServiceManager] VoiceSvcClient_SetNaviVolume\n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->funcId = ServiceManager::FuncID_Send_SetVolume;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		memcpy(paramData->data, data, sizeof(uint32));
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		
		return;
	}

}
