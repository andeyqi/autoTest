#include "stdafx_serviceManagerEngine.h"
//#include "serviceManagerEngine_Voice.h"
#include <string>
//#include <sstream>

namespace ServiceManager
{
	VoiceSvcEngine* VoiceSvcEngine::m_instance = NULL;

	// 函数声明
	void AnswerVoiceStart(void* data);
	void AnswerVolume(void* data);
	void AnswerMute(void* data);

	/**
	*	@brief
	*		实例化
	*/
	VoiceSvcEngine* VoiceSvcEngine::getInstance()
	{
		if (NULL == m_instance)
		{
			m_instance = new VoiceSvcEngine();
			m_instance->startService();
		}

		return m_instance;
	}

	/**
	*	@brief
	*		释放对象
	*/
	//delete by shenghao for exit timeout
	//void VoiceSvcEngine::releaseInstance()
	//{
	//	CQ_LOG_INFO("[ServiceManager] VoiceSvcEngine::releaseInstance.\n");
	//	if (NULL != m_instance)
	//	{
	//		m_instance->stopService();
	//		delete m_instance;
	//		m_instance = NULL;
	//	}
	//	return;
	//}

	/**
	*	@brief
	*		构造函数
	*/
	VoiceSvcEngine::VoiceSvcEngine()
		:m_reqVoiceStartEvent(invalidHandle),
		m_volumeEvent(invalidHandle),
		isClientInitCompleted(false)
	{

		m_volume = 0xffffffff;
	};

	/**
	*	@brief
	*		析构函数
	*/
	VoiceSvcEngine::~VoiceSvcEngine(){};

	/**
	*	@brief
	*		初始化
	*/
	bool VoiceSvcEngine::InitInstance(void)
	{
		//同期Callback函数注册
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerVoiceStart, AnswerVoiceStart);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerVolume, AnswerVolume);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerMute, AnswerMute);

		return true;
	}

	/*========================2.2 导航诱导音 车机开始发声============================*/
	/**
	*	@brief
	*		导航请求车机开始发声(同期要求)
	*	@return
	*	 	BOOL
	*/
	BOOL VoiceSvcEngine::ReqVoiceStart()
	{
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
/* qihc add start*/
		/* 导航在后台状态且当前状态模拟导航,或者导航中时 */
		if(ServiceManager::DisplaySvcEngine::getInstance()->GetNaviRunStatus() == true)
		{
			NavigationStatus status;
			status = NaviSession_getNaviStatus();
			if(!((status == NavigationStatus_inNavi && NaviSession_isNaviPaused() == FALSE) ||\
				NaviSession_getSimulationState() == TRUE))	//获取模拟导航状态接口变更 NaviSession_isInSimulation() -> NaviSession_getSimulationState()
			{
				SVC_MGE_LOG_INFO("[ServiceManager] back can't speak [%s] [%d]\n",__FUNCTION__,__LINE__);
				return FALSE;
			}
		}
/* qihc add end */
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqVoiceStart;
		paramData->data = (void*)NULL;
		m_reqVoiceStartEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_reqVoiceStartEvent, 1000);//等待1秒
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_reqVoiceStartEvent, 1000);//等待1秒
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_reqVoiceStartEvent);
			m_reqVoiceStartEvent = invalidHandle;   // add by liubo
			return TRUE;
		}

		Mapbar_destroyEvent(m_reqVoiceStartEvent);
		m_reqVoiceStartEvent = invalidHandle;	// add by liubo
		return FALSE;
	}

	/**
	*	@brief
	*		向Reciever注册的开始车机发声要求同期Callback函数
	*	@return
	*		void
	*/
	void AnswerVoiceStart(void* data)
	{
		// delete by liubo
		// Receiver 中进行释放
		UNUSED_VAR(data);
		/*
		if (data != NULL)
		{
			free(data);
			data = NULL;
		}
		*/

		if (VoiceSvcEngine::getInstance()->m_reqVoiceStartEvent != invalidHandle)
			Mapbar_setEvent(VoiceSvcEngine::getInstance()->m_reqVoiceStartEvent);

		return;
	}
	/*========================2.2 导航诱导音 车机开始发声============================*/
	//
	//
	//
	/*========================2.2 导航诱导音 车机结束发声============================*/
	/**
	*	@brief
	*		导航请求车机结束发声(同期要求)
	*	@return
	*	 	void
	*/
	BOOL VoiceSvcEngine::ReqVoiceStop()
	{
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqVoiceStop;
		paramData->data = (void*)NULL;

		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		return TRUE;
	}
	/*========================2.2 导航诱导音 车机结束发声============================*/
	//
	//
	//
	/*========================2.2 导航诱导音 BEEP发声要求============================*/
	/**
	*	@brief
	*		导航请求车机要求BEEP发声
	*	@return
	*	 	void
	*/
	void VoiceSvcEngine::ReqPlayBeepSound(BEEP_TYPE beepType)
	{
		BEEP_TYPE* pData = (BEEP_TYPE*)malloc(sizeof(BEEP_TYPE));
		if (pData == NULL)
		{
			return;
		}
		*pData = beepType;
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			free(pData);
			pData = NULL;
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqPlayBeepSound;
		paramData->data = pData;

		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		return;
	}
	/*========================2.2 导航诱导音 BEEP发声要求============================*/

	/*====================================德赛=======================================*/
	/**
	*	@brief
	*		导航请求车机提供音量值
	*	@return
	*	 	uint32 返回值介于0~100之间时为有效值；
	*			   返回值大于100的情况为无效值，需要等待异步callback
	*/
	uint32 VoiceSvcEngine::ReqSysVolume()
	{
		CQ_LOG_INFO("[ServiceManager] ReqSysVolume\n" );
		if (m_volume <= 100)
		{
			return m_volume;
		}

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return 0;
		}
		paramData->funcId = ServiceManager::FuncID_Send_GetVolume;
		paramData->data = (void*)NULL;

		m_volumeEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_volumeEvent, 1000);//等待1秒
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_volumeEvent, 1000);//等待1秒
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_volumeEvent);
			m_volumeEvent = invalidHandle;
			return m_volume;
		}

		Mapbar_destroyEvent(m_volumeEvent);
		m_volumeEvent = invalidHandle;
		return 0;
	}

	/**
	*	@brief
	*		Volume状态变化通知
	*	@param [in] uint32
	*		para 音量值
	*	@return void
	*/
	void VoiceSvcEngine::VolumeChangedNotify(uint32* para)
	{
		CQ_LOG_INFO("[ServiceManager] VoiceSvcEngine::VolumeChangedNotify volume = %d \n", *para);
		std::set<VoiceSvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((VoiceSvcEngineListener*)*iter)->onVolumeChangeNotify(para);
		}
		return;
	}

	// 通知函数
	void VolumeChangedNotify(void* para)
	{
		CQ_LOG_INFO("[ServiceManager] VolumeChangedNotify\n");
		VoiceSvcEngine::getInstance()->VolumeChangedNotify((uint32*)para);
		return;
	}

	/**
	*	@brief
	*		向Reciever注册的音量同期Callback函数
	*	@param [in] data
	*		音量
	*	@return
	*		void
	*/
	void AnswerVolume(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AnswerVolume NULL!\n");
			return;
		}
		VoiceSvcEngine* ins = VoiceSvcEngine::getInstance();
		ins->m_volume = *(int32*)data;
		CQ_LOG_INFO("[ServiceManager] AnswerVolume = %d\n", ins->m_volume);
		// add by renxs 2017.10.10 启动未完成时，不显示音量条
		if (!ins->isClientInitCompleted)
		{
			return;
		}
		if (ins->m_volumeEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_volumeEvent);
		}
		else
		{
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VolumeChangedNotify, data);
		}
		return;
	}


	/**
	*	@brief
	*		Mute状态变化通知
	*	@param [in] NULL
	*
	*	@return void
	*/
	void VoiceSvcEngine::MuteChangedNotify(uint32* para)
	{
		CQ_LOG_INFO("[ServiceManager] MuteChangedNotify\n");
		std::set<VoiceSvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((VoiceSvcEngineListener*)*iter)->onMuteChangedNotify(para);
		}
		return;
	}

	// 通知函数
	void MuteChangedNotify(void* para)
	{
		VoiceSvcEngine::getInstance()->MuteChangedNotify((uint32*)para);
		return;
	}

	/**
	*	@brief
	*		向Reciever注册的音量同期Callback函数
	*	@param [in] data
	*		音量
	*	@return
	*		void
	*/
	void AnswerMute(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AnswerMute NULL!\n");
			return;
		}
		VoiceSvcEngine* ins = VoiceSvcEngine::getInstance();
		ins->m_volume = *(uint32*)data;
		CQ_LOG_INFO("[ServiceManager] AnswerMute = %d\n", ins->m_volume);
		// add by renxs 2017.10.10 启动未完成时，不显示音量条
		if (!ins->isClientInitCompleted)
		{
			return;
		}
		uint32* pData = (uint32*)malloc(sizeof(uint32));
		if (pData == NULL)
		{
			return;
		}
		*pData = ins->m_volume;
		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)MuteChangedNotify, (uint32*)data);
		free(pData);
		pData = NULL;
		return;
	}

	/**
	*	@brief
	*		导航向车机系统设置音量
	*	@param [in] data
	*		音量
	*	@return
	*	 	void	   
	*/
	void VoiceSvcEngine::SetNaviVolume(uint32 vol)
	{
		if ( vol > 100)
		{
			CQ_LOG_ERROR("[ServiceManager] volume is error volume = %d!\n",vol);
			return;
		}

		CQ_LOG_INFO("[ServiceManager] VoiceSvcClient_SetNaviVolume vol = %d\n", vol);
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_SetVolume;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		if (paramData->data == NULL)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &vol, sizeof(uint32));
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		return;
	}


	/*========================requestAudioGuidancePlay============================*/
	/**
	*	@brief
	*		如果允许播放音频指令，则必须在播放音频指导之前调用此功能。
	*/
	BOOL VoiceSvcEngine::RequestAudioGuidancePlay()
	{
		CQ_LOG_INFO("[ServiceManager] RequestAudioGuidancePlay \n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_RequestAudioGuidancePlay;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return FALSE;
	}

	/*========================releaseAudioGuidancePlay============================*/
	/**
	*	@brief
	*		释放已经获得音频播放权限，在以下情况下必须调用此函数
	*        1) 诱导音频播放完成后.
	*        2) 诱导音频遇到错误.
	*        3) 诱导音频中断，如蓝牙手机铃声，CarPlay启动诱导等.
	*/
	void VoiceSvcEngine::ReleaseAudioGuidancePlay()
	{
		CQ_LOG_INFO("[ServiceManager] ReleaseAudioGuidancePlay \n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReleaseAudioGuidancePlay;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return;
	}

	void VoiceSvcEngine::setClientInitFlag(bool flg)
	{
		isClientInitCompleted = flg;
	}

	/*====================================德赛=======================================*/
}
