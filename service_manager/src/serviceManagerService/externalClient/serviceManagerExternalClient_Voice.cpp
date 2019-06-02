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
	// ��������
	void VoiceSvcClient_ReqStopEMG(void* data);
	void VoiceSvcClient_ReqVoiceStateCHK(void* data);
	int VoiceSvcClient_SoundArbiter_ForceStop(void* data);

	/**
	*	@brief
	*		VoiceSvc���ڻ�
	*	@return
	*		void
	*	@remarks
	*		��Recieveģ���Mbapiģ��ע��Callback����
	*/
	void VoiceSvcClient_Initialize()
	{

		//��Recieveģ�����Callback����ע��
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_reqStopVoiceEMG, VoiceSvcClient_ReqStopEMG);//������������Ҫ��
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_checkVoiceState, VoiceSvcClient_ReqVoiceStateCHK);//����״̬��ѯ

		//����ʱ��ȡ���������������͸�����
		int32 volume = MbApi::SystemSound_getVolume();
		CQ_LOG_INFO("[ServiceManager]Func VoiceSvcClient_Initialize volume = %d.\n", volume);
		VoiceSvcClient_SetNaviVolume(&volume);

		return;
	}

	
	/**
	*	@brief
	*		VoiceSvc���
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
	
	/*========================2.2 �����յ��� ������������============================*/
	/**
	*	@brief
	*		���󵼺�������������
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
	*		�������󵼺�������������
	*	@return
	*	 	void
	*/
	void VoiceSvcClient_ReqStopEMG(void* data)
	{
		// delete by liubo
		// Receiver�н����ͷ� 
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
	/*========================2.2 �����յ��� ������������============================*/
	//
	//
	//
	/*========================2.2 �����յ��� ������ѯ����״̬============================*/
	/**
	*	@brief
	*		��ѯ����״̬
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
	*		�������󵼺���ѯ����״̬
	*	@return
	*	 	void
	*/
	void VoiceSvcClient_ReqVoiceStateCHK(void* data)
	{
		// delete by liubo
		// Receiver�н����ͷ�
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
	/*========================2.2 �����յ��� ������ѯ����״̬============================*/

	
	/**
	*	@brief
	*		�����򳵻��������������仯
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
