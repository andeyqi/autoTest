#include "stdafx_serviceManagerEngine.h"
#include <string>
//#include <sstream>

namespace ServiceManager
{
	//=====================================================//
	SystemNotify* SystemNotify::m_instance = NULL;
	//=====================================================//
	// 函数声明
	void AnswerNaviExitSucess(void* para);
	void sendNaviStopSuc(void);

	/**
	*	@brief
	*		实例化
	*/
	SystemNotify* SystemNotify::getInstance()
	{
		if (NULL == m_instance)
		{
			m_instance = new SystemNotify();
			return m_instance;
		}
		return m_instance;
	}

	/**
	*	@brief
	*		释放对象
	*/
	void SystemNotify::releaseInstance()
	{
		CQ_LOG_INFO("[ServiceManager] SystemNotify::releaseInstance.\n");
		// add by liubo
		// 将ExternalClient_NaviSystemStateNotify中发送stop success移动到这里
		//delete by shenghao for exit timeout 20180711
		//if (!m_instance->isNeedSendMsg){
			sendNaviStopSuc();
		//}
		
		//if (NULL != m_instance)
		//{
		//	delete m_instance;
		//	m_instance = NULL;
		//}
		return;
	}

	/**
	*	@brief
	*		导航退出时，发送stop success
	*/
	void sendNaviStopSuc(void)
	{
		CQ_LOG_INFO("[ServiceManager] sendStopSuc FuncID_Send_NaviStopSuc\n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_NaviStopSuc;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//SystemNotify * ins = SystemNotify::getInstance();

		//ins->m_stopSucessEvent = Mapbar_createEvent(FALSE, FALSE);
		//NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		////MapbarWaitState state = Mapbar_waitWithTimeout(ins->m_stopSucessEvent, 1000);//等待1秒
		//MapbarWaitState state = Mapbar_waitEventWithTimeout(ins->m_stopSucessEvent, 1000);//等待1秒
		//if (state == MapbarWaitState_ok)
		//{
		//	Mapbar_destroyEvent(ins->m_stopSucessEvent);
		//	ins->m_stopSucessEvent = invalidHandle;
		//	return;
		//}

		//Mapbar_destroyEvent(ins->m_stopSucessEvent);
		//ins->m_stopSucessEvent = invalidHandle;
		return ;
	}

	void SystemNotify::sendACCOFFSuc(void)
	{
		// 发送数据保存成功消息
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_NaviACCOFFSuc;
		paramData->data = (void*)NULL;
		//NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		m_exitCompleteEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_exitCompleteEvent, 1000);//等待1秒
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_exitCompleteEvent, 1000);//等待1秒

		Mapbar_destroyEvent(m_exitCompleteEvent);
		m_exitCompleteEvent = invalidHandle;

		//UnixTimestamp time = Mapbar_getCurrentTimeMillis();
		//uint32 hours = (time / 1000 / 60 / 60 + 8) % 24;
		//uint32 minutes = time / 1000 / 60 % 60;
		//uint32 seconds = time / 1000 % 60;
		//uint32 milliseconds = time % 1000;
		//printf("DATE: %02d%02d%02d%02d %s \n", hours, minutes, seconds, milliseconds, "ACCOFF finish!\n");

		return;
	}

	//BOOL SystemNotify::naviExitComplete()
	//{
	//	CQ_LOG_INFO("[ServiceManager] NaviSystemStateNotifySvcEngine_NaviExitCompleteMsg.\n");
	//	ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
	//	paramData->funcId = ServiceManager::FuncID_Send_NaviACCOFFSuc;
	//	paramData->data = (void*)NULL;

	//	m_exitCompleteEvent = Mapbar_createEvent(FALSE, FALSE);
	//	NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
	//	//MapbarWaitState state = Mapbar_waitWithTimeout(m_exitCompleteEvent, 1000);//等待1秒
	//	MapbarWaitState state = Mapbar_waitEventWithTimeout(m_exitCompleteEvent, 1000);//等待1秒
	//	if (state == MapbarWaitState_ok)
	//	{
	//		Mapbar_destroyEvent(m_exitCompleteEvent);
	//		m_exitCompleteEvent = invalidHandle;
	//		return TRUE;
	//	}

	//	Mapbar_destroyEvent(m_exitCompleteEvent);
	//	m_exitCompleteEvent = invalidHandle;
	//	return FALSE;
	//}

	void AnswerNaviExitSucess(void* para)
	{
		CQ_LOG_INFO("[ServiceManager] AnswerNaviExitSucess.\n");
		SystemNotify * ins = SystemNotify::getInstance();

		UNUSED_VAR(para);
		if (ins->m_exitCompleteEvent != invalidHandle)
			Mapbar_setEvent(ins->m_exitCompleteEvent);
		return;
	}


	//delete by shenghao for exit timeout
	//void AnswerNaviStopSucess(void* para)
	//{
	//	CQ_LOG_INFO("[ServiceManager] AnswerNaviStopSucess.\n");
	//	SystemNotify * ins = SystemNotify::getInstance();

	//	// delete by liubo
	//	// Receiver中进行释放
	//	UNUSED_VAR(para);
	//	/*
	//	if (para != NULL)
	//	{
	//		free(para);
	//		para = NULL;
	//	}
	//	*/

	//	if (ins->m_stopSucessEvent != invalidHandle)
	//		Mapbar_setEvent(ins->m_stopSucessEvent);

	//	free(para);
	//	return;
	//}
	

	SystemNotify::SystemNotify()
		:m_exitCompleteEvent(invalidHandle)
		//delete by shenghao for exit timeout 20180711
		//, isNeedSendMsg(false)
	{
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerSendAccOffSucess, AnswerNaviExitSucess);
		//NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_AnswerSendStopSucess, AnswerNaviStopSucess);
	}

	SystemNotify::~SystemNotify()
	{
		
	}

	//delete by shenghao for exit timeout 20180711
	//void SystemNotify::setSendStopMsgFlag(bool isSend)
	//{
	//	m_instance->isNeedSendMsg = isSend;
	//}
}