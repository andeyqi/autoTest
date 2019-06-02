#include "stdafx_serviceManagerEngine.h"
//#include "serviceManagerEngine_Voice.h"
#include <string>
//#include <sstream>

namespace ServiceManager
{
	VoiceSvcEngine* VoiceSvcEngine::m_instance = NULL;

	// ��������
	void AnswerVoiceStart(void* data);
	void AnswerVolume(void* data);
	void AnswerMute(void* data);

	/**
	*	@brief
	*		ʵ����
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
	*		�ͷŶ���
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
	*		���캯��
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
	*		��������
	*/
	VoiceSvcEngine::~VoiceSvcEngine(){};

	/**
	*	@brief
	*		��ʼ��
	*/
	bool VoiceSvcEngine::InitInstance(void)
	{
		//ͬ��Callback����ע��
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerVoiceStart, AnswerVoiceStart);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerVolume, AnswerVolume);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerMute, AnswerMute);

		return true;
	}

	/*========================2.2 �����յ��� ������ʼ����============================*/
	/**
	*	@brief
	*		�������󳵻���ʼ����(ͬ��Ҫ��)
	*	@return
	*	 	BOOL
	*/
	BOOL VoiceSvcEngine::ReqVoiceStart()
	{
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
/* qihc add start*/
		/* �����ں�̨״̬�ҵ�ǰ״̬ģ�⵼��,���ߵ�����ʱ */
		if(ServiceManager::DisplaySvcEngine::getInstance()->GetNaviRunStatus() == true)
		{
			NavigationStatus status;
			status = NaviSession_getNaviStatus();
			if(!((status == NavigationStatus_inNavi && NaviSession_isNaviPaused() == FALSE) ||\
				NaviSession_getSimulationState() == TRUE))	//��ȡģ�⵼��״̬�ӿڱ�� NaviSession_isInSimulation() -> NaviSession_getSimulationState()
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
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_reqVoiceStartEvent, 1000);//�ȴ�1��
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_reqVoiceStartEvent, 1000);//�ȴ�1��
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
	*		��Recieverע��Ŀ�ʼ��������Ҫ��ͬ��Callback����
	*	@return
	*		void
	*/
	void AnswerVoiceStart(void* data)
	{
		// delete by liubo
		// Receiver �н����ͷ�
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
	/*========================2.2 �����յ��� ������ʼ����============================*/
	//
	//
	//
	/*========================2.2 �����յ��� ������������============================*/
	/**
	*	@brief
	*		�������󳵻���������(ͬ��Ҫ��)
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
	/*========================2.2 �����յ��� ������������============================*/
	//
	//
	//
	/*========================2.2 �����յ��� BEEP����Ҫ��============================*/
	/**
	*	@brief
	*		�������󳵻�Ҫ��BEEP����
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
	/*========================2.2 �����յ��� BEEP����Ҫ��============================*/

	/*====================================����=======================================*/
	/**
	*	@brief
	*		�������󳵻��ṩ����ֵ
	*	@return
	*	 	uint32 ����ֵ����0~100֮��ʱΪ��Чֵ��
	*			   ����ֵ����100�����Ϊ��Чֵ����Ҫ�ȴ��첽callback
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
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_volumeEvent, 1000);//�ȴ�1��
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_volumeEvent, 1000);//�ȴ�1��
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
	*		Volume״̬�仯֪ͨ
	*	@param [in] uint32
	*		para ����ֵ
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

	// ֪ͨ����
	void VolumeChangedNotify(void* para)
	{
		CQ_LOG_INFO("[ServiceManager] VolumeChangedNotify\n");
		VoiceSvcEngine::getInstance()->VolumeChangedNotify((uint32*)para);
		return;
	}

	/**
	*	@brief
	*		��Recieverע�������ͬ��Callback����
	*	@param [in] data
	*		����
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
		// add by renxs 2017.10.10 ����δ���ʱ������ʾ������
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
	*		Mute״̬�仯֪ͨ
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

	// ֪ͨ����
	void MuteChangedNotify(void* para)
	{
		VoiceSvcEngine::getInstance()->MuteChangedNotify((uint32*)para);
		return;
	}

	/**
	*	@brief
	*		��Recieverע�������ͬ��Callback����
	*	@param [in] data
	*		����
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
		// add by renxs 2017.10.10 ����δ���ʱ������ʾ������
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
	*		�����򳵻�ϵͳ��������
	*	@param [in] data
	*		����
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
	*		�����������Ƶָ�������ڲ�����Ƶָ��֮ǰ���ô˹��ܡ�
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
	*		�ͷ��Ѿ������Ƶ����Ȩ�ޣ�����������±�����ô˺���
	*        1) �յ���Ƶ������ɺ�.
	*        2) �յ���Ƶ��������.
	*        3) �յ���Ƶ�жϣ��������ֻ�������CarPlay�����յ���.
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

	/*====================================����=======================================*/
}
