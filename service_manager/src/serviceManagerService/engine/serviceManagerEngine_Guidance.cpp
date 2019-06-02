#include "stdafx_serviceManagerEngine.h"
//#include "serviceManagerEngine_Guidance.h"
#include <string>
//#include <sstream>

namespace ServiceManager
{
	void AnswerCarPlayNaviStatus(void* data);
	
	GuidanceSvcEngine* GuidanceSvcEngine::m_instance = NULL;

	/**
	*	@brief
	*		ʵ����
	*/
	GuidanceSvcEngine* GuidanceSvcEngine::getInstance()
	{
		if (NULL == m_instance)
		{
			m_instance = new GuidanceSvcEngine();
			m_instance->startService();
		}
		return m_instance;
	}

	//delete by shenghao for exit timeout
	//void GuidanceSvcEngine::releaseInstance()
	//{
	//	CQ_LOG_INFO("[ServiceManager] GuidanceSvcEngine::releaseInstance.\n");
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
	GuidanceSvcEngine::GuidanceSvcEngine()
		:m_isCarPlayNaviActive(FALSE)
	{
		//ͬ��Callback����ע��
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_IsCarPlayNaviActive, AnswerCarPlayNaviStatus);
	}

	/**
	*	@brief
	*		��������
	*/
	GuidanceSvcEngine::~GuidanceSvcEngine()
	{
	}


	/**
	*	@brief
	*		��ʼ��
	*/
	bool GuidanceSvcEngine::InitInstance()
	{
		return true;
	}

	/**
	*	@brief
	*		cleanup
	*/
	//delete by shenghao for exit timeout
	//bool GuidanceSvcEngine::ExitInstance()
	//{
	//	return true;
	//}

	/**
	*	@brief
	*		֪ͨ����ע��Listener
	*	@param
	*/
	void GuidanceSvcEngine::GuidanceSvcEngine_CarPlayActiveChangedNotify(BOOL para)
	{
		CQ_LOG_INFO("[ServiceManager] GuidanceSvcEngine_CarPlayActiveChangedNotify.\n");
		std::set<GuidanceSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((GuidanceSvcEngineListener*)*iter)->onCarPlayActiveChangeNotify(para);

		return;
	}

	/*================================����=================================*/

	/*========================Desai isCarPlayNaviActive============================*/
	/**
	*	@brief
	*		���CarPlay�Ƿ��Ѿ��ƶ���·�ߡ�
	*		�˺��������������Ǿ�Ӧ�ñ�����
	*		����������ؽ����true���������ܷ����κ��յ�����
	*	@param
	*/
	BOOL GuidanceSvcEngine::IsCarPlayNaviActive()
	{
		CQ_LOG_INFO("[ServiceManager] IsCarPlayNaviActive.\n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_IsCarPlayNaviActive;
		paramData->data = (void*)NULL;

		m_carPlayNaviActiveEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_carPlayNaviActiveEvent, 1000);//�ȴ�1��
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_carPlayNaviActiveEvent, 1000);//�ȴ�1��
		if (state == MapbarWaitState_ok)
		{

			Mapbar_destroyEvent(m_carPlayNaviActiveEvent);
			m_carPlayNaviActiveEvent = invalidHandle;
			return m_isCarPlayNaviActive;
		}

		Mapbar_destroyEvent(m_carPlayNaviActiveEvent);
		m_carPlayNaviActiveEvent = invalidHandle;

		return FALSE;
	}

	/**
	*	@brief
	*		��Client֪ͨCarPlay״̬�仯
	*	@param [in] bool
	*		�Ƿ�Active
	*	@return
	*		void
	*/
	void CarPlayActiveChangedNotify(void* para)
	{
		CQ_LOG_INFO("[ServiceManager] CarPlayActiveChangedNotify.\n");
		UNUSED_VAR(para);
		GuidanceSvcEngine::getInstance()->GuidanceSvcEngine_CarPlayActiveChangedNotify(GuidanceSvcEngine::getInstance()->m_isCarPlayNaviActive);
		return;
	}

	/**
	*	@brief
	*		��Recieverע���ж�CarPlay Navi�Ƿ�Active��Callback����,
	*	@param [in] data
	*		�Ƿ�Active
	*	@return
	*		void
	*/
	void AnswerCarPlayNaviStatus(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] AnswerCarPlayNaviStatus.\n");
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AnswerCarPlayNaviStatus data is NULL!\n");
			return;
		}
		GuidanceSvcEngine * ins = GuidanceSvcEngine::getInstance();
		ins->m_isCarPlayNaviActive = *(bool*)data;
		if (ins->m_carPlayNaviActiveEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_carPlayNaviActiveEvent);
		}
		else
		{
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)CarPlayActiveChangedNotify, NULL);
		}
		return;
	}
	/*====================================����=======================================*/
}
