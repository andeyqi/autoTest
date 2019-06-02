#include "stdafx_serviceManagerClient.h"
//#include "mbapi/mbapi_app_service.h"
#include "serviceManagerEngine_Display.h"
//#include "serviceManagerLog.h"
using namespace MbApi;

namespace ServiceManager
{
	// 静默模式
	static SLIENT_MODE g_silentMode = SLIENT_MODE_OFF;

	void updateRoadNameForLanguage(ALanguageType lang);
	void updateCurrentRoadName(cqWCHAR* RoadName);
	/**
	*	@brief
	*		DisplaySvc初期化
	*	@return
	*		void
	*	@remarks
	*		向Recieve模块和Mbapi模块注册Callback函数
	*/
	void DisplaySvcClient_Initialize()
	{
		//向Recieve模块进行Callback函数注册
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_ReqShowNaviForm, DisplaySvcClient_ReqShowNavi);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_ReqHideNaviForm, DisplaySvcClient_ReqHideNavi);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_AnswerSendShowNaviFormSuc, DisplaySvcClient_ReqRefreshNavi);
		//NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_LanguageSwitchChanged, DisplaySvcClient_LanguageSwitchChanged);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_SilentModeChanged, DisplaySvcClient_SilentModeChanged);

		return;
	}
	
	
	/**
	*	@brief
	*		DisplaySvc清除
	*	@return
	*		void
	*/

	//delete by shenghao for exit timeout
	//void DisplaySvcClient_CleanUp()
	//{
	//	CQ_LOG_INFO("[ServiceManager] DisplaySvcClient_CleanUp.\n");
	//	return;
	//}
	
	
	/*========================2.3 画面迁移 显示Navi画面============================*/
	/**
	*	@brief
	*		导航显示Navi画面
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_ReqFormShow(void* data)
	{	
		UNUSED_VAR(data);
		//paint、Mapdal刷新		
		MbApi::AppService::getInstance()->setRenderEnable(TRUE);
		//退出静默模式
		g_silentMode = SLIENT_MODE_OFF;
		MbApi::App_invalidateScreen();
		//MbApi::App_updateScreen();
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->funcId = ServiceManager::FuncID_Send_ShowNaviFormSuc;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		return;
	}

	
	/**
	*	@brief
	*		车机请求导航显示Navi画面
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_ReqShowNavi(void* data)
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

		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d] navi is pre\n",__FUNCTION__,__LINE__);
		ServiceManager::DisplaySvcEngine::getInstance()->SetNaviRunStatus(false);
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)DisplaySvcClient_ReqFormShow, NULL);

		return;
	}
	/*========================2.3 画面迁移 显示Navi画面============================*/
	//
	//
	//
	/*========================2.3 画面迁移 隐藏Navi画面============================*/
	/**
	*	@brief
	*		导航隐藏Navi画面
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_ReqFormHide(void* data)
	{
		UNUSED_VAR(data);
		//paint、Mapdal停止刷新		
		MbApi::AppService::getInstance()->setRenderEnable(FALSE);
		return;
	}

	
	/**
	*	@brief
	*		车机请求导航隐藏Navi画面
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_ReqHideNavi(void* data)
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

		ServiceManager::DisplaySvcEngine::getInstance()->SetNaviRunStatus(true);
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d] navi is back\n",__FUNCTION__,__LINE__);
		/*call  force stop*/
		if(!(RealNavigationState_inNavigation == MbApi::Navi_getNavigationState() || MbApi::Navi_isInSimulation()))
		{
			MbApi::SystemSound_forceStop();
			SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d] navi is afte forcestop\n",__FUNCTION__,__LINE__);
		}

		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)DisplaySvcClient_ReqFormHide, NULL);

		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->funcId = ServiceManager::FuncID_Send_HideNaviFormSuc;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		return;
	}

	/**
	*	@brief
	*		刷新Navi画面
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_RefreshNavi(void* data)
	{
		UNUSED_VAR(data);
		//2017.10.19 丰田19DOP 设置页面退出再进入时，强制刷新
		MbApi::App_invalidateScreen();

		return;
	}
	/**
	*	@brief
	*		通知车机导航显示成功后，刷新Navi画面
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_ReqRefreshNavi(void* data)
	{
		UNUSED_VAR(data);
		
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)DisplaySvcClient_RefreshNavi, NULL);

		return;
	}
	/*======================== 3.19 中英文切换 ============================*/
	/*
	*	@brief
	*		导航Receive注册的Callback函数
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_LanguageSwitchChanged(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] DisplaySvcClient_LanguageSwitchChanged\n");
		if (data == NULL)
		{
			CQ_LOG_ERROR("[ServiceManager] DisplaySvcClient_LanguageSwitchChanged data is NULL!\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_LanguageSwitchFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		else
		{
			MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)LanguageSwitchNotify, (void *)data);

			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_LanguageSwitchSuccess;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}
		return;
	}

	//通知函数
	void LanguageSwitchNotify(void *data)
	{
		SETTING_SERVICE()->setLanguage(*(ALanguageType*)data);

		// 更新仪表路名
		updateRoadNameForLanguage(*(ALanguageType*)data);
		return;
	}

	/*
	*	@brief
	*		语言切换后，更新当前路名
	*	@return
	*	 	void
	*/
	void updateRoadNameForLanguage(ALanguageType lang)
	{
		if (RealNavigationState_inNavigation != MbApi::Navi_getNavigationState()
			&& RealNavigationState_pause != MbApi::Navi_getNavigationState()
			&& !MbApi::Navi_isInSimulation()
			&& !MbApi::Navi_isPauseSimulation())
		{
		// 自车位置，与client保持一致
			NaviDataCenter* dc = MbApi::Session_getDataCenter();
			SegmentGrabInfo info = { 0 };
			DSegmentAttributes attrTmp;	//函数内使用
			cqWCHAR roadName[MAX_SIZE] = { 0 };
			uint32 i = MbApi::Map_grabSegments(&dc->m_carPos, &info, FALSE);//实际道路名，无名路也正常取
			if (i == 0)
			{
				if (ALanguage_enu == lang)
					cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
				else
					cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
				updateCurrentRoadName(roadName);
			}
			else
			{
				MbApi::RouteBaseApi_getDSegmentAttributes(info.dsegId, &attrTmp, MbApi::MbApiSegmentAttributesFlag_All);
				if (attrTmp.name[0] == 0)
				{
					if (ALanguage_enu == lang)
						cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
					else
						cq_wcscat_s(roadName, MAX_SIZE, L"无名路");					
				}					
				else
				{
					cq_wcscpy_s(roadName, element_of(attrTmp.name), attrTmp.name);
				}
				updateCurrentRoadName(roadName);
			}		
		}
	}

	/*
	*	@brief
	*		更新当前路名
	*	@return
	*	 	RoadName
	*/
	void updateCurrentRoadName(cqWCHAR* RoadName)
	{
		if (NULL == RoadName)
		{
			CQ_LOG_ERROR("[ServiceManager] updateRoadName NULL!\n");
			return;
		}
		CQ_LOG_INFO("[ServiceManager]:Current road name is changed, and send signal now.\n");
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_UpdateRoadName;
		paramData->data = (cqWCHAR*)malloc(sizeof(cqWCHAR)* (cq_wcslen(RoadName) + 1));
		memcpy(paramData->data, RoadName, sizeof(cqWCHAR)* (cq_wcslen(RoadName) + 1));
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}

	/*
	*	@brief
	*		通知Client静默模式变化
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_NotifySilentModeChange(void* data)
	{
		if (NULL == data || *(int32*)data == -1)
			return;

		SLIENT_MODE m_mode = *(SLIENT_MODE*)data;
		
		MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
		*pCmdBackSig = MBAPI_VR_COMMAND_DID_SUCESS;
		
		// 当前为非静默模式
		if (SLIENT_MODE_OFF == g_silentMode)
		{	
			// 进入静默通知
			if (SLIENT_MODE_ON == m_mode)
			{
				// 导航在前台时，先切至后台
				if (!ServiceManager::DisplaySvcEngine::getInstance()->GetNaviRunStatus()) // 前台：false，后台true
					MbApi::AppService::getInstance()->setRenderEnable(FALSE);
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_SILENT_MODE_ENTER, NULL, pCmdBackSig);
			}
			// 退出静默通知
			else if (SLIENT_MODE_OFF == m_mode)
			{
				// 导航在后台时，先切至前台，再回复路线
				if (ServiceManager::DisplaySvcEngine::getInstance()->GetNaviRunStatus()) // 前台：false，后台true
				{
					MbApi::AppService::getInstance()->setRenderEnable(TRUE);
					MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_SILENT_MODE_EXIT, NULL, pCmdBackSig);
				}
				
			}
		}
		// 当前为静默模式
		else if (SLIENT_MODE_ON == g_silentMode)
		{
			// 退出静默通知
			if (SLIENT_MODE_OFF == m_mode)
			{
				// 导航在后台时，先切至前台
				if (ServiceManager::DisplaySvcEngine::getInstance()->GetNaviRunStatus()) // 前台：false，后台true
					MbApi::AppService::getInstance()->setRenderEnable(TRUE);
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_SILENT_MODE_EXIT, NULL, pCmdBackSig);
			}
		}
		// 保存SilentMode
		g_silentMode = m_mode;
		// 向车机发送消息
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));		
		
		//if (MBAPI_VR_COMMAND_DID_SUCESS == *pCmdBackSig)
			paramData->funcId = ServiceManager::FuncID_Send_SilentModeChangeSucess;
		//else if (MBAPI_VR_COMMAND_DID_FAILURE == *pCmdBackSig)
			//paramData->funcId = ServiceManager::FuncID_Send_SilentModeChangeFailure;
		
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		free(pCmdBackSig);
		pCmdBackSig = NULL;
		return;
	}



	/*
	*	@brief
	*		静默模式变更回调
	*	@return
	*	 	void
	*/
	void DisplaySvcClient_SilentModeChanged(void* data)
	{
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)DisplaySvcClient_NotifySilentModeChange, data);
		return;
	}
}
