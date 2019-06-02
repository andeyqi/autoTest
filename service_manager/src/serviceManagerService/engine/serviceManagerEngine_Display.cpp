#include "stdafx_serviceManagerEngine.h"
#include "service_manager/serviceManagerEngine_Display.h"
//#include "serviceManagerOutput.h"

namespace ServiceManager
{
	//=====================================================//
	DisplaySvcEngine* DisplaySvcEngine::m_instance = NULL;
	// [TEMP]: add by liubo
	// delete by shenghao
	//extern uint32 flag_just_test;  // bit 0 is uuid; bit 1 is PKB; bit 3 is ILL; bit 4 is wifi
	//extern BOOL flag_navi_init_state;
	//extern std::mutex lck;
	//extern std::unique_lock<std::mutex> lck(mtx);
	//=====================================================//
	
	void DisplaySvcEngine_SgnShowOtherFormSuc(void* data);
	void DisplaySvcEngine_SgnHideOtherFormSuc(void* data);
	void DisplaySvcEngine_SgnILLState(void* data);
	void DisplaySvcEngine_AnswerLanguageID(void* data);
	void DisplaySvcEngine_AnswerBacklightMode(void* data);
	void AnswerShowMusicWidget(void* data);
	void AnswerHideMusicWidget(void* data);
	void AnswerSysHmiTime(void* data);
	void DisplaySvcEngine_LanguageSwitchChanged(void* data);
	
	void NotifyDayNightLinkAge(void* data);
	void NotifyTouchStatus(void* data);
	bool DisplaySvcEngine::InitInstance()
	{
		//ͬ��Callback����ע��
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_showOtherFormSuc, DisplaySvcEngine_SgnShowOtherFormSuc);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_hideOtherFormSuc, DisplaySvcEngine_SgnHideOtherFormSuc);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_sgnILLStateChanged, DisplaySvcEngine_SgnILLState);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_GetLanguageID, DisplaySvcEngine_AnswerLanguageID);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_getBackLight, DisplaySvcEngine_AnswerBacklightMode);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_SysHmiTimeChanged, AnswerSysHmiTime);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_SysDayNightLinkAge, NotifyDayNightLinkAge);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_TouchStatus, NotifyTouchStatus);
		//�첽Callback����ע��
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_MusicWidgeShowChanged, AnswerShowMusicWidget);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_MusicWidgeHideChanged, AnswerHideMusicWidget);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_LanguageSwitchChanged, DisplaySvcEngine_LanguageSwitchChanged);
		
		return true;
	}

	//delete by shenghao for exit timeout
	//bool DisplaySvcEngine::ExitInstance()
	//{
	//	return true;
	//}

		
	/**
	*	@brief
	*		���캯��		
	*/
	DisplaySvcEngine::DisplaySvcEngine()
		: m_illStatus(EN_DISPLAYSVCENGINE_ILL_STATE_INVALID)
		, m_languageId(-1)
		, m_backLight(NULL)
		, m_widgetStatus(EN_DISPLAYSVCENGINE_WIDGET_INVALID)
		, m_displaySvcShowOtherEvent(invalidHandle)
		, m_displaySvcHideOtherEvent(invalidHandle)
		, m_displaySvcEventILL(invalidHandle)
		, m_displaySvcBackLightEvent(invalidHandle)
		, m_displaySvcLanguageIdEvent(invalidHandle)
		, m_displaySvcDayNightLinkAgeEvent(invalidHandle)
		, m_timeDValue(0)
		, isBack(true)
		, m_timeInfo(NULL)
		, m_timeFormat(EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN)
		, m_tickCount(0)
	{
		memset(&m_dayNightLinkAgeData,0,sizeof(m_dayNightLinkAgeData));
		m_touchInfo.status = FALSE;
		m_touchInfo.tickCount = 0;
	}
	
	
	/**
	*	@brief
	*		��������
	*/
	DisplaySvcEngine::~DisplaySvcEngine(){
		free(m_backLight);
		m_backLight = NULL;
	}
	
	
	/**
	*	@brief
	*		ʵ����
	*/
	DisplaySvcEngine* DisplaySvcEngine::getInstance()
	{
		if (NULL == m_instance)
		{
			m_instance = new DisplaySvcEngine();
			if (NULL != m_instance)
			{
				m_instance->startService();
			}
			return m_instance;
		}
		return m_instance;
	}
	
	
	/**
	*	@brief
	*		�ͷŶ���
	*/
	//delete by shenghao for exit timeout
	//void DisplaySvcEngine::releaseInstance()
	//{
	//	CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine::releaseInstance.\n");
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
	*		ILL״̬״̬�仯֪ͨ
	*	@param [in] DisplaySvcEngineRunStatus
	*		data ״̬��Ϣ
	*	@return void
	*/
	void DisplaySvcEngine::illStateChangedNotify(EN_DISPLAYSVCENGINE_ILL_STATE data)
	{
		std::set<DisplaySvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onILLStateChangedNotify((EN_DISPLAYSVCENGINE_ILL_STATE)data);
		}

		return;
	}
	
	/**
	*	@brief
	*		����״̬�仯֪ͨ
	*	@param [in] 
	*		data ״̬��Ϣ
	*	@return void
	*/
	void DisplaySvcEngine::backlightModeChangNotify(DeSai_BackLight data)
	{
		CQ_LOG_INFO("[ServiceManager] backlightModeChangNotify\n");
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onBacklightModeChangNotify(data);
		}
		return;
	}


	/**
	*	@brief
	*		����ID״̬�仯֪ͨ
	*	@param [in] int32
	*		data ����ID
	*	@return void
	*/
	void DisplaySvcEngine::languageIdChangNotify(int32 data)
	{
		CQ_LOG_INFO("[ServiceManager] languageIdChangNotify\n");
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onLanguageIdChangNotify(data);
		}
		return;
	}
	
	/**
	*	@brief
	*		����MusicWidget��ʾ���֪ͨ
	*	@param [in] bool
	*		data �Ƿ�ɹ�
	*	@return void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_ShowMusicWidgetNotify(bool data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ShowMusicWidgetNotify\n");
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onMusicWidgetShowNotify(data);
			// ֪ͨmapdal
			((DisplaySvcEngineListener*)*iter)->onMusicWidgetIsShowNotify(data);
		}
		return;
	}
	
	/**
	*	@brief
	*		����MusicWidget��ʾ���֪ͨ
	*	@param [in] bool
	*		data �Ƿ�ɹ�
	*	@return void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_HideMusicWidgetNotify(bool data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ShowMusicWidgetNotify\n");
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onMusicWidgetHideNotify(data);
			// ֪ͨmapdal
			((DisplaySvcEngineListener*)*iter)->onMusicWidgetIsShowNotify(!data);
		}
		return;
	}

	/*========================2.3 ����Ǩ�� ����������ʾ============================*/
	/**
	*	@brief
	*		�������󳵻���ʾ��������(ͬ��Ҫ��)
	*	@return
	*	 	BOOL
	*/
	BOOL DisplaySvcEngine::DisplaySvcEngine_ReqShowOtherForm()
	{
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqShowOtherForm;
		paramData->data = (void*)NULL;

		m_displaySvcShowOtherEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_displaySvcShowOtherEvent, 1000);//�ȴ�1��
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcShowOtherEvent, 1000);//�ȴ�1��
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_displaySvcShowOtherEvent);
			m_displaySvcShowOtherEvent = invalidHandle;

			return TRUE;
		}

		Mapbar_destroyEvent(m_displaySvcShowOtherEvent);
		m_displaySvcShowOtherEvent = invalidHandle;

		return FALSE;
	}

	/**
	*	@brief
	*		��Recieverע���Ҫ������������ʾͬ��Callback����
	*	@return
	*		void
	*/
	void DisplaySvcEngine_SgnShowOtherFormSuc(void* data)
	{
		DisplaySvcEngine * ins = DisplaySvcEngine::getInstance();

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

		if (ins->m_displaySvcShowOtherEvent != invalidHandle)
			Mapbar_setEvent(ins->m_displaySvcShowOtherEvent);

		return;
	}
	/*========================2.3 ����Ǩ�� ����������ʾ============================*/
	//
	//
	//
	/*========================2.3 ����Ǩ�� ������������============================*/
	/**
	*	@brief
	*		�������󳵻�������������(ͬ��Ҫ��)
	*	@return
	*	 	void
	*/
	BOOL DisplaySvcEngine::DisplaySvcEngine_ReqHideOtherForm()
	{
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqHideOtherForm;
		paramData->data = (void*)NULL;

		m_displaySvcHideOtherEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_displaySvcHideOtherEvent, 1000);//�ȴ�1��
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcHideOtherEvent, 1000);//�ȴ�1��
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_displaySvcHideOtherEvent);
			m_displaySvcHideOtherEvent = invalidHandle;

			return TRUE;
		}

		Mapbar_destroyEvent(m_displaySvcHideOtherEvent);
		m_displaySvcHideOtherEvent = invalidHandle;

		return FALSE;
	}
	
	
	/**
	*	@brief
	*		��Recieverע���Ҫ��������������ͬ��Callback����
	*	@return
	*		void
	*/
	void DisplaySvcEngine_SgnHideOtherFormSuc(void* data)
	{
		DisplaySvcEngine * ins = DisplaySvcEngine::getInstance();

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

		if (ins->m_displaySvcHideOtherEvent != invalidHandle)
			Mapbar_setEvent(ins->m_displaySvcHideOtherEvent);

		return;
	}
	/*========================2.3 ����Ǩ�� ������������============================*/
	//
	//
	//
	/*========================2.4 ILL(����)�ź�============================*/
	/**
	*	@brief
	*		�������󳵻��ṩILL(����)״̬(ͬ��Ҫ��)
	*	@return
	*	 	EN_DISPLAYSVCENGINE_ILL_STATE
	*/
	EN_DISPLAYSVCENGINE_ILL_STATE DisplaySvcEngine::ReqCheckILLState()
	{
		if (m_illStatus != EN_DISPLAYSVCENGINE_ILL_STATE_INVALID)
		{
			return m_illStatus;
		}
		else
		{
			// just for test
			// delete by shenghao
			//CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, before ill wait flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
			//flag_just_test |= 4; // wait���߳�, |0100
			//CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, after  ill wait flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);

			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			if (paramData == NULL)
			{
				return EN_DISPLAYSVCENGINE_ILL_STATE_OFF;
			}
			paramData->funcId = ServiceManager::FuncID_Send_ReqCheckILLState;
			paramData->data = (void*)NULL;

			m_displaySvcEventILL = Mapbar_createEvent(FALSE, FALSE);
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			
			//if (lck.try_lock())
			//{
			//	CQ_LOG_INFO("[servicemanager]. ReqCheckILLState lck.try_lock() sucess\n");
			//	printf("ServiceManager ReqCheckILLState lck.try_lock() sucess\n");
			//}
			//else
			//{
			//	CQ_LOG_INFO("[servicemanager]. ReqCheckILLState lck.try_lock() failed\n");
			//	printf("ServiceManager ReqCheckILLState lck.try_lock() failed\n");
			//}

			//MapbarWaitState state = Mapbar_waitWithTimeout(m_displaySvcEventILL, 1000);//��һ�Σ��ȴ�1��
			MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcEventILL, 1000);//�ȴ�1��
			if (state == MapbarWaitState_timeout)
			{
				ServiceManager::SendData* paramData_1 = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				if (paramData_1 == NULL)
				{
					return EN_DISPLAYSVCENGINE_ILL_STATE_OFF;
				}
				paramData_1->funcId = ServiceManager::FuncID_Send_ReqCheckILLState;
				paramData_1->data = (void*)NULL;

				NaviSysSender::getInstance()->sendMsg(paramData_1, LOW_PRIO);
				//state = Mapbar_waitWithTimeout(m_displaySvcEventILL, 1000);//�ڶ��Σ��ȴ�1��
				state = Mapbar_waitEventWithTimeout(m_displaySvcEventILL, 1000);//�ȴ�1��
				if (state == MapbarWaitState_timeout)
				{	
					ServiceManager::SendData* paramData_2 = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
					if (paramData_2 == NULL)
					{
						return EN_DISPLAYSVCENGINE_ILL_STATE_OFF;
					}
					paramData_2->funcId = ServiceManager::FuncID_Send_ReqCheckILLState;
					paramData_2->data = (void*)NULL;

					NaviSysSender::getInstance()->sendMsg(paramData_2, LOW_PRIO);
					//state = Mapbar_waitWithTimeout(m_displaySvcEventILL, 1000);//�����Σ��ȴ�1��
					state = Mapbar_waitEventWithTimeout(m_displaySvcEventILL, 1000);//�ȴ�1��
				}
			}
			
			//lck.unlock();
			//printf("ServiceManager ReqCheckILLState lck.unlock()\n");
			if (state == MapbarWaitState_ok)
			{
				Mapbar_destroyEvent(m_displaySvcEventILL);
				m_displaySvcEventILL = invalidHandle;
				return m_illStatus;
			}		

			// just for test
			// delete by shenghao
			//CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, before ill release flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
			//flag_just_test &= 11; // �ͷ����߳�,&1011
			//CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, after  ill release flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);

			/*����retry��ʧ��ʱ��ILL����OFF�趨*/
			Mapbar_destroyEvent(m_displaySvcEventILL);
			m_displaySvcEventILL = invalidHandle;
			return EN_DISPLAYSVCENGINE_ILL_STATE_OFF;

		}
	}


	// ֪ͨ����
	void illStateChangedNotify(void* param)
	{
		DisplaySvcEngine::getInstance()->illStateChangedNotify(*(EN_DISPLAYSVCENGINE_ILL_STATE*)param);

		return;
	}

	
	/**
	*	@brief
	*		��Recieverע���ILL(����)Stateͬ��Callback����
	*	@param [in] data
	*		ILL State
	*	@return
	*		void
	*/
	void DisplaySvcEngine_SgnILLState(void* data)
	{
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();

		if (data != NULL)
		{
			BOOL state = *(BOOL*)data;
			if(state)
			{
				ins->m_illStatus = EN_DISPLAYSVCENGINE_ILL_STATE_ON;
			}
			else
			{
				ins->m_illStatus = EN_DISPLAYSVCENGINE_ILL_STATE_OFF;
			}
			
			//�жϵ�ǰ�Ƿ�������ILL״̬��ѯ
			if (ins->m_displaySvcEventILL != invalidHandle)
			{
				Mapbar_setEvent(ins->m_displaySvcEventILL);
			}
			else
			{
				// [TMEP]: add by liubo
				// ��������������
				/*
				if (FALSE == flag_navi_init_state)
				{
					CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, ill changed flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
					CQ_LOG_INFO("[ServiceManager]function:%s, line:%d, ill judge   flag:%d.\n", __FUNCTION__, __LINE__, ((flag_navi_init_state >> 1) & 1));
					// �ж��Ƿ����Լ���Ҫ��
					if (0 == ((flag_just_test >> 2) & 1))
					{
						return;
					}
				}
				*/
				//if (lck.try_lock())
				//{
					CQ_LOG_INFO("[ServiceManager]. DisplaySvcEngine_SgnILLState lck.try_lock() sucess\n");
					Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)illStateChangedNotify, (void*)&ins->m_illStatus);
					ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
					if (paramData == NULL)
					{
						return;
					}
					paramData->funcId = ServiceManager::FuncID_Send_RepILLStateChanged;
					paramData->data = NULL;
					NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				//}
				/*else
				{
					CQ_LOG_INFO("[ServiceManager]. DisplaySvcEngine_SgnILLState lck.try_lock() failed\n");
					printf("ServiceManager DisplaySvcEngine_SgnILLState lck.try_lock() failed\n");
					return;
				}*/

			}

			// del by liubo
			// Receiver �н����ͷ�
			//free(data);
			//data = NULL;
		}

		return;
	}
	/*========================2.4 ILL(����)�ź�============================*/
	//
	//
	//
	/*========================Background Switch============================*/
	/**
	*	@brief
	*		ǰ��̨�л�
	*	@param [in] void
	*	@return void
	*/
	void DisplaySvcEngine::ReqBackgroundSwitch(void)
	{
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = FuncID_Send_BgSwt;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}

	/**
	*	@brief
	*		��¼����ǰ��̨״̬
	*	@param [in] void
	*	@return void
	*/
	bool DisplaySvcEngine:: GetNaviRunStatus(void)
	{
		return isBack;
	}
	/**
	*	@brief
	*		��ȡ����ǰ��̨״̬
	*	@param [in] void
	*	@return void
	*/
	void DisplaySvcEngine:: SetNaviRunStatus(bool status)
	{
		isBack = status;
	}
	
	
	/*========================Background Switch============================*/
	//
	//
	//
	///*======================== 3.19 ��Ӣ���л� ============================*/
	///*
	//*	@brief
	//*		����Receiveע���Callback����
	//*	@return
	//*	 	void
	//*/
	//void DisplaySvcEngine_LanguageSwitchChanged(void* data)
	//{
	//	printf("%d", data);
	//	DisplaySvcEngine * ins = DisplaySvcEngine::getInstance();
	//	ins->m_languageShowStatus = data;
	//	Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)LanguageSwitchNotify, (void *)ins->m_languageShowStatus);

	//	return;
	//}

	////֪ͨ����
	//void LanguageSwitchNotify(void *data)
	//{
	//	DisplaySvcEngine::getInstance()->DisplaySvcEngine_LanguageSwitchNotify(data);

	//	return;
	//}

	////֪ͨ����
	//void DisplaySvcEngine::DisplaySvcEngine_LanguageSwitchNotify(void* data)
	//{
	//	std::set<DisplaySvcEngineListener*>::iterator iter;

	//	for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
	//	{
	//		((DisplaySvcEngineListener*)*iter)->onLanguageSwitchNotify((uint32*) data);
	//	}

	//	return;
	//}

	//void DisplaySvcEngine::DisplaySvcEngine_RetLanguageSwitch(bool data)
	//{
	//	if (data == TRUE)
	//	{
	//		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
	//		paramData->funcId = ServiceManager::FuncID_Send_LanguageSwitchSuccess;
	//		paramData->data = NULL;
	//		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
	//		return;
	//	}
	//	else
	//	{
	//		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
	//		paramData->funcId = ServiceManager::FuncID_Send_LanguageSwitchFailure;
	//		paramData->data = NULL;
	//		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
	//		return;
	//	}
	//}


	///*======================== 3.19 ��Ӣ���л� ============================*/
	//
	//
	//
	/*===============================����=================================*/

	/*========================Desai backLight============================*/
	/**
	*	@brief
	*		�������󳵻��ṩ������Ϣ
	*	@return
	*	 	BkLightMode BLM_MANUAL��BLM_AUTO
	*		BkLightType BLT_DAY��BLT_NIGHT

	*/
	DeSai_BackLight DisplaySvcEngine::GetBacklightMode()
	{
		CQ_LOG_INFO("[ServiceManager] GetBacklightMode\n");
		DeSai_BackLight backLight;
		backLight.mode = SM_BLM_AUTO;
		backLight.type = SM_BLT_DAY;
		backLight.level = 0;
		
		//if (m_backLight != NULL)
		//{
		//	return *m_backLight;
		//}
		//else
		//{
		//	m_backLight = (DeSai_BackLight*)malloc(sizeof(DeSai_BackLight));
		//}

		//if (m_backLight != NULL)
		//{
		//	return backLight;
		//}

		//ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		//if (paramData == NULL)
		//{
		//	free(m_backLight);
		//	m_backLight = NULL;
		//	return backLight;
		//}
		//paramData->funcId = ServiceManager::FuncID_Send_GetBackLight;
		//paramData->data = (void*)NULL;
		//
		//m_displaySvcBackLightEvent = Mapbar_createEvent(FALSE, FALSE);
		//NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		////MapbarWaitState state = Mapbar_waitWithTimeout(m_displaySvcBackLightEvent, 1000);//�ȴ�1��
		//MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcBackLightEvent, 1000);//�ȴ�1��
		//if (state == MapbarWaitState_ok)
		//{
		//	Mapbar_destroyEvent(m_displaySvcBackLightEvent);
		//	m_displaySvcBackLightEvent = invalidHandle;
		//	return backLight;
		//}

		//Mapbar_destroyEvent(m_displaySvcBackLightEvent);
		//m_displaySvcBackLightEvent = invalidHandle;
		
		return backLight;
	}

	void DisplaySvcEngine_BacklightModeChangeNotify(void* pData)
	{
		UNUSED_VAR(pData);
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_BacklightModeChangeNotify\n");
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		ins->backlightModeChangNotify(*(ins->m_backLight));
		return;
	}
	
	/**
	*	@brief
	*		��Recieverע��ı���ͬ��Callback����
	*	@param [in] data
	*		����
	*	@return
	*		void
	*/
	void DisplaySvcEngine_AnswerBacklightMode(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_AnswerBacklightMode\n");
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] DisplaySvcEngine_AnswerBacklightMode data is NULL!\n");
			return;
		}
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		*(ins->m_backLight) = *(DeSai_BackLight*)data;

		if (ins->m_displaySvcBackLightEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_displaySvcBackLightEvent);
		}
		else
		{
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)DisplaySvcEngine_BacklightModeChangeNotify, NULL);
		}
		return;
	}

	/**
	*	@brief
	*		������ʾMusicWidget
	*	@return
	*		void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_ReqShowMusicWidget()
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ReqShowMusicWidget\n");

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqShowMusicWidget;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		// ֪ͨClient��ʾ�ɹ�
		DisplaySvcEngine_ShowMusicWidgetNotify(true);
		// ��������Widget״̬
		m_widgetStatus = EN_DISPLAYSVCENGINE_WIDGET_SHOW;

		return;
	}

	/**
	*	@brief
	*		��������MusicWidget
	*	@return
	*		void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_ReqHideMusicWidget()
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ReqHideMusicWidget\n");

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqHideMusicWidget;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//�ͻ������Ʊȳ���widget��������ʱ���
		Mapbar_sleep(150);
		// ֪ͨClient���سɹ�
		DisplaySvcEngine_HideMusicWidgetNotify(true);
		// ��������Widget״̬
		m_widgetStatus = EN_DISPLAYSVCENGINE_WIDGET_HIDE;
		return;
	}

	/**
	*	@brief
	*		�������߳�ִ����Ϣ�ص�
	*	@param [in] data
	*		������ʾ�ɹ���ʧ��
	*	@return
	*		void
	*/
	void ShowMusicWidgetNotify(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] ShowMusicWidgetNotify\n");
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		bool isSucess = *(bool*)(data);
		if (isSucess)
		{
			ins->m_widgetStatus = EN_DISPLAYSVCENGINE_WIDGET_SHOW;
		}
		else
		{
			ins->m_widgetStatus = EN_DISPLAYSVCENGINE_WIDGET_HIDE;
		}
		//ins->DisplaySvcEngine_ShowMusicWidgetNotify(isSucess);
		return;
	}

	/**
	*	@brief
	*		��Recieverע���Callback����,����������ʾMusicWidge�Ľ��
	*	@param [in] data
	*		��ʾ�ɹ�ʧ��
	*	@return
	*		void
	*/
	void AnswerShowMusicWidget(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] AnswerShowMusicWidget\n");
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AnswerShowMusicWidget data is NULL!\n");
			return;
		}	
		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)ShowMusicWidgetNotify, data);
		return;
	}

	/**
	*	@brief
	*		�������߳�ִ����Ϣ�ص�
	*	@param [in] data
	*		�������سɹ���ʧ��
	*	@return
	*		void
	*/
	void HideMusicWidgetNotify(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] HideMusicWidgetNotify\n");
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		bool isSucess = *(bool*)(data);
		if (!isSucess)
		{
			ins->m_widgetStatus = EN_DISPLAYSVCENGINE_WIDGET_SHOW;
		}
		else
		{
			ins->m_widgetStatus = EN_DISPLAYSVCENGINE_WIDGET_HIDE;
		}
		//ins->DisplaySvcEngine_HideMusicWidgetNotify(isSucess);
		return;
	}

	/**
	*	@brief
	*		��Recieverע���Callback����,������������MusicWidge�Ľ��
	*	@param [in] data
	*		�������سɹ���ʧ��
	*	@return
	*		void
	*/
	void AnswerHideMusicWidget(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] AnswerHideMusicWidget\n");
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AnswerHideMusicWidget data is NULL!\n");
			return;
		}		
		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)HideMusicWidgetNotify, data);
		return;
	}

	/**
	*	@brief
	*		��ȡMusicWidget��ʾ״̬
	*	@return
	*		EN_DISPLAYSVCENGINE_WIDGET_STATE
	*/
	EN_DISPLAYSVCENGINE_WIDGET_STATE DisplaySvcEngine::DisplaySvcEngine_GetMusicWidgetStatus()
	{
		return m_widgetStatus;
	}

	/**
	*	@brief
	*		�������󳵻��ṩ����ID
	*	@return
	*/
	int32 DisplaySvcEngine::GetLanguageID()
	{
		CQ_LOG_INFO("[ServiceManager] GetLanguageID\n");
		if(m_languageId != -1)
			return m_languageId;
		// ͬ���ӿڣ���û�������ص������Ա��ز���洢
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return -1;
		}
		paramData->funcId = ServiceManager::FuncID_Send_GetLanguageID;
		paramData->data = (void*)NULL;

		m_displaySvcLanguageIdEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		//MapbarWaitState state = Mapbar_waitWithTimeout(m_displaySvcLanguageIdEvent, 1000);//�ȴ�1��
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcLanguageIdEvent, 1000);//�ȴ�1��
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_displaySvcLanguageIdEvent);
			m_displaySvcLanguageIdEvent = invalidHandle;
			return m_languageId;
		}

		Mapbar_destroyEvent(m_displaySvcLanguageIdEvent);
		m_displaySvcLanguageIdEvent = invalidHandle;
		return -1;
	}
	
	void DisplaySvcEngine_LanguageIDChangeNotify(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_LanguageIDChangeNotify\n");
		DisplaySvcEngine::getInstance()->languageIdChangNotify(*(int32*) data);
		//free(data);
		//data = NULL;
	}
	

	/**
	*	@brief
	*		��Recieverע�������Callback����
	*	@param [in] data
	*		����
	*	@return
	*		void
	*/
	void DisplaySvcEngine_AnswerLanguageID(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_AnswerLanguageID\n");
		DisplaySvcEngine * ins = DisplaySvcEngine::getInstance();
		int languageid = -1;
		
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] DisplaySvcEngine_AnswerLanguageID data is NULL!\n");
			ins->m_languageId = -1;
			if (ins->m_displaySvcLanguageIdEvent != invalidHandle)
			{
				Mapbar_setEvent(ins->m_displaySvcLanguageIdEvent);
			}
			return;
		}	
		languageid = *(int*)data;
		ins->m_languageId = languageid;
		if (ins->m_displaySvcLanguageIdEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_displaySvcLanguageIdEvent);
		}
		return;
	}
	
	void DisplaySvcEngine_LanguageSwitchChanged(void* data)
	{
		UNUSED_VAR(data);
		if(data == NULL)
		{
			CQ_LOG_ERROR("[ServiceManager] DisplaySvcClient_LanguageSwitchChanged data is NULL!\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			if (paramData == NULL)
			{
				return;
			}
			paramData->funcId = ServiceManager::FuncID_Send_LanguageSwitchFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		else
		{
			DisplaySvcEngine * ins = DisplaySvcEngine::getInstance();
			ins->m_languageId  = *(int32*)data;
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)DisplaySvcEngine_LanguageIDChangeNotify, (void *)data);
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			if (paramData == NULL)
			{
				return;
			}
			paramData->funcId = ServiceManager::FuncID_Send_LanguageSwitchSuccess;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		}
		return ;
	}
	
	/*========================Desai BackToHost============================*/
	/**
	*	@brief
	*		�Ȼָ��˵����ڽ��뵼��APP֮ǰ
	*	@return
	*		void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_BackToHost()
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_BackToHost\n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_BackToHost;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return;
	}

	/*========================Desai ShowHostMenu============================*/
	/**
	*	@brief
	*		��ʾ�����˵�
	*	@return
	*		void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_ShowHostMenu()
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ShowHostMenu\n");
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ShowHostMenu;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return;
	}

	/*===============================����=================================*/
	/*====================================== TimeFormat==============================*/
	/**
	*	@brief
	*		�������󳵻�ʱ���ʽ
	*	@return
	*	TRUE:
	*		24-hour clock.
	*	FALSE:
	*		12-hour clock.
	*/
	EN_DISPLAYSVCENGINE_TIME_FORMAT DisplaySvcEngine::getTimeFormat()
	{
		if (m_timeFormat == EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN)
		{
			DisplaySvcEngine_ReqSysHmiTime();
			if (m_timeFormat != EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN)
			{
				return m_timeFormat;
			}
			else
			{
				//return EN_DISPLAYSVCENGINE_TIME_FORMAT_24HOUR;
				//dummy
				m_timeFormat = EN_DISPLAYSVCENGINE_TIME_FORMAT_24HOUR;
				return m_timeFormat;
			}
		}
		return m_timeFormat;
	}

	void TimeFormatChangeNotify(void *data)
	{
		UNUSED_VAR(data);
		DisplaySvcEngine::getInstance()->TimeFormatNotify(DisplaySvcEngine::getInstance()->m_timeFormat);
	}

	void DisplaySvcEngine::TimeFormatNotify(EN_DISPLAYSVCENGINE_TIME_FORMAT data)
	{
		std::set<DisplaySvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((DisplaySvcEngineListener*)*iter)->onTimeFormatNotify(data);

		return;
	}
	/*====================================== TimeFormat==============================*/

	/*===============================3.16 HMIʱ����Ϣ=================================*/
	/**
	*	@brief
	*		����ϵͳHMIʱ��
	*	@return
	*		uint64 ϵͳʱ���ֵ
	*/
	uint64 DisplaySvcEngine::DisplaySvcEngine_ReqSysHmiTime()
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ReqSysHmiTime\n");
		// HMIʱ�����ʵʱ����
		/*if (m_timeDValue != 0)
		{
			return m_timeDValue;
		}*/

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return 0;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReqSysHmiTime;
		paramData->data = (void*)NULL;

		m_displaySvcSysHmiTimeEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcSysHmiTimeEvent, 1000);//�ȴ�1��
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_displaySvcSysHmiTimeEvent);
			m_displaySvcSysHmiTimeEvent = invalidHandle;

			return m_timeDValue;
		}

		Mapbar_destroyEvent(m_displaySvcSysHmiTimeEvent);
		m_displaySvcSysHmiTimeEvent = invalidHandle;
		return 0;
	}

	void DisplaySvcEngine::DisplaySvcEngine_SysHmiTimeNotify(uint64 data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_SysHmiTimeNotify\n");
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onSysHmiTimeChangeNotify(data);
		}
		return;
	}

	void SysHmiTimeChangedNotify(void* data)
	{
		UNUSED_VAR(data);
		CQ_LOG_INFO("[ServiceManager] SysHmiTimeNotify\n");
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		ins->DisplaySvcEngine_SysHmiTimeNotify(ins->m_timeDValue);
		return;
	}

	void AnswerSysHmiTime(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] AnswerShowMusicWidget\n");
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AnswerShowMusicWidget data is NULL!\n");
			if (ins->m_displaySvcSysHmiTimeEvent != invalidHandle)
			Mapbar_setEvent(ins->m_displaySvcSysHmiTimeEvent);
			return;
		}
		ins->m_timeInfo = (TimeInfo*)data;
		ins->m_timeDValue = ins->m_timeInfo->timeValue;
		//if (ins->m_timeFormat != EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN)
		//{
		//	ins->m_timeFormat = ins->m_timeInfo->timeFormat;
		//}
		//dummy
		if (ins->m_timeInfo->timeFormat== EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN)
		{
			ins->m_timeFormat = EN_DISPLAYSVCENGINE_TIME_FORMAT_24HOUR;
		}
		else
		{
			ins->m_timeFormat = ins->m_timeInfo->timeFormat;
		}

		if (ins->m_displaySvcSysHmiTimeEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_displaySvcSysHmiTimeEvent);
		}
		else
		{
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)SysHmiTimeChangedNotify, NULL);
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)TimeFormatChangeNotify, NULL);
		}
		return;
	}
	/*===============================3.16 HMIʱ����Ϣ=================================*/

	/*===============================3.19 ��ҹģʽ����=================================*/
	
	/**
	*	@brief
	*		��ҹģʽ����ioctrl��ӿڣ�����cmdidָ����Ӧ�Ĵ�����
	*	@return
	*		0���������óɹ� -1����������ʧ��
	*/
	
	int32_t DisplaySvcEngine::DisplaySvcEngine_ReqSysLinkAgeInfo(Linkage_Ctrl_ID cmdid,void *data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ReqSysLinkAgeInfo [IN]\n");
		switch(cmdid)
		{
		case NOTIFY_SYS_LINKAGE_YES:
		case NOTIFY_SYS_LINKAGE_NO:
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			if (paramData == NULL)
			{
				return -1;
			}
			paramData->funcId = ServiceManager::FuncID_Send_SetDayNightMode;
			int* pdata = NULL;
			pdata = (int*)malloc(sizeof(int));
			if (pdata == NULL)
			{
				free(paramData);
				paramData = NULL;
				return -1;
			}
			cmdid == NOTIFY_SYS_LINKAGE_YES ? *pdata = 1: *pdata = 0;
			paramData->data = pdata;
			//paramData->data = NULL;
			if(cmdid == NOTIFY_SYS_LINKAGE_NO)
			{
				//m_dayNightLinkAgeData.linkageevent.bits.isAlreadyGeted = 0;
				m_dayNightLinkAgeData.status = 0;
				m_dayNightLinkAgeData.status = -1;
			}
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		}
			break;
		case GET_DAYNIGHT_MODE_LINKAGE_STATUS:
		{
			/* if not first get use local date */
			{
				if(m_dayNightLinkAgeData.linkageevent.bits.isAlreadyGeted == 1)
				{
					memcpy(data,&m_dayNightLinkAgeData,sizeof(DayNight_Mode_Status));
					return 0;
				}
			}
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			if (paramData == NULL)
			{
				return -1;
			}
			paramData->funcId = ServiceManager::FuncID_Send_GetDayNightMode;
			paramData->data = (void*)NULL;

			m_displaySvcDayNightLinkAgeEvent = Mapbar_createEvent(FALSE, FALSE);
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			MapbarWaitState state = Mapbar_waitEventWithTimeout(m_displaySvcDayNightLinkAgeEvent, 1000);//�ȴ�1��
			if (state == MapbarWaitState_ok)
			{
				memcpy(data,&m_dayNightLinkAgeData,sizeof(DayNight_Mode_Status));
				Mapbar_destroyEvent(m_displaySvcDayNightLinkAgeEvent);
				m_displaySvcDayNightLinkAgeEvent = invalidHandle;
				return 0;
			}

			Mapbar_destroyEvent(m_displaySvcDayNightLinkAgeEvent);
			m_displaySvcDayNightLinkAgeEvent = invalidHandle;
			return -1;
		}
			break;
		default :
			return -1;
			break;
		}
		return 0;
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ReqSysLinkAgeInfo [OUT]\n");
	}
	
	/**
	*	@brief
	*		���ÿͻ��˻ص�������ִ����ҹģʽ���涯��
	*	@return
	*		void
	*/
	
	void DisplaySvcEngine::DisplaySvcEngine_SysDayNightLinkAgNotify(int32_t status,int32_t mode)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_SysDayNightLinkAgNotify [IN]\n");
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onSysDayNightModeSwitchChangeNotify(status,mode);
		}
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_SysDayNightLinkAgNotify [OUT]\n");
		return;
	}
	
	/**
	*	@brief
	*		������ҹģʽ����ģʽ����
	*	@return
	*		void
	*/
	
	void SysDayNightLinkAgNotify(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] SysDayNightLinkAgNotify [IN]\n");
		UNUSED_VAR(data);
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		ins->DisplaySvcEngine_SysDayNightLinkAgNotify(ins->m_dayNightLinkAgeData.status,ins->m_dayNightLinkAgeData.mode);
		CQ_LOG_INFO("[ServiceManager] SysDayNightLinkAgNotify [OUT]\n");
	}
	
	/**
	*	@brief
	*		����֪ͨ��������ҹ����ģʽ������Ϣ
	*	@return
	*		void
	*/
	
	void NotifyDayNightLinkAge(void *data)
	{
		CQ_LOG_INFO("[ServiceManager] NotifyDayNightLinkAge [IN] data is [%s]\n",data ? data: "NULL");
		if(NULL == data)
		{
			return;
		}
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		if (ins->m_displaySvcDayNightLinkAgeEvent != invalidHandle)
		{
			/* TBD get data */
			CQ_LOG_INFO("[ServiceManager] get linkage resp data\n");
			sscanf((const char *)data,"%d,%d",&ins->m_dayNightLinkAgeData.status,&ins->m_dayNightLinkAgeData.mode);
			ins->m_dayNightLinkAgeData.linkageevent.bits.isAlreadyGeted = 1;
			Mapbar_setEvent(ins->m_displaySvcDayNightLinkAgeEvent);
		}
		else
		{
			CQ_LOG_INFO("[ServiceManager] get linkage notify data\n");
			sscanf((const char *)data,"%d,%d",&ins->m_dayNightLinkAgeData.status,&ins->m_dayNightLinkAgeData.mode);
			ins->m_dayNightLinkAgeData.linkageevent.bits.isAlreadyGeted = 1;
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)SysDayNightLinkAgNotify, data);
		}
		CQ_LOG_INFO("[ServiceManager] NotifyDayNightLinkAge [OUT]\n");
	}
	
	/*===============================2.1.1 ֪ͨ���������л�ǰ��̨=====================*/
	/**
	*	@brief
	*		֪ͨ���������л�ǰ��̨
	*	@return
	*		void
	*/
	void DisplaySvcEngine::DisplaySvcEngine_ReadyForForground()
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcEngine_ReadySwitchForground\n");

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = ServiceManager::FuncID_Send_ReadyForForgroud;
		paramData->data = (void*)NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
	}


	/**
	*	@brief
	*		����֪ͨ�����������¼��ź�ʹ����ر� 
	*	@return
	*		void
	*/
/*===============================3.19 �����¼��ź�ʹ����ر�=====================*/


	void DisplaySvcEngine::DisplaySvcEngine_TouchStatusChangedNotify(bool data)
	{
		std::set<DisplaySvcEngineListener*>::iterator iter;
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
		{
			((DisplaySvcEngineListener*)*iter)->onTouchStatusChangeNotify(data);
		}
		return;
	}

	int TouchStatusChangedNotify(void *data)
	{
		DisplaySvcEngine* ins = DisplaySvcEngine::getInstance();
		memcpy(&(ins->m_touchInfo), (TouchInfo*)data, sizeof(TouchInfo));
		if (ins->m_touchInfo.tickCount < ins->m_tickCount)
		{
			CQ_LOG_ERROR("[ServiceManager] This message is early!\n");
			return 0;
		}
		ins->m_tickCount = ins->m_touchInfo.tickCount;
		DisplaySvcEngine::getInstance()->DisplaySvcEngine_TouchStatusChangedNotify(ins->m_touchInfo.status);
		return 0;
	}

	void NotifyTouchStatus(void* data)
	{

		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] NotifyTouchStatus data is NULL!\n");
			return;
		}

		Mapbar_runFunctionInMainThread(TouchStatusChangedNotify, (void*)data);

	}

/*===============================3.19 �����¼��ź�ʹ����ر�=====================*/
}
