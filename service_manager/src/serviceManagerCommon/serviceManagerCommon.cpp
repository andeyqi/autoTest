/*�ṩengine��Client�ĳ�ʼ���ӿ�*/
#include "../stdafx_serviceManager.h"

//#ifdef MAPBAR_TOYOTA_262AVN_LINUX
//#include "serviceManagerLog/LitesTraceDrc.h"
//#endif

//#include "serviceManagerLog/LitesTraceDrc.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Addition.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Display.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Guidance.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_NaviSystemStateNotify.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Voice.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_VR.h"
#include "communication/serviceManagerOutput.h"
namespace ServiceManager{
	// add by liubo
	// delete by shenghao
	//extern uint32 flag_just_test;
	//extern BOOL flag_navi_init_state;
	//void serviceManagerNaviInit(void)
	//{
	//	CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, begin flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
	//	flag_navi_init_state = FALSE;
	//	AdditionSvcEngine::getInstance()->isNaviActivate();
	//	AdditionSvcEngine::getInstance()->getRunStatus();
	//	DisplaySvcEngine::getInstance()->ReqCheckILLState();
	//	AdditionSvcEngine::getInstance()->getWifiStatus();
	//	flag_navi_init_state = TRUE;
	//	CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, final flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
	//	return;
	//}

	/**
	@brief ServiceManager Clinet���ڻ�
		�ȳ�ʼ��Sender��Sender���Receiver�л�ȡIpcWrapper
	@return 		void
	@details
	*/
	void initServiceManagerClient()
	{
		CQ_LOG_INFO("[ServiceManager] initServiceManagerClient.\n");
		ServiceManager::AdditionSvcClient_Initialize();
		//ServiceManager::DisplaySvcClient_Initialize();
		ServiceManager::GuidanceServiceClient_Initialize();
		ServiceManager::VoiceSvcClient_Initialize();
		ServiceManager::NaviSystemStateNotifySvcClient_Initialize();
		ServiceManager::VRSvcClient_Initialize();
		return;
	}

	/**
	@brief ServiceManager Engine���ڻ�
		�ȳ�ʼ��Receiver��Sender��Receiver�л��ʼ��IpcWrapper
	@return 	void
	*/
	void initServiceManagerEngine()
	{
		// ��ʼ��LOG���TAG
//#ifdef MAPBAR_TOYOTA_262AVN_LINUX
//		fc_sys_InitLts(ID_MAPBAR);
//#endif
		
		CQ_LOG_INFO("[ServiceManager] initServiceManagerEngine.\n");
		// ��ʼ��ServiceManager Receiver��Sender
		NaviSysReceiver::getInstance();
		NaviSysSender::getInstance();
		// ��ʼ��ServiceManager Engine
		AdditionSvcEngine::getInstance()->startService();
		DisplaySvcEngine::getInstance()->startService();
		VoiceSvcEngine::getInstance()->startService();
		GuidanceSvcEngine::getInstance()->startService();
		ServiceManager::SystemNotify::getInstance();
		// ��ʼ��ServiceManger Client Display
		DisplaySvcClient_Initialize();
		// [TMEP]:add by liubo
		// delete by shenghao
		//serviceManagerNaviInit();
		return;
	}


	//delete by shenghao for exit timeout 20180711
	/**
	@brief ServiceManager Client Clean up
		����Clientģ��
	@return 	void
	*/
	//void deInitServiceManagerClient()
	//{
	//	CQ_LOG_INFO("[ServiceManager] deInitServiceManagerClient.\n");
	//	����stopsuc ֮ǰ��Ϊ��485�˳�ʱ����㷢�ó����軭��������������˳��ˣ�
	//	����û���軭����������262����Ҫ����ɾ����
	//	ServiceManager::NaviSystemStateNotifySvcClient_CleanUp();
	//	delete by shenghao for exit timeout 20180711
	//	ServiceManager::GuidanceServiceClient_CleanUp();
	//	ServiceManager::AdditionSvcClient_CleanUp();
	//	ServiceManager::DisplaySvcClient_CleanUp();
	//	ServiceManager::VoiceSvcClient_CleanUp();
	//	ServiceManager::VRSvcClient_CleanUp();
	//	return;
	//}

	/**
	@brief ServiceManager Engine Clean up
	����Clientģ��
	@return 	void
	*/
	void deInitServiceManagerEngine()
	{
		CQ_LOG_INFO("[ServiceManager] deInitServiceManagerEngine.\n");
		// �ж�ClearMemory��־�����ΪTRUE����������ɹ���Ϣ
		/*if (AdditionSvcEngine::getInstance()->getMemoryClearFlag())
		{
			SendData* paramData = (SendData*)malloc(sizeof(SendData));
			paramData->funcId = FuncID_Send_HistoryClear;
			paramData->data = (void*)NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			AdditionSvcEngine::getInstance()->setMemoryClearFlag(FALSE);
		}*/
		SystemNotify::releaseInstance();
		SystemNotify::getInstance()->sendACCOFFSuc();
		//NaviSysReceiver::getInstance()->releaseInstance();
		//delete by shenghao for exit timeout
		//GuidanceSvcEngine::releaseInstance();
		// delete by liubo
		//AdditionSvcEngine::getInstance()->stopService();
		//AdditionSvcEngine::releaseInstance();
		// delete by liubo
		//DisplaySvcEngine::getInstance()->stopService();
		//DisplaySvcEngine::releaseInstance();
		//VoiceSvcEngine::releaseInstance();
	
		return;
	}

	/**
	@brief �����˳��ɹ���֪ͨϵͳ
	����Clientģ��
	@return 	void
	*/
	// delete by liubo
	/*
	void naviExitCompleted()
	{
		CQ_LOG_INFO("[ServiceManager] naviAppExitComplete.\n");
		// ServiceManager::SystemNotify::getInstance()->naviExitComplete();
		deInitServiceManagerEngine();
		return;
	}
	*/

	/**
	@brief
	֪ͨ�������Խ���ǰ��̨�л�
	@return 	void
	*/
	void ServiceManager_ReadyForground()
	{
		DisplaySvcEngine::getInstance()->DisplaySvcEngine_ReadyForForground();
	}

	//log
	void ServiceManager_log_start()
	{
		ServiceManagerOutput::getInstance()->creatLogFile();
	}

	void ServiceManager_log_stop()
	{
		ServiceManagerOutput::getInstance()->closeLogFile();
	}
}