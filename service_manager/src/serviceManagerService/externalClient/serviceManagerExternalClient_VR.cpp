#include "stdafx_serviceManagerClient.h"
//#include "mbapi/mbapi_system_setting.h"
//#include "mbapi/mbapi_value.h"
#include "mapdal/message_loop.h"
//#include "mbapi/mbapi_poi_query.h"

#ifdef MAPBAR_TOYOTA_262AVN_LINUX
#include "serviceManagerExternalClient_VR_Shm.h"
#endif
 
using namespace MbApi;
#define CLIENT_CONFIG_FILE_JSON		RegionList_buildFileName(L"userdata/mapbar_client_data.cfg")
#define VR_SEARCH_MAX_NUM	20

namespace ServiceManager
{
	static VRServiceClient* g_VRServiceClient = NULL;
	// 保存上次路线
	static MbApi::RouteService::Plan g_VRLastRoutePlan;
	BOOL isDeletePassPointReqPlan = FALSE;
	BOOL isItemSeletReqPlan = FALSE;
	BOOL isPassAndDestReqPlan = FALSE;
	BOOL isCollectionReqPlan = FALSE;
	BOOL isRouteSwitchReqPlan = FALSE;
	BOOL isAvoidJamSet = FALSE;
	static uint32_t VRStartRouteBitMask = 0;
		
	// 函数声明
	void VRCommandControlSig(void* data);
	void VRUdeSearchSig(void* data);
	void VRNearbySearchSig(void* data);
	void VRTrafficControlSig(void* data);
	void VRPassingPointSig(void* data);
	void VRSwitchRouteSig(void* data);
	void VRItemSelectSig(void* data);
	void VRInquiryDistanceSig(void* data);
	void VRPassingPoiAndDestPoiSig(void* data);
	uint32 VRDecode(uint32 latorlon);
	void VRCollectionSig(void* data);
	void VRCollectionSigAnalysis(void* data);
	void VRStartNaviSig(void* data);
	
	// 将json数据转为路线偏好
	bool VRGetHybridRouteType(const json_t* json, HybridRouteParam* value);
	// 从文件中加载路线偏好
	json_t* VRDataLoadFromFile();
	// 携带路线偏好进行算路
	void VRHybridStartRoute(RouteService::Plan& plan, const HybridRouteParam& param);
	// 添加途径点
	void VRAddPoint2FavSig(void* data);
	/**
	*	@brief
	*		VoiceSvc初期化
	*	@return
	*		void
	*	@remarks
	*		向Recieve模块注册Callback函数
	*/
	void VRSvcClient_Initialize()
	{
		if (NULL == g_VRServiceClient)
		{
			g_VRServiceClient = new VRServiceClient();
			g_VRServiceClient->initialize();
		}
		//向Recieve模块进行Callback函数注册 
		CQ_LOG_INFO("[ServiceManager] VRSvcClient_Initialize.\n");
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRUdeSearch, VRUdeSearchSig);				
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRCommandControl, VRCommandControlSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRTrafficControl, VRTrafficControlSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRPassingPoint, VRPassingPointSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRSwitchRoute, VRSwitchRouteSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRItemSelect, VRItemSelectSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRInquiryDistance, VRInquiryDistanceSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRSearchNearby, VRNearbySearchSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRPassPointAndDestPoint, VRPassingPoiAndDestPoiSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRCollection, VRCollectionSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_VRStartNavi, VRStartNaviSig);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_AddFavPoint, VRAddPoint2FavSig);
		#ifdef MAPBAR_TOYOTA_262AVN_LINUX
		shm_Server_Init();
		#endif
		return;
	}


	/**
	*	@brief
	*		VR Clean up
	*	@return
	*		void
	*/
	//delete by shenghao for exit timeout
	//void VRSvcClient_CleanUp()
	//{
	//	CQ_LOG_INFO("[ServiceManager] VRSvcClient_CleanUp.\n");
	//	return;
	//}

	/**
	*	@brief
	*		导航显示Navi画面
	*	@return
	*	 	void
	*/
	void VRCommandControlSigAnalysis(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRCommandControlSigAnalysis.\n");

		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CommandControlFailure;
			paramData->data = (VR_COMMAND_CONTROL_FAILED_TYPE*)malloc(sizeof(VR_COMMAND_CONTROL_FAILED_TYPE));
			*((VR_COMMAND_CONTROL_FAILED_TYPE*)paramData->data) = VR_COMMAND_CONTROL_FAILED_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}

		BOOL isMbApiSucess = FALSE;
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->data = (VR_COMMAND_CONTROL_FAILED_TYPE*)malloc(sizeof(VR_COMMAND_CONTROL_FAILED_TYPE));
		VR_COMMAND_CONTROL_FAILED_TYPE failedType = VR_COMMAND_CONTROL_FAILED_OTHER;
		if (NULL != data)
		{			
			CommandControlSig* m_sig = (CommandControlSig*)data;
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT;
			CQ_LOG_INFO("[ServiceManager] VRCommandControlSigAnalysis m_sig->uiCommand = %d.\n", m_sig->uiCommand);
			// MbApi返回结果		
			switch (m_sig->uiCommand)
			{
			case VR_COMMAND_CANCLE_NAVI:
			{
				// 取消导航
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_CANCLE, NULL, pCmdBackSig);
				*pCmdBackSig = MBAPI_VR_COMMAND_DID_SUCESS;	// redmine 2762 路线选择页面或非导航状态，返回成功
			}
				break;
			case VR_COMMAND_CHANGE_MAP_MODE:
			{
				// 切换地图模式
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_MAP_MODE_SWITCH, NULL, pCmdBackSig);
			}
				break;
			case VR_COMMAND_CHANGE_MAP_TO:
			{
				// 切换到指定地图模式
				CQ_LOG_INFO("[ServiceManager] VRCommandControlSigAnalysis m_sig->uiMapMode = %d.\n", m_sig->uiMapMode);
				if (m_sig->uiMapMode != VR_COMMAND_MAP_INVALIDE)
				{
					MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_SPECIFIED_MAP_MODE_SWITCH, &m_sig->uiMapMode, pCmdBackSig);
				}
			}
				break;
			case VR_COMMAND_MAP_ZOOMIN:
				// 放大地图
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_ZOOM_IN, NULL, pCmdBackSig);
				break;
			case VR_COMMAND_MAP_ZOOMOUT:
				// 缩小地图
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_ZOOM_OUT, NULL, pCmdBackSig);
				break;
			default:
				break;
			}

			// 判断成功与失败类型
			/*if (!isMbApiSucess)
			{*/
				switch (*pCmdBackSig)
				{
				case MBAPI_VR_COMMAND_DID_SUCESS:
					isMbApiSucess = true;
					break;
				// 失败或者当前页面不支持
				case MBAPI_VR_COMMAND_DID_FAILURE:
				case MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT:
					failedType = VR_COMMAND_CONTROL_FAILED_OTHER;
					isMbApiSucess = false;
					break;
				// 当前状态已设置
				case MBAPI_VR_COMMAND_SAME_WITH_CURRENT_STATE:
					failedType = VR_COMMAND_CONTROL_SAME_STATUS;
					isMbApiSucess = false;
					break;
				// 当前状态已是极限值
				case MBAPI_VR_COMMAND_CURRENT_STATE_IS_MAX:
				case MBAPI_VR_COMMAND_CIRREMT_STATE_IS_MIN:
					failedType = VR_COMMAND_CONTROL_LIMIT_VALUE;
					isMbApiSucess = false;
					break;
				default:
					break;
				}

			//}	
			
			free(pCmdBackSig);
			pCmdBackSig = NULL;
		}

		// 通知车机本次请求结果
		//ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (isMbApiSucess)
		{
			paramData->funcId = ServiceManager::FuncID_Send_CommandControlSucess;
			free(paramData->data);
			paramData->data = NULL;
		}
		else
		{
			paramData->funcId = ServiceManager::FuncID_Send_CommandControlFailure;
			memcpy(paramData->data, &failedType, sizeof(VR_COMMAND_CONTROL_FAILED_TYPE));
		}
		//paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

		return;
	}

	/**
	*	@brief
	*		VR命令控制Callback
	*	@return
	*		void
	*/
	void VRCommandControlSig(void* data)
	{
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRCommandControlSigAnalysis, data);
	}

	/**
	*	@brief
	*		VR实时路况命令解析与执行
	*	@return
	*		void
	*/
	void VRTrafficControlSigAnalysis(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRTrafficControlSigAnalysis .\n");
		
		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CommandTrafficFailure;
			paramData->data = (VR_TRAFFIC_COMMAND_FAILED_TYPE*)malloc(sizeof(VR_TRAFFIC_COMMAND_FAILED_TYPE));
			*((VR_TRAFFIC_COMMAND_FAILED_TYPE*)paramData->data) = VR_TRAFFIC_COMMAND_FAILED_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}

		// 通知车机本次请求结果
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->data = (VR_TRAFFIC_COMMAND_FAILED_TYPE*)malloc(sizeof(VR_TRAFFIC_COMMAND_FAILED_TYPE));

		// 返回执行结果，成功或失败（失败原因）,默认页面不支持
		MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
		*pCmdBackSig = MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT;

		BOOL isTrafficSucess = FALSE;
		uint32 failedType = VR_TRAFFIC_COMMAND_FAILED_OTHER;
		uint32 trafficCommandReq = VR_COMMAND_TRAFFIC_INVALIDE;

		VR_COMMAND_TRAFFIC_SIG traffic_sig = *(VR_COMMAND_TRAFFIC_SIG*)data;
		CQ_LOG_INFO("[ServiceManager] VRTrafficControlSigAnalysis traffic_sig = %d.\n", traffic_sig);
		switch (traffic_sig)
		{
		case VR_COMMAND_TRAFFIC_OFF:
			// 关闭路况
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_TMC_CLOSE, NULL, pCmdBackSig);
			trafficCommandReq = VR_COMMAND_TRAFFIC_OFF;
			break;
		case VR_COMMAND_TRAFFIC_ON:
			// 开启路况
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_TMC_OPEN, NULL, pCmdBackSig);
			trafficCommandReq = VR_COMMAND_TRAFFIC_ON;
			break;
		default:
			break;
		}

		// 判断成功与失败类型
		CQ_LOG_INFO("[ServiceManager] VRTrafficControlSigAnalysis pCmdBackSig = %d.\n", *pCmdBackSig);
		switch (*pCmdBackSig)
		{
		case MBAPI_VR_COMMAND_DID_SUCESS:
			isTrafficSucess = true;
			break;
		case MBAPI_VR_COMMAND_DID_FAILURE:
		case MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT:
		{
			failedType = VR_TRAFFIC_COMMAND_FAILED_OTHER;
			isTrafficSucess = false;
		}
			break;
		case MBAPI_VR_COMMAND_SAME_WITH_CURRENT_STATE:
		{
			if (trafficCommandReq == VR_COMMAND_TRAFFIC_ON)
			{
				failedType = VR_TRAFFIC_COMMAND_HAS_OPENED;
			}
			else if (trafficCommandReq == VR_COMMAND_TRAFFIC_OFF)
			{
				failedType = VR_TRAFFIC_COMMAND_HAS_CLOSED;
			}
			isTrafficSucess = false;
		}
			break;
		default:
			break;
		}
		
		if (isTrafficSucess)
		{
			paramData->funcId = ServiceManager::FuncID_Send_CommandTrafficSucess;
			free(paramData->data);
			paramData->data = NULL;
		}
		else
		{
			paramData->funcId = ServiceManager::FuncID_Send_CommandTrafficFailure;
			memcpy(paramData->data, &failedType, sizeof(VR_TRAFFIC_COMMAND_FAILED_TYPE));
		}
		
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		
		free(pCmdBackSig);
		pCmdBackSig = NULL;
	}
	
	/**
	*	@brief
	*		VR实时路况Callback
	*	@return
	*		void
	*/
	void VRTrafficControlSig(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]VRTrafficControlSig data is NULL.\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CommandTrafficFailure;
			paramData->data = (VR_TRAFFIC_COMMAND_FAILED_TYPE*)malloc(sizeof(VR_TRAFFIC_COMMAND_FAILED_TYPE));
			*((VR_TRAFFIC_COMMAND_FAILED_TYPE*)paramData->data) = VR_TRAFFIC_COMMAND_FAILED_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		VR_COMMAND_TRAFFIC_SIG traffic_sig = *(VR_COMMAND_TRAFFIC_SIG*)data;
		if (traffic_sig == VR_COMMAND_TRAFFIC_INVALIDE)
		{
			CQ_LOG_ERROR("[ServiceManager]VRTrafficControlSig data is illegal.\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CommandTrafficFailure;
			paramData->data = (VR_TRAFFIC_COMMAND_FAILED_TYPE*)malloc(sizeof(VR_TRAFFIC_COMMAND_FAILED_TYPE));
			*((VR_TRAFFIC_COMMAND_FAILED_TYPE*)paramData->data) = VR_TRAFFIC_COMMAND_FAILED_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}

		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRTrafficControlSigAnalysis, data);
		return;
	}

	/**
	*	@brief
	*		VR实时路况命令解析与执行
	*	@return
	*		void
	*/
	void VRPassingPointSigAnalysis(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis.\n");

		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis AppState_running != getCurrentState().\n");
			if (NULL != data)
			{
			int32 reqType = *(int32*)data;
			VRPassPointRepInfo repInfo;
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->data = (VRPassPointRepInfo*)malloc(sizeof(VRPassPointRepInfo));
			paramData->funcId = ServiceManager::FuncID_Send_PassingPointFailure;
			if (reqType == VR_PASSING_POINT_DELETE_ALL)
			{
				repInfo.repType = VR_PASSING_POINT_DELETE_ALL;
				repInfo.passPointNum = INVALIDE_VALUE;
				memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis FuncID_Send_PassingPointFailure VR_PASSING_POINT_DELETE_ALL.\n");
			}
			else if (reqType == VR_PASSING_POINT_INQUIRY_NUM)
			{
				repInfo.repType = VR_PASSING_POINT_INQUIRY_NUM;
				repInfo.passPointNum = INVALIDE_VALUE;
				memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis FuncID_Send_PassingPointFailure VR_PASSING_POINT_INQUIRY_NUM.\n");
			}
			}
			return;
		}

		//BOOL isPPSucess = FALSE;
		// 构造消息参数
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		paramData->data = (VRPassPointRepInfo*)malloc(sizeof(VRPassPointRepInfo));
		VRPassPointRepInfo repInfo;

		int32 reqType = *(int32*)data;
		CQ_LOG_ERROR("[ServiceManager] VRPassingPointSigAnalysis reqType = %d.\n", reqType);
		switch (reqType)
		{
		case VR_PASSING_POINT_INQUIRY_NUM:
			// 查询途径点个数
		{
			MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
				
			// 有路线，返回查询成功消息
			if (NULL != MbApi::RouteApi_getPlan() && plan.getPointNum() > 1)
			{
				repInfo.repType = VR_PASSING_POINT_INQUIRY_NUM;
				repInfo.passPointNum = plan.getPointNum() - 2;
				CQ_LOG_ERROR("[ServiceManager] VRPassingPointSigAnalysis repInfo.repType = %d , repInfo.passPointNum = %d .\n", repInfo.repType,repInfo.passPointNum);
				paramData->funcId = ServiceManager::FuncID_Send_PassingPointSucess;
				memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);

			}
			else //无路线，返回查询失败消息
			{
				repInfo.repType = VR_PASSING_POINT_INQUIRY_NUM;
				repInfo.passPointNum = INVALIDE_VALUE;
				CQ_LOG_ERROR("[ServiceManager] VRPassingPointSigAnalysis repInfo.repType = %d , repInfo.passPointNum = %d .\n", repInfo.repType, repInfo.passPointNum);
				paramData->funcId = ServiceManager::FuncID_Send_PassingPointFailure;
				memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			}
			return;
		};
			break;
		case VR_PASSING_POINT_DELETE_ALL:
			// 删除所有途径点 重新构造RoutePlan
		{
			// 判断有无途径点
			MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
			if (plan.getPointNum() > 2)
			{

				// 先取出终点
				//const RoutePlan* plan = MbApi::RouteApi_getPlan();
				uint32 endIndex = plan.getPointNum() - 1;
				// 终点POI
				PoiFavorite endPosition;
				plan.getPoint(endIndex, endPosition);

				// 1、如果当前在算路中，无条件取消当前算路
				if (MbApi::RouteService::getInstance()->isRouting())
				{
					MbApi::RouteService::getInstance()->cancel();
					CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis MbApi::RouteService::getInstance()->cancel().\n");
				}
				// 2、如果当前在导航中，无条件取消导航
				if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState()
					|| RealNavigationState_pause == MbApi::Navi_getNavigationState())
				{
					//MbApi::Navi_reqCancelNavigation();
					MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
					*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
					MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_CANCLE, NULL, pCmdBackSig);
					free(pCmdBackSig);
					pCmdBackSig = NULL;
					CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis MbApi::Navi_reqCancelNavigation().\n");
				}
				// 3、如果当前在模拟导航中，无条件取消模拟导航
				/*if (MbApi::Navi_isInSimulation() || MbApi::Navi_isPauseSimulation())
				{
					MbApi::Navi_reqCancelSimulation();
					CQ_LOG_INFO("[ServiceManager] VRPassingPointSigAnalysis MbApi::Navi_reqCancelSimulation().\n");
				}*/

				// 构造新的route plan
				MbApi::RouteService::Plan routePlan;

				// 获取自车位置设置为起点
				//Point carPosition = (Point)MbApi::Map_getCarPosition();
				// 从DataCenter中获取自车位置
				NaviDataCenter* dc = MbApi::Session_getDataCenter();
				PoiFavorite startPosition;
				MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&dc->m_carPos, &startPosition);
				routePlan.insertPoint(0, startPosition);  // 设置起点
				CQ_LOG_INFO("[ServiceManager]routePlan set start position.\n");

				size_t pointNum = routePlan.getPointNum();    // routePlan中的所有点个数，包括起点、途经点、终点
				CQ_LOG_INFO("[ServiceManager]routePlan destNum is %d.\n", pointNum);
				if (0 == pointNum)
				{
					CQ_LOG_ERROR("[ServiceManager]:Route plan is error!\n");
				}
				routePlan.insertPoint(pointNum, endPosition);  // 设置终点
				CQ_LOG_INFO("[ServiceManager]routePlan set end position.\n");

				// 判断route plan是否有效
				if (RoutePlanErr_none != RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum()-1))
				{
					CQ_LOG_INFO("[ServiceManager]Route plan is invalid!\n");
					repInfo.repType = VR_PASSING_POINT_DELETE_ALL;
					repInfo.passPointNum = INVALIDE_VALUE;
					paramData->funcId = ServiceManager::FuncID_Send_PassingPointFailure;
					memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
					NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
					break;
				}
				// 通知Client切换画面（针对模拟导航）
				MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
				*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_START_ROUTE, NULL, pCmdBackSig);
				
				/***********************VR新算路-携带路线偏好************************/
				// 备份routePlan
				g_VRLastRoutePlan = routePlan;
				// 获取路线偏好json数据
				json_t* routePrefer = VRDataLoadFromFile();
				HybridRouteParam param;
				// 将json转换为偏好对象
				if (NULL != routePrefer)
				{
					VRGetHybridRouteType(routePrefer, &param);
				}
				// 开始算路
				VRHybridStartRoute(routePlan, param);
				// 释放json指针
				json_decref(routePrefer);
				/***********************VR新算路-携带路线偏好************************/
				
				// 开始算路
				// MbApi::RouteService::getInstance()->route(routePlan, MbApi::RouteService::ResultMode_single, MbApi::ROUTE_SERVICE_USER_ID_SERVICE_MANAGER);
				// 设置标志位
				isDeletePassPointReqPlan = TRUE;
				CQ_LOG_INFO("[ServiceManager]start route.\n");
				free(pCmdBackSig);
				pCmdBackSig = NULL;
				// 删除途径点成功
				/*repInfo.repType = VR_PASSING_POINT_DELETE_ALL;
				repInfo.passPointNum = INVALIDE_VALUE;
				paramData->funcId = ServiceManager::FuncID_Send_PassingPointSucess;
				memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);*/
			}
			else
			{
				// 无途径点返回失败
				repInfo.repType = VR_PASSING_POINT_DELETE_ALL;
				repInfo.passPointNum = INVALIDE_VALUE;
				paramData->funcId = ServiceManager::FuncID_Send_PassingPointFailure;
				memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				CQ_LOG_INFO("[ServiceManager]VRPassingPointSigAnalysis FuncID_Send_PassingPointFailure VR_PASSING_POINT_DELETE_ALL no routePlan .\n");
			}
		}
			break;
		default:
			break;
		}
	}
	
	/**
	*	@brief
	*		VR途径点Callback
	*	@return
	*		void
	*/
	void VRPassingPointSig(void* data)
	{
		if (NULL == data)
		{
			//2018.5.21 by shenghao
			//此处应添加给车机返回对应的失败消息，但此时没有适合的失败消息，暂不添加。
			CQ_LOG_ERROR("[ServiceManager]VRPassingPointSig data is NULL!");
			return;
		}
		int32 reqType = *(int32*)data;
		if (reqType == -1)
		{
			//2018.5.21 by shenghao
			//此处应添加给车机返回对应的失败消息，但此时没有适合的失败消息，暂不添加。
			CQ_LOG_ERROR("[ServiceManager]VRPassingPointSig data is illegal!");
			return;
		}
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRPassingPointSigAnalysis, data);
		return;
	}
	
	/**
	*	@brief
	*		VR路线偏好命令解析与执行
	*	@return
	*		void
	*/
	void VRSwitchRouteSigAnalysis(void* data)
	{
		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteFailure;
			paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
			*(VR_ROUTE_PLAN*)(paramData->data) = VR_ROUTE_PLAN_NONE;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		// 判断当前是否有路线
		VR_ROUTE_PLAN* RoutePlan = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
		if (MbApi::RouteApi_getRoute() != NULL)
		{
			*RoutePlan = VR_ROUTE_PLAN_EXIST;
		}
		else
		{
			*RoutePlan = VR_ROUTE_PLAN_NONE;
		}

		MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
		*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;

		//BOOL isroutePrefer = FALSE;
		
		VR_COMMAND_ROUTE_PREFER route_prefer = *(VR_COMMAND_ROUTE_PREFER*)data;
		switch (route_prefer)
		{
		case VR_COMMAND_ROUTE_AVOID_JAMS:
		case VR_COMMAND_ROUTE_AVOID_HIGH_SPEED:
		case VR_COMMAND_ROUTE_FEWER_FEES:
		case VR_COMMAND_ROUTE_FIRST_HIGH_SPEED:
		case VR_COMMAND_ROUTE_SHORTEST:
		case VR_COMMAND_ROUTE_AVOID_FERRY:
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_ROUTE_PREFERENCE_SWITCH, data, pCmdBackSig);
			break;
		default:
			break;
		}
	
		// 判断成功与失败类型
		CQ_LOG_INFO("[ServiceManager]VRSwitchRouteSigAnalysis *pCmdBackSig = %d .\n", *pCmdBackSig);
		switch (*pCmdBackSig)
		{
		case MBAPI_VR_COMMAND_DID_SUCESS_AND_JAMS:
		{
			// 已设置避让拥堵
			isAvoidJamSet = TRUE;
			if (*RoutePlan == VR_ROUTE_PLAN_EXIST)
				{
					// 需要等待客户端算路完成
					isRouteSwitchReqPlan = TRUE;
				}	
		}
			break;
		case MBAPI_VR_COMMAND_DID_SUCESS:
		{
			//isroutePrefer = TRUE;
			if (*RoutePlan == VR_ROUTE_PLAN_EXIST)
			{
				// 需要等待客户端算路完成
				isRouteSwitchReqPlan = TRUE;
				free(RoutePlan);
				RoutePlan = NULL;
			}
			else
			{
				isRouteSwitchReqPlan = FALSE;
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteSucess;
				//paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
				paramData->data = RoutePlan;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			}
		}
			break;
		case MBAPI_VR_COMMAND_SAME_WITH_CURRENT_STATE:
		{
				// 当前状态已设置，无论有无路线，参数都为无路线
				isRouteSwitchReqPlan = FALSE;
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteSucess;
				paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
				*(VR_ROUTE_PLAN*)paramData->data = VR_ROUTE_PLAN_NONE;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				free(RoutePlan);
				RoutePlan = NULL;
		}			
			break;
		case MBAPI_VR_COMMAND_DID_FAILURE:
		case MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT:
		{
				isRouteSwitchReqPlan = FALSE;
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteFailure;
				//paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
				paramData->data = RoutePlan;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}
			
			break;
		default:
			break;
		}

		free(pCmdBackSig);
		pCmdBackSig = NULL;

		return;
	}
	/**
	*	@brief
	*		VR路线偏好Callback
	*	@return
	*		void
	*/
	void VRSwitchRouteSig(void* data)
	{
		//判断当前有无路线
		VR_ROUTE_PLAN* RoutePlan = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
		if (MbApi::RouteApi_getRoute() != NULL)
		{
			*RoutePlan = VR_ROUTE_PLAN_EXIST;
		}
		else
		{
			*RoutePlan = VR_ROUTE_PLAN_NONE;
		}

		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]VRSwitchRouteSig data is NULL.\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteFailure;
			//paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
			paramData->data = RoutePlan;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}

		VR_COMMAND_ROUTE_PREFER route_prefer = *(VR_COMMAND_ROUTE_PREFER*)data;

		if (route_prefer == VR_COMMAND_ROUTE_PREFER_INVALIDE)
		{
			CQ_LOG_ERROR("[ServiceManager]VRSwitchRouteSig data is illegal.\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteFailure;
			//paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
			paramData->data = RoutePlan;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		if (NULL != RoutePlan)
		{
			free(RoutePlan);
			RoutePlan = NULL;
		}			
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRSwitchRouteSigAnalysis, data);
		return;
	}


	void VRItemSelectSigAnalysis(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRItemSelectSigAnalysis.\n");

		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_ItemSelectFailure;
			paramData->data = (VR_ITEM_SELECT*)malloc(sizeof(VR_ITEM_SELECT));
			*((VR_ITEM_SELECT*)paramData->data) = VR_ITEM_SELECT_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_INFO("[ServiceManager]VRItemSelectSigAnalysis AppState_running != getCurrentState().\n");
			return;
		}

		//BOOL isItemSelectSucess = FALSE;
		VRPoiInfo* poi = (VRPoiInfo*)data;
			
		// 解密poi经纬度
		poi->lat = VRDecode(poi->lat);
		poi->lon = VRDecode(poi->lon);

		CQ_LOG_INFO("[ServiceManager] VRItemSelectSigAnalysis VRPoiInfo.lon = %d, VRPoiInfo.lat = %d.\n", poi->lon, poi->lat);
			
		// 判断经纬度是否有效
		if (poi->lat == 0 && poi->lon == 0 || *(poi->name) == '\0')
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_ItemSelectFailure;
			paramData->data = (VR_ITEM_SELECT*)malloc(sizeof(VR_ITEM_SELECT));
			*((VR_ITEM_SELECT*)paramData->data) = VR_ITEM_SELECT_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}

		// 1、如果当前在算路中，无条件取消当前算路
		if (MbApi::RouteService::getInstance()->isRouting())
		{
			MbApi::RouteService::getInstance()->cancel();
			CQ_LOG_INFO("[ServiceManager] VRItemSelectSigAnalysis MbApi::RouteService::getInstance()->cancel().\n");
		}
		// 2、如果当前在导航中，无条件取消导航
		if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState()
			|| RealNavigationState_pause == MbApi::Navi_getNavigationState())
		{
			//MbApi::Navi_reqCancelNavigation();
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_CANCLE, NULL, pCmdBackSig);
			free(pCmdBackSig);
			pCmdBackSig = NULL;
			CQ_LOG_INFO("[ServiceManager] VRItemSelectSigAnalysis MbApi::Navi_reqCancelNavigation().\n");
		}
		// 3、如果当前在模拟导航中，无条件取消模拟导航
		/*if (MbApi::Navi_isInSimulation() || MbApi::Navi_isPauseSimulation())
		{
			MbApi::Navi_reqCancelSimulation();
			CQ_LOG_INFO("[ServiceManager] VRItemSelectSigAnalysis MbApi::Navi_reqCancelSimulation().\n");
		}*/

		// 构造route plan
		MbApi::RouteService::Plan routePlan;

		// 获取自车位置设置为起点				
		// 从DataCenter中获取自车位置
		//Point carPosition = (Point)MbApi::Map_getCarPosition();
		NaviDataCenter* dc = MbApi::Session_getDataCenter();
		PoiFavorite startPosition;
		MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&dc->m_carPos, &startPosition);
		routePlan.insertPoint(0, startPosition);  // 设置起点
		CQ_LOG_INFO("[ServiceManager]routePlan set start position.\n");
			
		// 构造终点,解密poi 经纬度			
		Point endPoint;
		endPoint.set(poi->lon, poi->lat);
		PoiFavorite endPosition;
		MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&endPoint, &endPosition);

		//TYTAVNNEW-1542、Bug #2491 
		memset(endPosition.name, 0, sizeof(endPosition.name));
		cq_decodeUtf8((const cqCHAR*)poi->name, SIZE_T_MAX, (cqWCHAR*)endPosition.name, 128);

		routePlan.insertPoint(1, endPosition);  // 设置终点
		CQ_LOG_INFO("[ServiceManager]routePlan set end position.\n");

		//答应wchar汉字的十六进制
		//cqWCHAR geo_addr_utf8[128] = { 0 };
		//for (int i = 0; i < cq_wcslen(endPosition.name); i++)
		//{
		//	geo_addr_utf8[i] = endPosition.name[i];
		//}

		//for (int i = 0; i < cq_wcslen(geo_addr_utf8); i++)
		//{
		//	printf("0x%02X,", geo_addr_utf8[i]);
		//}printf("\n");
		
		// 判断route plan是否有效
		if (RoutePlanErr_none == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
		{
			// 通知Client切换画面（针对模拟导航）
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_START_ROUTE, NULL, pCmdBackSig);
			free(pCmdBackSig);
			pCmdBackSig = NULL;
			/***********************VR新算路-携带路线偏好************************/
			// 备份routePlan
			g_VRLastRoutePlan = routePlan;
			// 获取路线偏好json数据
			json_t* routePrefer = VRDataLoadFromFile();
			HybridRouteParam param;
			// 将json转换为偏好对象
			if (NULL != routePrefer)
			{
				VRGetHybridRouteType(routePrefer, &param);
			}
			// 设置开始导航Speaker标识
			MbApi::Navi_notifyNaviBeginSpeakInfo();
			// 开始算路，同时开始导航
			VRHybridStartRoute(routePlan, param);
			// 释放json指针
			json_decref(routePrefer);
			/***********************VR新算路-携带路线偏好************************/
			//MbApi::RouteService::getInstance()->route(routePlan, MbApi::RouteService::ResultMode_single, MbApi::ROUTE_SERVICE_USER_ID_SERVICE_MANAGER);
			CQ_LOG_INFO("[ServiceManager]start route.\n");
			
			isItemSeletReqPlan = TRUE;
			return;
		}
		else if (RoutePlanErr_endStartTooNear == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_ItemSelectFailure;
			paramData->data = (VR_ITEM_SELECT*)malloc(sizeof(VR_ITEM_SELECT));
			*((VR_ITEM_SELECT*)paramData->data) = VR_ITEM_SELECT_TOO_CLOSE;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		else
		{
			CQ_LOG_INFO("[ServiceManager]Route plan is invalid!\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_ItemSelectFailure;
			paramData->data = (VR_ITEM_SELECT*)malloc(sizeof(VR_ITEM_SELECT));
			*((VR_ITEM_SELECT*)paramData->data) = VR_ITEM_SELECT_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			return;
		}
		return;

	}

	/**
	*	@brief
	*		VR ITEM选择Callback
	*	@return
	*		void
	*/
	void VRItemSelectSig(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]VRItemSelectSig data is NULL.\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_ItemSelectFailure;
			paramData->data = (VR_ITEM_SELECT*)malloc(sizeof(VR_ITEM_SELECT));
			*((VR_ITEM_SELECT*)paramData->data) = VR_ITEM_SELECT_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_INFO("[ServiceManager]VRItemSelectSigAnalysis AppState_running != getCurrentState().\n"); 
			return;
		}
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRItemSelectSigAnalysis, data);
		return;
	}
	
		/**
	*	@brief
	*		VR查询距离命令解析与执行
	*	@return
	*		void
	*/
	void VRInquiryDistanceSigAnalysis(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRInquiryDistanceSigAnalysis start!\n");
		UNUSED_VAR(data);
		SendData* paramData = NULL;
		paramData = (SendData*)malloc(sizeof(SendData));
		memset(paramData, 0, sizeof(SendData));
		//判断当前是否有路线 没有路线返回失败
		if (MbApi::RouteApi_getRoute() == NULL)
		{
			CQ_LOG_ERROR("[ServiceManager] RouteApi_getRoute is NULL!\n");
			paramData->funcId = ServiceManager::FuncID_Send_InquiryDistanceFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			return;
		}
		// 向引擎请求前先判断引擎状态，防止出现引擎已开始析构，请求还在继续的错误出现
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			CQ_LOG_ERROR("[ServiceManager] AppState  is not running!\n");
			paramData->funcId = ServiceManager::FuncID_Send_InquiryDistanceFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			return;
		}
		//获取当前的导航状态数据
		const NaviSessionData* naviData = MbApi::Navi_getNaviData();
		InquiryDistance* m_inquiry = NULL;

		m_inquiry = (InquiryDistance*)malloc(sizeof(InquiryDistance));
		if (0 == naviData->m_travelledDistance)
			m_inquiry->remainingTime = MbApi::RouteBaseApi_getEstimatedTime(MbApi::RouteApi_getRoute());
		else
			m_inquiry->remainingTime = MbApi::Eta_getRemainingTime();
			m_inquiry->remainingDistance = (naviData->m_routeLength - naviData->m_travelledDistance);

		//发送消息
		paramData->funcId = ServiceManager::FuncID_Send_InquiryDistanceSuccess;
		//paramData->data = (InquiryDistance*)malloc(sizeof(InquiryDistance));
		paramData->data = m_inquiry;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}

	/**
	*	@brief
	*		VR查询距离Callback
	*	@return
	*		void
	*/
	void VRInquiryDistanceSig(void* data)
	{
		UNUSED_VAR(data);
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRInquiryDistanceSigAnalysis, NULL);
		return;
	}
/**
	*	@brief
	*		VR导航到指定途径点的目的地命令解析与执行
	*	@return
	*		void
	*/
	void VRPassingPoiAndDestPoiSigAnalysis(void* data)
	{
		CQ_LOG_ERROR("[ServiceManager] VRPassingPointAndDestPointSigAnalysis.\n");

		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
			paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
			*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_INFO("[ServiceManager] VRPassingPointAndDestPointSigAnalysis >getCurrentState() != AppState_running.\n");
			return;
		}

		if (NULL != data)
		{
			//VRPoiInfo *poiInfo[2];
			VRViaAndDestPoiInfo *poiInfo = (VRViaAndDestPoiInfo*)data;
			
			// 解密poi经纬度
			poiInfo->viaLat = VRDecode(poiInfo->viaLat);
			poiInfo->viaLon = VRDecode(poiInfo->viaLon);
			poiInfo->destLat = VRDecode(poiInfo->destLat);
			poiInfo->destLon = VRDecode(poiInfo->destLon);

			CQ_LOG_INFO("[ServiceManager] VRPassingPoiAndDestPoiSigAnalysis poiInfo.viaLon = %d, poiInfo.viaLat = %d, poiInfo.viaName = %s \n poiInfo.destLon = %d, poiInfo.destLat = %d ",
				poiInfo->viaLon, poiInfo->viaLat,poiInfo->viaName, poiInfo->destLon, poiInfo->destLat, poiInfo->destName);

			// 判断经纬度是否有效
			if ((poiInfo->viaLat == 0 && poiInfo->viaLon == 0) || (poiInfo->destLat == 0 && poiInfo->destLon == 0))
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
				paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
				*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_OTHER;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				return;
			}

			// 1、如果当前在算路中，无条件取消当前算路
			if (MbApi::RouteService::getInstance()->isRouting())
			{
				MbApi::RouteService::getInstance()->cancel();
				CQ_LOG_INFO("[ServiceManager] VRPassingPoiAndDestPoiSigAnalysis MbApi::RouteService::getInstance()->cancel().\n");
			}
			// 2、如果当前在导航中，无条件取消导航
			if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState()
				|| RealNavigationState_pause == MbApi::Navi_getNavigationState())
			{
				//MbApi::Navi_reqCancelNavigation();
				MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
				*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_CANCLE, NULL, pCmdBackSig);
				free(pCmdBackSig);
				pCmdBackSig = NULL;
				CQ_LOG_INFO("[ServiceManager] VRPassingPoiAndDestPoiSigAnalysis MbApi::Navi_reqCancelNavigation().\n");
			}
			// 3、如果当前在模拟导航中，无条件取消模拟导航
			/*if (MbApi::Navi_isInSimulation() || MbApi::Navi_isPauseSimulation())
			{
				MbApi::Navi_reqCancelSimulation();
				CQ_LOG_INFO("[ServiceManager] VRPassingPoiAndDestPoiSigAnalysis MbApi::Navi_reqCancelSimulation().\n");
			}*/

			// 构造route plan
			MbApi::RouteService::Plan routePlan;

			// 获取自车位置设置为起点
			//Point carPosition = (Point)MbApi::Map_getCarPosition();
			// 从DataCenter中获取自车位置
			NaviDataCenter* dc = MbApi::Session_getDataCenter();
			PoiFavorite startPosition;
			MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&dc->m_carPos, &startPosition);
			routePlan.insertPoint(0, startPosition);  // 设置起点
			CQ_LOG_INFO("[ServiceManager]routePlan set start position.\n");

			// 先构造终点和起点的因为RoutePlanApi_checkRoutePlan() check时  检测到终点时是只检查和前一个点的距离
			//中间有途经点的话就不能判断出和起点过近的这种情况。
			// 构造终点 解密经纬度
			Point endPoint;
			endPoint.set(poiInfo->destLon, poiInfo->destLat);
			PoiFavorite endPosition;
			MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&endPoint, &endPosition);

			//TYTAVNNEW-1542、Bug #2491 
			memset(endPosition.name, 0, sizeof(endPosition.name));
			cq_decodeUtf8((const cqCHAR*)poiInfo->destName, SIZE_T_MAX, (cqWCHAR*)endPosition.name, 128);

			routePlan.insertPoint(1, endPosition);  // 设置终点
			CQ_LOG_INFO("[ServiceManager]routePlan set end position.\n");
			if (RoutePlanErr_none != RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
			{
				if (RoutePlanErr_endStartTooNear == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
				{
					ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
					paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
					paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
					*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_TOO_CLOSE;
					NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
					return;
				}
				else
				{
					ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
					paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
					paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
					*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_OTHER;
					NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
					return;
				}

			}

			// 构造途径点 解密经纬度
			Point viaPoint;
			viaPoint.set(poiInfo->viaLon, poiInfo->viaLat);
			PoiFavorite viaPosition;
			MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&viaPoint, &viaPosition);

			//TYTAVNNEW-1542、Bug #2491 
			memset(viaPosition.name, 0, sizeof(viaPosition.name));
			cq_decodeUtf8((const cqCHAR*)poiInfo->viaName, SIZE_T_MAX, (cqWCHAR*)viaPosition.name, 128);

			routePlan.insertPoint(1, viaPosition);  // 设置途径点

			// 判断route plan是否有效
			if (RoutePlanErr_none == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 2) &&
					RoutePlanErr_none == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
			{
				// 通知Client切换画面（针对模拟导航）
				MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
				*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
				MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_START_ROUTE, NULL, pCmdBackSig);
				free(pCmdBackSig);
				pCmdBackSig = NULL;
				/***********************VR新算路-携带路线偏好************************/
				// 备份routePlan
				g_VRLastRoutePlan = routePlan;
				// 获取路线偏好json数据
				json_t* routePrefer = VRDataLoadFromFile();
				HybridRouteParam param;
				// 将json转换为偏好对象
				if (NULL != routePrefer)
				{
					VRGetHybridRouteType(routePrefer, &param);
				}				
				// 设置开始导航Speaker标识
				MbApi::Navi_notifyNaviBeginSpeakInfo();
				// 开始算路
				VRHybridStartRoute(routePlan, param);
				// 释放json指针
				json_decref(routePrefer);
				/***********************VR新算路-携带路线偏好************************/

				// 开始算路，同时开始导航 
				// MbApi::RouteService::getInstance()->route(routePlan, MbApi::RouteService::ResultMode_single, MbApi::ROUTE_SERVICE_USER_ID_SERVICE_MANAGER);
				CQ_LOG_INFO("[ServiceManager]start route.\n");
				isPassAndDestReqPlan = TRUE;
			}
			else if (RoutePlanErr_endViaTooNear == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 2) ||
						RoutePlanErr_viaStartTooNear == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 2) ||
						RoutePlanErr_viaEndTooNear == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 2))
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
				paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
				*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_TOO_CLOSE;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			}
			else
			{
				CQ_LOG_INFO("[ServiceManager]Route plan is invalid!\n");
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
				paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
				*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_OTHER;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			}
		}
		else
		{
			CQ_LOG_ERROR("[ServiceManager]data == NULL!\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_PassingPoiAndDestPoiFailure;
			paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
			*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}
		return;
	}


	/**
	*	@brief
	*		VR导航到指定途径点的目的地Callback
	*	@return
	*		void
	*/
	void VRPassingPoiAndDestPoiSig(void* data)
	{
		CQ_LOG_ERROR("[ServiceManager] VRPassingPointAndDestPointSig.\n");
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRPassingPoiAndDestPoiSigAnalysis, data);
		return;
	}

	/**
	*	@brief
	*		VR回家/公司
	*	@return
	*		void
	*/

	void VRCollectionSigAnalysis(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis().\n");

		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis()  getCurrentState() != AppState_running.\n");
			return;
		}

		IDX_INFO index = *(IDX_INFO*)data;

		// 判断是否有对应家和公司信息
		if (!((index == IDX_HOME && MbApi::NaviShortcuts_isValid(IDX_HOME)) || (index == IDX_OFFICE && MbApi::NaviShortcuts_isValid(IDX_OFFICE))))
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis() Home or Office is NULL.\n");
			return;
		}

		// 1、如果当前在算路中，无条件取消当前算路
		if (MbApi::RouteService::getInstance()->isRouting())
		{
			MbApi::RouteService::getInstance()->cancel();
			CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis() MbApi::RouteService::getInstance()->cancel().\n");
		}
		// 2、如果当前在导航中，无条件取消导航
		if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState()
			|| RealNavigationState_pause == MbApi::Navi_getNavigationState())
		{
			//MbApi::Navi_reqCancelNavigation();
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_CANCLE, NULL, pCmdBackSig);
			free(pCmdBackSig);
			pCmdBackSig = NULL;
			CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis() MbApi::Navi_reqCancelNavigation().\n");
		}
		// 3、如果当前在模拟导航中，无条件取消模拟导航
		/*if (MbApi::Navi_isInSimulation() || MbApi::Navi_isPauseSimulation())
		{
			MbApi::Navi_reqCancelSimulation();
			CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis() MbApi::Navi_reqCancelSimulation().\n");
		}*/

		// 构造route plan
		MbApi::RouteService::Plan routePlan;
		NaviDataCenter* dc = MbApi::Session_getDataCenter();

		// 获取自车位置设置为起点
		PoiFavorite startPosition;
		MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&dc->m_carPos, &startPosition);
		routePlan.insertPoint(0, startPosition);  // 设置起点

		// 获取目的地设置为终点
		PoiFavorite* endPosition = MbApi::NaviShortcuts_getFav(index);
		if (NULL == endPosition)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_ERROR("[ServiceManager] VRCollectionSigAnalysis() endPosition is NULL.\n");
			return;
		}

		routePlan.insertPoint(1, *endPosition);  // 设置终点
		CQ_LOG_INFO("[ServiceManager]VRCollectionSigAnalysis() routePlan set end position.\n");

		// 判断route plan是否有效
		if (RoutePlanErr_none == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
		{
			// 通知Client切换画面（针对模拟导航）
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_START_ROUTE, NULL, pCmdBackSig);
			free(pCmdBackSig);
			pCmdBackSig = NULL;
			/***********************VR新算路-携带路线偏好************************/
			// 备份routePlan
			g_VRLastRoutePlan = routePlan;
			// 获取路线偏好json数据
			json_t* routePrefer = VRDataLoadFromFile();
			HybridRouteParam param;
			// 将json转换为偏好对象
			if (NULL != routePrefer)
			{
				VRGetHybridRouteType(routePrefer, &param);
			}
			// 设置开始导航Speaker标识
			MbApi::Navi_notifyNaviBeginSpeakInfo();
			// 开始算路
			VRHybridStartRoute(routePlan, param);
			// 释放json指针
			json_decref(routePrefer);
			/***********************VR新算路-携带路线偏好************************/
						
			// 开始算路，同时开始导航 
			// MbApi::RouteService::getInstance()->route(routePlan, MbApi::RouteService::ResultMode_single, MbApi::ROUTE_SERVICE_USER_ID_SERVICE_MANAGER);
			CQ_LOG_INFO("[ServiceManager] VRCollectionSigAnalysis() start route.\n");
			isCollectionReqPlan = TRUE;
			free(pCmdBackSig);
			pCmdBackSig = NULL;
		}
		else if (RoutePlanErr_endStartTooNear == RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), routePlan.getPointNum() - 1))
		{
			CQ_LOG_ERROR("[ServiceManager] VRCollectionSigAnalysis() Route plan is invalid!\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_TOO_CLOSE;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}
		else
		{
			CQ_LOG_ERROR("[ServiceManager] VRCollectionSigAnalysis() Route plan is invalid!\n");
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		}
		return;
	}

	/**
	*	@brief
	*		VR回家/公司Callback
	*	@return
	*		void
	*/
	void VRCollectionSig(void* data)
	{
		CQ_LOG_ERROR("[ServiceManager] VRCollectionSig() start.\n");

		if (NULL == data)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_ERROR("[ServiceManager] VRCollectionSigAnalysis() data is NULL .\n");
			return;
		}
		int32 collection = *(int32*)data;
		if (collection == -1)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_CollectionFailure;
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			CQ_LOG_ERROR("[ServiceManager] VRCollectionSigAnalysis() collection is -1.\n");
			return;		
		}
		else
		{
			MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRCollectionSigAnalysis, data);
		}
		
		return;
	}

	/**
	*	@brief
	*		VR开始导航命令解析与执行
	*	@return
	*		void
	*/

	void VRStartNaviSigAnalysis(void* data)
	{
		if (NULL != data)
		{
			free(data);
			data = NULL;
		}
		bool isStartNaviSucess = false;
		if (MbApi::AppService::AppState_running == MbApi::AppService::getInstance()->getCurrentState())
		{			
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT;
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_START, NULL, pCmdBackSig);

			switch (*pCmdBackSig)
			{
			case MBAPI_VR_COMMAND_DID_SUCESS:
				isStartNaviSucess = true;
				break;
			case MBAPI_VR_COMMAND_DID_FAILURE:
			case MBAPI_VR_COMMAND_CURRENT_PAGE_NOT_SUPPORT:
				isStartNaviSucess = false;
				break;
			default:
				break;
			}
			free(pCmdBackSig);
			pCmdBackSig = NULL;
		}
			
		// 通知车机本次请求结果
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (isStartNaviSucess)
		{
			paramData->funcId = ServiceManager::FuncID_Send_StartNaviSuccess;
		}
		else
		{
			paramData->funcId = ServiceManager::FuncID_Send_StartNaviFailure;
		}
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);	
		return;		
	}


	/**
	*	@brief
	*		接收VR开始导航Callback
	*	@return
	*		void
	*/
	void VRStartNaviSig(void* data)
	{
		CQ_LOG_ERROR("[ServiceManager] VRStartNaviSig() start.\n");
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRStartNaviSigAnalysis, data);
		return;
	}
	/**
	*	@brief
	*		路径规划成功
	*	@param [in] data
	*		data 参数是一个 RouteCollection*，其中保存着计算得到的路线结果(可能有不止1条结果)。
	*	@return
	*		void
	*/
	static void VRServiceClient_complete(void* data)
	{
		UNUSED_VAR(data);
		CQ_LOG_INFO("[ServiceManager] VRServiceClient_complete.\n");
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		// 判断请求方，返回对应结果
		if (isDeletePassPointReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_complete isDeletePassPointReqPlan = TRUE.\n");
			VRPassPointRepInfo repInfo;
			repInfo.repType = VR_PASSING_POINT_DELETE_ALL;
			repInfo.passPointNum = INVALIDE_VALUE;
			paramData->funcId = ServiceManager::FuncID_Send_PassingPointSucess;
			paramData->data = (VRPassPointRepInfo*)malloc(sizeof(VRPassPointRepInfo));
			memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isDeletePassPointReqPlan = FALSE;
		}
		else if (isItemSeletReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_complete isItemSeletReqPlan = TRUE.\n");
			paramData->funcId = FuncID_Send_ItemSelectSucess;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isItemSeletReqPlan = FALSE;
		}
		else if (isPassAndDestReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_complete isPassAndDestReqPlan = TRUE.\n");
			paramData->funcId = FuncID_Send_PassingPoiAndDestPoiSuccess;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isPassAndDestReqPlan = FALSE;
		}
		else if (isCollectionReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_complete isCollectionReqPlan = TRUE.\n");
			paramData->funcId = FuncID_Send_CollectionSuccess;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isCollectionReqPlan = FALSE;
		}		
		return;
	}

	/**
	*	@brief
	*		路径规划失败
	*	@param [in] data
	*		data 参数是一个 RouterErrorInfo*，其中保存着更详细的错误信息。
	*	@return
	*		void
	*/
	static void VRServiceClient_failed(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] VRServiceClient_failed.\n");
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		UNUSED_VAR(data);
		if (isDeletePassPointReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_failed isDeletePassPointReqPlan = TRUE.\n");
			VRPassPointRepInfo repInfo;
			repInfo.repType = VR_PASSING_POINT_DELETE_ALL;
			repInfo.passPointNum = INVALIDE_VALUE;
			paramData->funcId = ServiceManager::FuncID_Send_PassingPointFailure;
			paramData->data = (VRPassPointRepInfo*)malloc(sizeof(VRPassPointRepInfo));
			memcpy(paramData->data, &repInfo, sizeof(VRPassPointRepInfo));
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isDeletePassPointReqPlan = FALSE;
		}
		else if (isItemSeletReqPlan)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_ItemSelectFailure;
			paramData->data = (VR_ITEM_SELECT*)malloc(sizeof(VR_ITEM_SELECT));
			*((VR_ITEM_SELECT*)paramData->data) = VR_ITEM_SELECT_OTHER;
			NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			isItemSeletReqPlan = FALSE;
		}
		else if (isPassAndDestReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_failed isPassAndDestReqPlan = TRUE.\n");
			paramData->funcId = FuncID_Send_PassingPoiAndDestPoiFailure;
			paramData->data = (VR_PASSINGPOI_FAILED_TYPE*)malloc(sizeof(VR_PASSINGPOI_FAILED_TYPE));
			*((VR_PASSINGPOI_FAILED_TYPE*)paramData->data) = VR_PASSINGPOI_OTHER;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isPassAndDestReqPlan = FALSE;
		}
		else if (isCollectionReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_failed isCollectionReqPlan = TRUE.\n");
			paramData->funcId = FuncID_Send_CollectionFailure; 
			paramData->data = (VR_COLLECTION_FAILED_TYPE*)malloc(sizeof(VR_COLLECTION_FAILED_TYPE));
			*((VR_COLLECTION_FAILED_TYPE*)paramData->data) = VR_COLLECTION_OTHER;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isCollectionReqPlan = FALSE;
		}
		else if (isRouteSwitchReqPlan)
		{
			CQ_LOG_INFO("[ServiceManager] VRServiceClient_failed isRouteSwitchReqPlan = TRUE.\n");
			paramData->funcId = FuncID_Send_SwitchRouteFailure;
			paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
			*(VR_ROUTE_PLAN*)(paramData->data) = VR_ROUTE_PLAN_NONE;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			isRouteSwitchReqPlan = FALSE;
		}
		
		return;
	}

	/**
	*	@brief
	*		构造函数
	*/
	VRServiceClient::VRServiceClient(){}
	/**
	*	@brief
	*		析构函数
	*/
	VRServiceClient::~VRServiceClient(){}
	
	/**
	*	@brief
	*		initialize
	*/
	void VRServiceClient::initialize()
	{
		MbApi::MessageDistribution::getInstance()->addNaviSessionEventListener(MbApi::MessageDistribution::Level_medium, this);
		return;
	}

	BOOL VRServiceClient::onNaviSessionEventNotify(MbApi::MessageDistribution::NaviSessionEvent event, void* data)
	{
		switch (event)
		{
		case MbApi::MessageDistribution::NaviSessionEvent_sm_routeComplete:
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_routeComplete.\n", __FUNCTION__, __LINE__);
			VRServiceClient_complete(data);
			break;
		case MbApi::MessageDistribution::NaviSessionEvent_sm_routeFailed:
		//case MbApi::MessageDistribution::NaviSessionEvent_routeCancelled:
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_routeFailed.\n", __FUNCTION__, __LINE__);
			// 如果为在线算路失败、并且避让拥堵已经设置，抛弃本次失败结果，再次发起离线算路
			switch (((RouterErrorInfo*)data)->errCode)
			{
			case ERouterError_NetworkError:
			case ERouterError_onlineCalcTimeout:
			{
				json_t* routePrefer = VRDataLoadFromFile();
				HybridRouteParam routeParam;
				// 将json转换为偏好对象
				if (NULL != routePrefer)
				{
					VRGetHybridRouteType(routePrefer, &routeParam);
				}
				if (routeParam.operator[](HybridRouteType_avoidJam))
				{
					routeParam.setRouteParam(MbApi::HybridRouteType_avoidJam, FALSE);
					// 设置开始导航Speaker标识
					MbApi::Navi_notifyNaviBeginSpeakInfo();
					// 开始算路
					VRHybridStartRoute(g_VRLastRoutePlan, routeParam);
					// 释放json指针	
					json_decref(routePrefer);
					return true;
				}
				json_decref(routePrefer);
			}
			default:
				break;
			}
			// 通知车机执行失败结果	
			VRServiceClient_failed(data);
			break;
		// 路线被取消
		case MbApi::MessageDistribution::NaviSessionEvent_routeCancelled:
			VRServiceClient_failed(data);
			break;

		case MbApi::MessageDistribution::NaviSessionEvent_routeComplete:
		{
			if (TRUE == isRouteSwitchReqPlan)
			{
				isRouteSwitchReqPlan = FALSE;
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteSucess;
				paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
				*(VR_ROUTE_PLAN*)(paramData->data) = VR_ROUTE_PLAN_EXIST;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);																	   
			}
		}
			break;
		case MbApi::MessageDistribution::NaviSessionEvent_routeFailed:
		{
			if (MbApi::RouteService::getInstance()->isRouting())
			 {
				return true;
			 }
			
			if (TRUE == isRouteSwitchReqPlan)
			{
				isRouteSwitchReqPlan = FALSE;
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->funcId = ServiceManager::FuncID_Send_SwitchRouteFailure;
				paramData->data = (VR_ROUTE_PLAN*)malloc(sizeof(VR_ROUTE_PLAN));
				*(VR_ROUTE_PLAN*)(paramData->data) = VR_ROUTE_PLAN_NONE;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
			}
		}
			break;
		default:
			break;
		}

		return true;
	}	
	
	static struct VRShmData shm_buff;
	static PoiKeywordQuery * keyWordSearchObjP = NULL;
	static PoiNearbyQuery * nearbySearchObjP = NULL;
	static PoiRouteQuery * routeSearchObjP = NULL;
	static PoiSuggestQueryResult* pKeyWordSuggestResultObj;
	static enum VR_SEEARCH_STATUS vrSearchStatus = VR_SEARCH_NONE;
	#ifdef SERCH_NESTED_ABORT_PRE
	//static bool isNestNeedResp = true;
	static bool isNearbyPoiNest = false;
	#endif
	//static cqWCHAR poiType[20] = {0};
	cqWCHAR * PoiTypeP = NULL;
	
	int8_t doVRNearbySearchSigAnalysis(const cqWCHAR* data,Point * point);
	
	
	const VRSearchNearbyTypeMap typeRemap[] = {
		{ L"加油站", 		L"加油站" },	/* 0 */
		{ L"停车场", 		L"停车场" },
		{ L"公厕", 			L"公共厕所" },
		{ L"美食",			L"餐饮" },
		{ L"银行",			L"银行" },
		//{ L"购物", 			L"购物" },						/* 5 */
		{ L"医院",			L"综合医院" },
		{ L"酒店", 			L"星级宾馆" },
		{ L"维修保养", 		L"汽车维护与保养" },
		{ L"中石油", 		L"中国石油" },
		{ L"中石化", 		L"中国石化" },					/* 10 */
		{ L"服务区", 		L"高速服务区" },
		{ L"汽车站", 		L"客运汽车站" },
		{ L"火车站", 		L"火车站" },
		{ L"飞机场", 		L"机场" },
		{ L"快餐", 			L"快餐" },						/* 15 */
		{ L"火锅", 			L"火锅/涮肉" },
		{ L"中餐", 			L"中餐馆" },
		{ L"西餐", 			L"异国风味" },
		{ L"川菜", 			L"川渝菜" },
		{ L"日本料理", 		L"日本料理" },					/* 20 */
		{ L"韩国料理", 		L"韩国料理" },
		{ L"咖啡馆", 		L"咖啡馆" },
		//{ L"地方特色", 		L"地方家常菜" },
		{ L"快捷酒店", 		L"招待所" },
		{ L"旅馆", 			L"招待所" },					/* 25 */
		{ L"汉庭", 			L"汉庭酒店" },
		{ L"如家", 			L"如家酒店" },
		{ L"7天", 			L"7天连锁酒店" },
		//{ L"速8",	 		L"速8酒店" },
		//{ L"汉庭酒店", 		L"汉庭酒店" },					/* 30 */
		//{ L"如家酒店", 		L"如家酒店" },
		//{ L"7天酒店", 		L"7天连锁酒店" },
		//{ L"速8酒店",	 	L"速8酒店" },
		{ L"三星级", 		L"三星级宾馆" },
		{ L"四星级", 		L"四星级酒店" },				/* 35 */
		{ L"五星级", 		L"五星级酒店" },
		{ L"名胜古迹", 		L"景点" },
		{ L"游乐园", 		L"游乐园" },
		{ L"动物园", 		L"动物园" },
		{ L"博物馆", 		L"展览馆" },					/* 40 */
		{ L"公园广场", 		L"公园;街心公园;广场" },
		{ L"商场", 			L"百货商城" },
		{ L"商城", 			L"百货商城" },
		{ L"超市", 			L"超市" },
		{ L"便利店", 		L"便利店" },
		{ L"家具建材", 		L"家具" },						/* 45 */
		//{ L"家居建材", 		L"家具" },
		//{ L"家具用品", 		L"家具" },
		{ L"电子电器", 		L"家用电器及电子产品零售" },
		{ L"家居用品", 		L"家居用品" },
		{ L"母婴用品", 		L"母婴用品、儿童用品" },		/* 50 */
		{ L"书店", 			L"书店" },
		{ L"花店", 			L"花卉专卖店" },
		//{ L"古玩字画", 		L"古玩字画店" },
		{ L"电影院", 		L"电影院" },
		{ L"健身", 			L"健身房" },					/* 55 */
		{ L"剧院", 			L"剧场、戏院、音乐厅" },
		{ L"洗浴", 			L"洗浴中心、SPA、足浴" },
		{ L"酒吧", 			L"酒吧" },
		{ L"夜店", 			L"夜总会" },
		{ L"运动场馆", 		L"运动场馆" },					/* 60 */
		{ L"网吧", 			L"网吧" },
		//{ L"彩票销售", 		L"彩票销售" },
		{ L"ATM", 			L"atm" },
		{ L"药店", 			L"药店" },
		//{ L"快递", 			L"物流" },						/* 65 */
		{ L"美容美发", 		L"美容院、理发店" },
		{ L"手机营业厅",	L"电信" },
		{ L"宠物服务", 		L"宠物及宠物用品专卖店" },
		{ L"邮局", 			L"邮局" },
		{ L"KTV", 			L"练歌房" },					/* 70 */
		{ L"度假村", 		L"度假疗养" },
		{ L"4S店", 			L"4s店" }
	};
	
	/**
	* @brief        getIndexOfTypeDate 获取引擎识别的关键字编码索引
	* @date         2018 03.05
	* @param[in]    周边搜索的关键字
	*/
	
	const cqWCHAR * getIndexOfTypeDate(cqWCHAR * data)
	{
		if( data == NULL)
		{
			return NULL;
		}
		int i = 0;
		for(i = 0; i < sizeof(typeRemap)/sizeof(typeRemap[0]);i++)
		{
			//if( memcmp((void *)data,(void *)typeRemap[i].nearbyType,strlen((char *)data )) == 0)
			if(cq_wcscmp(data, typeRemap[i].nearbyType ) == 0)
			{
				SVC_MGE_LOG_INFO("[ServiceManager] poitype index is [%d] data addr is [%x]\n",i,typeRemap[i].nearbyMapType);
				return typeRemap[i].nearbyMapType;
			}
		}
		return NULL;
	}
	
	class SmClientCallback : public MbApi:: PoiQueryListener
	{
	public:
		
		/**
		* @brief 查询结果获取回调处理
		* @date  2018.2.6
		* @param [in]   query
		* @param [in]   event
		* @param [in]   error
		*/
		virtual void onPoiQueryEventNotify(PoiQuery* query, PoiSrvQueryEvent event, PoiSrvQueryError error)
		{
			static Point point_temp ;
			GeneralPoiSuggestObject obj;
			size_t suggest_num = 0,index = 0;
			
			SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
			if (error != PoiSrvQueryError_none)
			{
				SVC_MGE_LOG_ERR("[ServiceManager] search is not ok error reason is [%d] obj is [%x]\n",error,query);
				#ifdef SERCH_NESTED_ABORT_PRE
				/* UDE状态时收到 nearby,route数据时 */
				if(vrSearchStatus == VR_SEARCH_UDE&&(nearbySearchObjP == query || routeSearchObjP == query))
				{
					SVC_MGE_LOG_WARNING("[ServiceManager] nearby or route is nested by ude \n");
					goto nested;
				}
				if(vrSearchStatus == VR_SEARCH_NEARBY &&(keyWordSearchObjP == query || routeSearchObjP == query))
				{
					 SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested\n", event);
					 goto nested;
				}
				if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP && (nearbySearchObjP == query || routeSearchObjP == query))
				{
					 SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested\n", event);
					 goto nested;
				}
				if(vrSearchStatus == VR_SEARCH_ROUTE && (keyWordSearchObjP == query || nearbySearchObjP == query))
				{
					SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested\n", event);
					goto nested;
				}
				#endif
				goto error;
			}
			SVC_MGE_LOG_INFO("[SM] q[%x] k[%x] n[%x] r[%x] event[%d]\n", query, keyWordSearchObjP , nearbySearchObjP,routeSearchObjP,event);
			/* 关键字搜索相关处理 */
			if (keyWordSearchObjP == query || query == nearbySearchObjP || query == routeSearchObjP)
			{
				if (event == PoiSrvQueryEvent_started)
				{
					SVC_MGE_LOG_INFO("[ServiceManager] load start clean temp buff\n");
					memset(&shm_buff, 0, sizeof(shm_buff));
					shm_buff.buff[0] = '[';
					shm_buff.len = 1;
				}
				//else if (event == PoiSrvQueryEvent_pageLoaded ||event == PoiSrvQueryEvent_suggestionLoaded)
				else if (event == PoiSrvQueryEvent_pageLoaded)
				{
					//PoiKeywordQueryResult searchResult((PoiKeywordQuery*)query);
					int count = 0;
					//static char buff[4096] = { 0 };
					//count = searchResult.getLoadedResultCount();
					keyWordSearchObjP == query ? (count = ((PoiKeywordQuery*)query)->getResult()->getLoadedResultCount()): \
					nearbySearchObjP  == query ? (count = ((PoiNearbyQuery* )query)->getResult()->getLoadedResultCount()): \
					(count = ((PoiRouteQuery* )query)->getResult()->getLoadedResultCount());
					if(keyWordSearchObjP == query && vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP)
					{
						PoiFavorite fav;
						int32_t distance = 0;
						Sideness side;
						//searchResult.getResultByIndex(0, fav, distance);
						keyWordSearchObjP == query ? (((PoiKeywordQuery*)query)->getResult()->getResultByIndex(0, fav, distance)): \
						nearbySearchObjP  == query ? (((PoiNearbyQuery* )query)->getResult()->getResultByIndex(0, fav, distance)): \
						(((PoiRouteQuery* )query)->getResult()->getResultByIndex(0, fav, side ,distance));
						point_temp.x = fav.pos.x;
						point_temp.y = fav.pos.y;
						return;
					}
					
					#ifdef SERCH_NESTED_ABORT_PRE
					/* UDE状态时收到 nearby,route数据时 */
					if(vrSearchStatus == VR_SEARCH_UDE&&(nearbySearchObjP == query || routeSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] nearby or route is nested by ude \n");
						return;
					}
					if(vrSearchStatus == VR_SEARCH_NEARBY &&(keyWordSearchObjP == query || routeSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] ude or route is nested by nearby \n");
						return;
					}
					if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP &&(nearbySearchObjP == query || routeSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] ude or route is nested by nearby \n");
						return;
					}
					if(vrSearchStatus == VR_SEARCH_ROUTE &&(nearbySearchObjP == query || keyWordSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] ude or route is nested by nearby \n");
						return;
					}
					#endif

					//shm_buff.counter = keyWordSearchObjP == query ? (((PoiKeywordQuery*)query)->getResult()->getTotalResultCount()): \
					//				nearbySearchObjP  == query ? (((PoiNearbyQuery* )query)->getResult()->getTotalResultCount()): \
					//				(((PoiRouteQuery* )query)->getResult()->getTotalResultCount());
					SVC_MGE_LOG_INFO("[ServiceManager] get load count is [%d] total count is[%d]\n", count,shm_buff.counter );
				}
				else if (event == PoiSrvQueryEvent_finished)
				{
					/* to write shm */
					if(keyWordSearchObjP == query && vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP)
					{
						vrSearchStatus = VR_SEARCH_NEARBY;
						doVRNearbySearchSigAnalysis(PoiTypeP,&point_temp);
						return;
					}
					#ifdef SERCH_NESTED_ABORT_PRE
					/* UDE状态时收到 nearby,route数据时 */
					if(vrSearchStatus == VR_SEARCH_UDE&&(nearbySearchObjP == query || routeSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] nearby or route is nested\n", event);
						goto nested;
					}
					if(vrSearchStatus == VR_SEARCH_NEARBY && (keyWordSearchObjP == query || routeSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested\n", event);
						goto nested;
					}
					if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP && (nearbySearchObjP == query || routeSearchObjP == query))
					{
						SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested\n", event);
						goto nested;
					}
					if(vrSearchStatus == VR_SEARCH_ROUTE && (keyWordSearchObjP == query || nearbySearchObjP == query))
					{

						SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested\n", event);
						goto nested;

					}
					#endif
					/* to get search result */
					{
						int i_loop,temp;
						shm_buff.counter = keyWordSearchObjP == query ? (((PoiKeywordQuery*)query)->getResult()->getLoadedResultCount()): \
						nearbySearchObjP  == query ? (((PoiNearbyQuery* )query)->getResult()->getLoadedResultCount()): \
						(((PoiRouteQuery* )query)->getResult()->getLoadedResultCount());
						temp = (shm_buff.counter > VR_SEARCH_RESULT_MAX_NUMS)?VR_SEARCH_RESULT_MAX_NUMS:shm_buff.counter;
						SVC_MGE_LOG_INFO("[ServiceManager] ude search numbers [%d]\n", temp);
						for (i_loop = 0; i_loop < temp; i_loop++)
						{
							/* 指定poi点附近搜索的情况下,只获取当前点的坐标 */
							PoiFavorite fav;
							int32_t distance = 0;
							Sideness side;
							//searchResult.getResultByIndex(i_loop, fav, distance);
							keyWordSearchObjP == query ? (((PoiKeywordQuery*)query)->getResult()->getResultByIndex(i_loop, fav, distance)): \
							nearbySearchObjP  == query ? (((PoiNearbyQuery* )query)->getResult()->getResultByIndex(i_loop, fav, distance)): \
							(((PoiRouteQuery* )query)->getResult()->getResultByIndex(i_loop, fav, side, distance));
							shm_buff.lon = fav.pos.x^EOR_KEY;
							shm_buff.lat = fav.pos.y^EOR_KEY;
							shm_buff.dis = distance;
							char name[100] = { 0 };
							char address[128] = { 0 };
							cq_encodeUtf8(fav.name, SIZE_T_MAX, name, 100);
							cq_encodeUtf8(fav.address, SIZE_T_MAX, address, 128);
							/* 打包数据 */
							shm_buff.len += sprintf(&shm_buff.buff[shm_buff.len], \
								"{\"longitude\":\"%d\",\"latitude\":\"%d\",\"distance\":%d,\"name\":\"%s\",\"address\":\"%s\"},", \
							shm_buff.lon, shm_buff.lat, shm_buff.dis, name, address);
						}
						
					}
					shm_buff.buff[shm_buff.len - 1] = ']';
					{
						//SVC_MGE_LOG_INFO("[ServiceManager] write shm [%s]\n",shm_buff.buff);
						//printf("[ServiceManager] write shm [%s]\n",shm_buff.buff);
						#ifdef MAPBAR_TOYOTA_262AVN_LINUX
						shm_Clean_Write((void *)shm_buff.buff,shm_buff.len);
						#endif
						goto done;
					}
				}
				else if(PoiSrvQueryEvent_districtLoaded == event || PoiSrvQueryEvent_failed == event) /* 不支持行政区域切换 */
				{
					SVC_MGE_LOG_ERR("[ServiceManager] recived unknow evevt [%d]\n", event);
					goto error;
				}
				else if(PoiSrvQueryEvent_suggestionLoaded == event) /* 该消息用来城市跳转，跳过处理 */
				{
					suggest_num = pKeyWordSuggestResultObj->getLoadedResultCount();
					for(index = 0;suggest_num > index;index++)
					{
						pKeyWordSuggestResultObj->getResultByIndex(index, obj);
						switch(obj.type)
						{
						case GeneralPoiSuggestType_cityJump:
							return;
						case GeneralPoiSuggestType_corrections:
						case GeneralPoiSuggestType_cityDistribution:
						case GeneralPoiSuggestType_suggestList:
							break;
						}
					}
					goto error;
				}
				else if(PoiSrvQueryEvent_suggestionAndPageLoaded == event)
				{
					/* do nothing 此case会返回finish事件 */
				}
				else
				{
					SVC_MGE_LOG_ERR("[ServiceManager] recived unknow evevt [%d]\n", event);
					goto error;
				}
			}
			SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
			return;
		done:
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->data = NULL;
				if (keyWordSearchObjP == query && vrSearchStatus == VR_SEARCH_UDE)
				{
					//构造DBUS消息通知系统
					SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d] \n",__FUNCTION__,__LINE__);
					paramData->funcId = ServiceManager::FuncID_Send_KeyWoardSearchSuccess;
				}
				else if (nearbySearchObjP == query || routeSearchObjP == query)
				{
					//构造DBUS消息通知系统
					SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d] \n",__FUNCTION__,__LINE__);
					paramData->funcId = ServiceManager::FuncID_Send_NearbySearchSuccess;
				}
				SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d] \n",__FUNCTION__,__LINE__);
				NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
				vrSearchStatus = VR_SEARCH_NONE;
				SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d] VR search status is change to [%d]\n",__FUNCTION__,__LINE__,vrSearchStatus);
				return;
			}
		error:
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->data = NULL;
				if (keyWordSearchObjP == query && vrSearchStatus == VR_SEARCH_UDE)
				{
					//构造DBUS消息通知系统
					paramData->funcId = ServiceManager::FuncID_Send_KeyWoardSearchFailure;
				}
				else
				//else if (nearbySearchObjP == query || routeSearchObjP == query)
				{
					//构造DBUS消息通知系统
					paramData->funcId = ServiceManager::FuncID_Send_NearbySearchFailure;
				}

				NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
				vrSearchStatus = VR_SEARCH_NONE;
				SVC_MGE_LOG_INFO("[ServiceManager] VR search status is change to [%d]\n",vrSearchStatus);
				/* 清除既有的共有内存的内容 */
				#ifdef MAPBAR_TOYOTA_262AVN_LINUX
				char tempBuff[2] = {0};
				shm_Clean_Write((void *)tempBuff,sizeof(tempBuff));
				#endif
				SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
				return;
			}
		#ifdef 	SERCH_NESTED_ABORT_PRE
		nested:
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->data = NULL;
				if (keyWordSearchObjP == query)
				{
					if(isNearbyPoiNest == true)
					{
						//构造DBUS消息通知系统
						paramData->funcId = ServiceManager::FuncID_Send_NearbySearchFailure;
						isNearbyPoiNest = false;
					}
					else
					{
						paramData->funcId = ServiceManager::FuncID_Send_KeyWoardSearchFailure;
					}
				}
				else
				//else if (nearbySearchObjP == query || routeSearchObjP == query)
				{
					//构造DBUS消息通知系统
					paramData->funcId = ServiceManager::FuncID_Send_NearbySearchFailure;
				}
				NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
				//vrSearchStatus == VR_SEARCH_NONE;
				SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d] VR search status is [%d]\n",__FUNCTION__,__LINE__,vrSearchStatus);
				return;
			}
		#endif
		}
	};
	/**
	* @brief        VR_Search_UDE 获取周边POI点信息并响应车机
	* @date         2018 03.05
	* @param[in]    车机通知数据结构体
	*/
	void doVRUdeSearchSigAnalysis(cqWCHAR *data)
	{	
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
		static int isUdeFristSearch = 1;
		static PoiKeywordQuery keyWordSearchObj;
		if (isUdeFristSearch)
		{
			static SmClientCallback keyWordSearchDataCallBcak;
			keyWordSearchObjP = &keyWordSearchObj;
			pKeyWordSuggestResultObj = new MbApi::PoiSuggestQueryResult(keyWordSearchObjP);
			keyWordSearchObj.setFromType(GRequestType_none);//设定搜索来源
			keyWordSearchObj.setSortMode(GeneralSearchSortType_name);//设定排序方式
			keyWordSearchObj.setPageSize(15);//设定搜索页大小
			//keyWordSearchObj.setMaxResultCount(300);设置300不会回finish消息
			keyWordSearchObj.setMaxResultCount(VR_SEARCH_MAX_NUM);//设定搜索结果数量
			keyWordSearchObj.setNetworkMode(PoiSearchMode_preferOnline);//设定网络模式
			keyWordSearchObj.addListener(&keyWordSearchDataCallBcak);//设定监听 call back函数
			isUdeFristSearch--;
		}
		keyWordSearchObj.setCenter(Session_getDataCenter()->m_carPos);//设定中心点位置
		//keyWordSearchObj.setWmrId(CitySelect_getCityIdByPosition(&(Session_getDataCenter()->m_carPos)));//设定搜索区域
		//add by shenghao for TYTAVNNEW-678 20180621
		Point center = Map_getMapCenter();
		WmrObjId id = CitySelect_getCityIdByPosition(&center);
		if (invalidWmrObjId == id)
		{
			center = Map_getCarPosition();
			id = CitySelect_getCityIdByPosition(&center);
		}
		keyWordSearchObj.setWmrId(id);//设定搜索区域
		keyWordSearchObj.setKeyword((const cqWCHAR*)data);//设定搜索关键字
		keyWordSearchObj.doSearch();//开始搜索
		SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
		return;
	}
	
	/**
	* @brief        VR_Search_UDE 获取周边POI点信息并响应车机
	* @date         2018 03.05
	* @param[in]    车机通知数据结构体
	*/
	
	void VRUdeSearchSigAnalysis(void *data)
	{
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
		#ifdef SERCH_NESTED_ABORT_PRE
		if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP || vrSearchStatus == VR_SEARCH_UDE)
		{
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->data = NULL;
			//构造DBUS消息通知系统
			vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP ? (paramData->funcId = ServiceManager::FuncID_Send_NearbySearchFailure):\
															  (paramData->funcId = ServiceManager::FuncID_Send_KeyWoardSearchFailure);
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			/* 清除既有的共有内存的内容 */
			//char tempBuff[2] = {0};
			//shm_Clean_Write((void *)tempBuff,sizeof(tempBuff));
			SVC_MGE_LOG_WARNING("[ServiceManager] nearby POI search is nested by UDE\n");
		}
		#endif
		#ifdef SERCH_NESTED_ABORT_CANCLE
		/* 获取UDE搜索对象,周边搜索,沿路搜索对象的状态 */
		/* UDE搜索对象非IDLE状态时， */
		#endif
		
		vrSearchStatus = VR_SEARCH_UDE;
		static cqWCHAR recv_buff[100] = { 0 };
		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState() || data == NULL || strlen((const char *)data) < 1)
		{
			goto error;
		}
		cq_decodeUtf8((cqCHAR*)data, SIZE_T_MAX, recv_buff, sizeof(recv_buff));
		doVRUdeSearchSigAnalysis(recv_buff);
		SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
		return ;
	error:
		{
			vrSearchStatus = VR_SEARCH_NONE;
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->data = NULL;
			//构造DBUS消息通知系统
			paramData->funcId = ServiceManager::FuncID_Send_KeyWoardSearchFailure;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			/* 清除既有的共有内存的内容 */
			#ifdef MAPBAR_TOYOTA_262AVN_LINUX
			char tempBuff[2] = {0};
			shm_Clean_Write((void *)tempBuff,sizeof(tempBuff));
			#endif
			SVC_MGE_LOG_ERR("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
			return;
		}		
	}
	
	/**
	*	@brief
	*		VR关键字搜索Callback
	*	@return
	*		void
	*/
	void VRUdeSearchSig(void* data)
	{
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRUdeSearchSigAnalysis, data);
	}

	/**
	* @brief        doVRNearbySearchSigAnalysis 获取周边POI点信息并响应车机
	* @date         2018 03.06
	* @param[in]    车机通知数据结构体
	*/

	int8_t doVRNearbySearchSigAnalysis(const cqWCHAR* data,Point * point)
	{
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
		static int isNearbyFristSearch = 1;
		static PoiNearbyQuery nearbySearchObj;
		/* 搜索，周边搜索使用同一个监听类 */
		static SmClientCallback nearby_clb;
		if (isNearbyFristSearch)
		{
			nearbySearchObjP = &nearbySearchObj;
			nearbySearchObj.setNetworkMode(PoiSearchMode_preferOnline);//设定网络搜索方式
			nearbySearchObj.setPageSize(15);//设定搜索结果页大小
			nearbySearchObj.setMaxResultCount(VR_SEARCH_MAX_NUM);//设定搜索结果上限值
			nearbySearchObj.setRange(20000);//设定搜索范围
			nearbySearchObj.addListener(&nearby_clb);
			isNearbyFristSearch--;			
		}
		nearbySearchObj.setPoiType(data);//设定搜索关键字
		if (point)
		{
			nearbySearchObj.setCenter(*point);
		}
		else
		{
			nearbySearchObj.setCenter(Session_getDataCenter()->m_carPos);//设定搜索中心点
		}
		nearbySearchObj.doSearch();
		SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
		return 0;
	}

	/**
	* @brief        doVRRouteSearchSigAnalysis 获取沿路POI点信息并响应车机
	* @date         2018 03.06
	* @param[in]    车机通知数据结构体
	*/

	int8_t doVRRouteSearchSigAnalysis(const cqWCHAR * data)
	{
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d]\n",__FUNCTION__,__LINE__);
		/* 搜索，周边搜索使用同一个监听类 */
		static SmClientCallback routeClb;
		static int isRouteFristSearch = 1;
		static PoiRouteQuery routeSearchObj;
		
		//cqWCHAR recv_buff[100] = { 0 };
		//cq_decodeUtf8((cqCHAR*)data, SIZE_T_MAX, recv_buff, sizeof(recv_buff));
		
		if (isRouteFristSearch)
		{
			routeSearchObjP = &routeSearchObj;
			routeSearchObj.setNetworkMode(PoiSearchMode_preferOnline);
			routeSearchObj.setPageSize(15);
			routeSearchObj.setMaxResultCount(VR_SEARCH_MAX_NUM);
			routeSearchObj.setMaxOrthogonalDistance(300);
			routeSearchObj.setMaxQueryDistance(20000);
			routeSearchObj.addListener(&routeClb);
			isRouteFristSearch--;
		}
		routeSearchObj.setRoute((RouteBase*)RouteApi_getRoute());
		routeSearchObj.setPoiType(data);//设定搜索关键字
		routeSearchObj.setCenter(Session_getDataCenter()->m_carPos);//设定搜索中心点
		routeSearchObj.doSearch();
		SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
		return 0;
	}


	void VRNearbySearchSigAnalysis(void *data)
	{
		int cmd = 0,len = 0,loop = 0;
		cqCHAR poiPos[100] = { 0 };
		cqCHAR poiType[30] = { 0 };
		cqCHAR * ptmp = (cqCHAR *)data;
		cqWCHAR wCharPoiType[100] = { 0 };
		cqWCHAR wCharPoiPos[100] = { 0 };

		if(data == NULL || (strlen((const char *)data) < 1))
		{
			goto error;
		}
		
		sscanf((const char *)data,"%d,%[^,]",&cmd,poiType);
		SVC_MGE_LOG_INFO("[ServiceManager] IN function [%s] [%d] nearby recv cmd is [%d] poitype is [%s]\n",__FUNCTION__,__LINE__,cmd,poiType);
		if(strlen(poiType) == 0 )
		{
			goto error;
		}
		
		cq_decodeUtf8((cqCHAR*)poiType, SIZE_T_MAX, wCharPoiType, sizeof(wCharPoiType));
		
		if(4 == cmd)
		{

			for(loop = 0;loop < 2; loop++)
			{
				ptmp = cq_strchr(ptmp, ',');
				if(!ptmp)
					goto error;
				else
					ptmp++;
			}
			if(cq_strlen(ptmp))
				cq_strncpy(poiPos, ptmp ,100);
			else
				goto error;
			cq_decodeUtf8((cqCHAR*)poiPos, SIZE_T_MAX, wCharPoiPos, sizeof(wCharPoiPos));
		}
		
		
		if( !(PoiTypeP = (cqWCHAR *)getIndexOfTypeDate(wCharPoiType)))
		{
			goto error;
		}
		else
		{
			SVC_MGE_LOG_INFO("[ServiceManager]len [%d] poitype is [%s] poipos[%s]\n",len,poiType,poiPos);
		}
		// 如果引擎非运行状态，向车机发送命令失败消息
		if (MbApi::AppService::AppState_running != MbApi::AppService::getInstance()->getCurrentState())
		{
			goto error;
		}
		
		switch (cmd)
		{
		/* 查找附近 */
		case 1:
		{
			#ifdef SERCH_NESTED_ABORT_PRE
			if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP )
			{
				/* 当前状态为Nearby poi 搜索时，keyword结果返回时应答应该是周边搜的失败 */
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby poi search is nested by nearby\n");
				//isNestNeedResp = false;
				isNearbyPoiNest = true;
			}
			if(vrSearchStatus == VR_SEARCH_NEARBY)
			{
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby search is nested by nearby\n");
				goto error;
			}
			#endif
			vrSearchStatus = VR_SEARCH_NEARBY;
			SVC_MGE_LOG_INFO("[ServiceManager] VR search status is change to [%d]\n",vrSearchStatus);
			doVRNearbySearchSigAnalysis(getIndexOfTypeDate(wCharPoiType),NULL);
		}
			break;
		/* 查找沿途 */
		case 2:
		{
			#ifdef SERCH_NESTED_ABORT_PRE
			if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP )
			{
				/* 当前状态为Nearby poi 搜索时，UDE对象结果返回时应答应该是周边搜的失败 */
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby poi search is nested by route\n");
				//isNestNeedResp = false;
				isNearbyPoiNest = true;
			}
			if(vrSearchStatus == VR_SEARCH_ROUTE)
			{
				SVC_MGE_LOG_WARNING("[ServiceManager] route search is nested by route\n");
				goto error;
			}
			#endif
			if (RouteApi_getRoute() != NULL)
			{
				vrSearchStatus = VR_SEARCH_ROUTE;
				SVC_MGE_LOG_INFO("[ServiceManager] VR search status is change to [%d]\n",vrSearchStatus);
				doVRRouteSearchSigAnalysis(getIndexOfTypeDate(wCharPoiType));
			}
			else
			{
				SVC_MGE_LOG_ERR("[ServiceManager] current line is NULL\n");
				goto error;
			}
			
		}
			break;
		/* 查找终点附近 */
		case 3:
		{
			#ifdef SERCH_NESTED_ABORT_PRE
			if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP )
			{
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby poi search  is nested by finish\n");
				isNearbyPoiNest = true;
			}
			if(vrSearchStatus == VR_SEARCH_NEARBY)
			{
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby search is nested by nearby\n");
				goto error;
			}
			#endif
			/* 获取终点坐标 */
			//const RoutePlan* plan = MbApi::RouteApi_getPlan();
			if( MbApi::RouteApi_getPlan() != NULL )
			{
				MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
				if(plan.getPointNum() < 2)
				{
					SVC_MGE_LOG_ERR("[ServiceManager] dest num is [%d]\n" , plan.getPointNum());
					goto error;
				}
				//size_t endIndex = plan->destNum - 1;
				// 保存终点
				Point endPosition = plan.getPoint(plan.getPointNum() -1)->pos;
				//调用周边搜索
				vrSearchStatus = VR_SEARCH_NEARBY;
				SVC_MGE_LOG_INFO("[ServiceManager] called VR finish search search status is change to [%d]\n",vrSearchStatus);
				doVRNearbySearchSigAnalysis(getIndexOfTypeDate(wCharPoiType), &endPosition);
			}
			else
			{
				SVC_MGE_LOG_ERR("[ServiceManager] get finish poi failed\n");
				goto error;
			}
		}
			break;
		/* 搜索指定poi */
		case 4:
		{
			#ifdef SERCH_NESTED_ABORT_PRE
			if(vrSearchStatus == VR_SEARCH_UDE)
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				paramData->data = NULL;
				paramData->funcId = ServiceManager::FuncID_Send_KeyWoardSearchFailure;
				NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
				SVC_MGE_LOG_WARNING("[ServiceManager] ude search is nested by nearby poi\n");
			}
			#endif
			if (strlen(poiPos) == 0)
			{
				goto error;
			}
			vrSearchStatus = VR_SEARCH_NEARBY_WAIT_UDERESP;
			SVC_MGE_LOG_INFO("[ServiceManager] called VR poi search status is change to [%d]\n",vrSearchStatus);
			doVRUdeSearchSigAnalysis(wCharPoiPos);
		}
			break;
		/* 家附近搜索 */
		case 5:
		{
			#ifdef SERCH_NESTED_ABORT_PRE
			if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP )
			{
				//isNestNeedResp = false;
				isNearbyPoiNest = true;
			}
			if(vrSearchStatus == VR_SEARCH_NEARBY)
			{
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby search is nested by nearby\n");
				goto error;
			}
			#endif
			/* 获取家的坐标 */
			if(PoiFavorite* fav = MbApi::NaviShortcuts_getFav(0))
			{
				vrSearchStatus = VR_SEARCH_NEARBY;
				SVC_MGE_LOG_INFO("[ServiceManager] called VR home search status is change to [%d]\n",vrSearchStatus);
				doVRNearbySearchSigAnalysis(getIndexOfTypeDate(wCharPoiType), &fav->pos);
			}
			else
			{
				SVC_MGE_LOG_ERR("[ServiceManager] get home poi failed\n");
				goto error;
			}
			
		}
			break;
		/* 公司附近搜索 */
		case 6:
		{
			#ifdef SERCH_NESTED_ABORT_PRE
			if(vrSearchStatus == VR_SEARCH_NEARBY_WAIT_UDERESP)
			{
				//isNestNeedResp = false;
				isNearbyPoiNest = true;
			}
			if(vrSearchStatus == VR_SEARCH_NEARBY)
			{
				SVC_MGE_LOG_WARNING("[ServiceManager] nearby search is nested by nearby\n");
				goto error;
			}
			#endif
			/* 获取公司的坐标 */
			if(PoiFavorite* fav = MbApi::NaviShortcuts_getFav(1))
			{
				vrSearchStatus = VR_SEARCH_NEARBY;
				SVC_MGE_LOG_INFO("[ServiceManager] called VR company search status is change to [%d]\n",vrSearchStatus);
				doVRNearbySearchSigAnalysis(getIndexOfTypeDate(wCharPoiType), &fav->pos);
			}
			else
			{
				SVC_MGE_LOG_ERR("[ServiceManager] get company poi failed\n");
				goto error;
			}
		}	
			break;
		default:
			goto error;
			break;
		}
		SVC_MGE_LOG_INFO("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
		return;
	error:
		{
			//构建dbus消息通知主机操作失败
			ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
			paramData->funcId = ServiceManager::FuncID_Send_NearbySearchFailure;
			paramData->data = NULL;
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
			//#ifndef SERCH_NESTED_ABORT_PRE
			//vrSearchStatus == VR_SEARCH_NONE;
			//SVC_MGE_LOG_ERR("[ServiceManager] called VR  search status is change to [%d]\n",vrSearchStatus);
			//#endif
			//char tempBuff[2] = {0};
			//shm_Clean_Write((void *)tempBuff,sizeof(tempBuff));
			SVC_MGE_LOG_ERR("[ServiceManager] OUT function [%s] [%d]\n",__FUNCTION__,__LINE__);
			return;
		}
	}
	/**
	*	@brief
	*		VR周边搜索Callback
	*	@return
	*		void
	*/
	void VRNearbySearchSig(void* data)
	{
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRNearbySearchSigAnalysis, data);
	}

	/**
	*	@brief
	*		解码经纬度
	*	@return
	*		uint32
	*/
	uint32 VRDecode(uint32 latlon)
	{
		return latlon^EOR_KEY;
	}

	/**
	*	@brief
	*		携带路线偏好开始算路
	*	@return
	*		void
	*/
	void VRHybridStartRoute(RouteService::Plan& routePlan, const HybridRouteParam& param)
	{
		AvoidRoadType avoidance = AvoidRoadType_none;
		RouteRule rule = RouteRule_recommended;

		// avoid jam
		routePlan.setTmcEnable(param[HybridRouteType_avoidJam]);
		if (param[HybridRouteType_avoidToll])
			avoidance = (AvoidRoadType)(avoidance | AvoidRoadType_toll);

		// shortest
		if (param[HybridRouteType_shortest])
			rule = RouteRule_shortest;

		// highway priority
		if (param[HybridRouteType_highwayPriority])
			rule = RouteRule_fastest;

		// avoid highway
		if (param[HybridRouteType_avoidHighway])
			//avoidance = (AvoidRoadType)(avoidance | AvoidRoadType_expy | AvoidRoadType_urbanExpy);
			//TYTAVNNEW-1154【系统测试】【导航功能】【径路诱导】偏好设置中选择避让高速，避让了城市快速路
		    avoidance = (AvoidRoadType)(avoidance | AvoidRoadType_expy);
		// avoid ferry
		if (param[HybridRouteType_avoidFerry])
			avoidance = (AvoidRoadType)(avoidance | AvoidRoadType_sailingRoute);

		//RoutePlan_setAvoidRoadType(&copyPlan, avoidance);
		routePlan.setAvoidMode(avoidance);

		//RoutePlan_setRule(&copyPlan, rule);
		routePlan.setRule(rule);

		MbApi::RouteService::getInstance()->route(routePlan, MbApi::RouteService::ResultMode_single, MbApi::ROUTE_SERVICE_USER_ID_SERVICE_MANAGER);
	}

	/**
	*	@brief
	*		从文件中load路线偏好
	*	@return
	*		void
	*/
	json_t* VRDataLoadFromFile()
	{
		FileObject* file = File_alloc();
		ssize_t fileLen;
		char* fileBuffer;

		// Open failed
		if (!File_openFallible(file, CLIENT_CONFIG_FILE_JSON, File_OpenForRead))
		{
			CQ_TRACE("[Config_loadJsonConfig]: open file failed.", 0);
			File_free(file);
			return NULL;
		}
		// Read file
		fileLen = File_getLength(file);
		fileBuffer = (char*)malloc(sizeof(char)* fileLen);
		File_read(file, fileBuffer, sizeof(char)*fileLen);

		// Convert file into json data
		json_error_t jError;
		json_t* root = json_loadb(fileBuffer, fileLen, JSON_DECODE_ANY, &jError);

		// If load successed
		if (root != NULL)
		{
			free(fileBuffer);
			File_close(file);
			File_free(file);
			// Convert from json data into NaviOption.
			return root;
			// Destruction
			//json_decref(root);
		}
		else
		{
			// Destruction
			free(fileBuffer);
			File_close(file);
			File_free(file);
			return NULL;
		}
	}

	// 将json数据转为路线偏好
	bool VRGetHybridRouteType(const json_t* json, HybridRouteParam* value)
	{
		BOOL ret = TRUE;
		json_t* jValue = json_object_get(json, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE);
		if (jValue == NULL)
			return FALSE;

		json_t* avoidJam = json_object_get(jValue, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_JAM);
		if (avoidJam == NULL)
			ret = FALSE;
		else
			value->setRouteParam(HybridRouteType_avoidJam, (BOOL)json_integer_value(avoidJam));

		json_t* avoidToll = json_object_get(jValue, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_TOLL);
		if (avoidToll == NULL)
			ret = FALSE;
		else
			value->setRouteParam(HybridRouteType_avoidToll, (BOOL)json_integer_value(avoidToll));

		json_t* shortest = json_object_get(jValue, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_SHORTEST);
		if (shortest == NULL)
			ret = FALSE;
		else
			value->setRouteParam(HybridRouteType_shortest, (BOOL)json_integer_value(shortest));

		json_t* highwayPriority = json_object_get(jValue, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_HIGHWAY_PRIORITY);
		if (highwayPriority == NULL)
			ret = FALSE;
		else
			value->setRouteParam(HybridRouteType_highwayPriority, (BOOL)json_integer_value(highwayPriority));

		json_t* avoidHighway = json_object_get(jValue, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_HIGHWAY);
		if (avoidHighway == NULL)
			ret = FALSE;
		else
			value->setRouteParam(HybridRouteType_avoidHighway, (BOOL)json_integer_value(avoidHighway));

		json_t* avoidFerry = json_object_get(jValue, SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_FERRY);
		if (avoidFerry == NULL)
			ret = FALSE;
		else
			value->setRouteParam(HybridRouteType_avoidFerry, (BOOL)json_integer_value(avoidFerry));

		return ret;
	}

	/**
	*	@brief
	*		回复主机应答
	*	@return
	*		void
	*/
	
	static void doVRrespAddFAav(VR_FAV_TYPE type,VR_ERR_TYPE errorcode)
	{
	
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		switch(type)
		{
		case VR_FAV_TYPE_CURRENT_POINT:
			paramData->funcId = FuncID_Send_SaveCurrentToFavResp;
			break;
		case VR_FAV_TYPE_DEST_POINT:
			paramData->funcId = FuncID_Send_SaveDestToFavResp;		
			break;
		case VR_FAV_TYPE_POI_POINT:
			paramData->funcId = FuncID_Send_SavePoiToFavResp;	
			break;
		}
		switch(errorcode)
		{
		case VR_ERROR_NONE:
			
			break;
		case VR_ERROR_FAV_ADD_FAILED:
			
			break;
		case VR_ERROR_FAV_AREADY_ADD:
			
			break;
		}
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	}

	/**
	*	@brief
	*		调用mbapi接口添加至收藏夹
	*	@return
	*		void
	*/
	
	static enum VR_ERR_TYPE VRAddPoint2Fav(PoiFavorite* fav)
	{
		/* check favorite */
		if(FAVORITE_SERVICE()->isFavoriteExist(fav))
		{
			printf("fav is added\n");
			return VR_ERROR_FAV_AREADY_ADD;
		}
		
		/* add favorite */
		MbApi::FavoriteErrorCode ret = FAVORITE_SERVICE()->addFavorite(fav);
		/* check return value */
		if(ret != FavoriteErrorCode_none)
		{
			printf(" ret is [%d] \n",ret);
			return VR_ERROR_FAV_ADD_FAILED;
		}
	}

	/**
	*	@brief
	*		构造参数
	*	@return
	*		void
	*/

	static DBUS_CMD_DATA* VRCmdDateInit(int argc)
	{
		DBUS_CMD_DATA* pret = NULL;
	
		if(!(argc > 0))
			return pret;
		pret = (DBUS_CMD_DATA*)malloc(sizeof(DBUS_CMD_DATA));
		if(!pret)
			return pret;
		pret->argv = (cqCHAR **)malloc(argc*sizeof(cqCHAR*));
		if(!(pret->argv))
		{
			free(pret);
			return NULL;
		}
		pret->argc = argc;
		return pret;
	}
	
	/**
	*	@brief
	*		释放参数指针
	*	@return
	*		void
	*/

	static int VRCmdDateDeInit(DBUS_CMD_DATA* data)
	{
		if(!data)
			return  -1;

		if(!data->argv)
		{
			free(data);
			return -1;
		}

		free(data->argv);
		free(data);
		return 0;
	}
	
	/**
	*	@brief
	*		获取收藏点协议域数据
	*	@return
	*		void
	*/

	static inline int VRGetFavDomain(cqCHAR * data)
	{
		cqCHAR * p = data;
		int count;
		
		if( data == NULL || !(cq_strlen(data)))
			return 0;

		for(count = 1;p = cq_strchr(p, ',');p++,count++);
		return count;
	}
	

	/**
	*	@brief
	*		解析收藏指令参数
	*	@return
	*		void
	*/
	static int VRGetFavInfo(cqCHAR * data,DBUS_CMD_DATA* cmd)
	{
		cqCHAR* p;
		int i_loop;
		
		if( data == NULL || !(cq_strlen(data)))
			return -1;

		p = (cqCHAR* )data;
		cmd->argv[0] = data;
		for(i_loop = 1;i_loop < cmd->argc;i_loop++)
		{
			p = cq_strchr(p, ',');
			p[0] = '\0';
			cmd->argv[i_loop] = ++p;
		}
		for(i_loop = 0;i_loop < cmd->argc; i_loop++)
			printf("argv[%d] = %s\n",i_loop,cmd->argv[i_loop]);
		return 0;
	}

	/**
	*	@brief
	*		添加收藏点到收藏夹
	*	@return
	*		void
	*/
	void VRAddPoint2FavSigAnalysis(void *data)
	{
		Point ptmep;		
		PoiFavorite fav = {0};
		cqCHAR* p = (cqCHAR *)data;
		DBUS_CMD_DATA* cmd;
		VR_ERR_TYPE errorcode = VR_ERROR_NONE;
		VR_FAV_TYPE type;

		/* check data */
		if(data == NULL || !cq_strlen((const cqCHAR *) data) || !(VRGetFavDomain((cqCHAR *)data) > 0))
		{
			errorcode = VR_ERROR_PARAMETER;
			goto out2;
		}
		
		/* Analytical data */
		cmd = VRCmdDateInit(VRGetFavDomain((cqCHAR*)data));
		if(!cmd)
		{
			errorcode = VR_ERROR_PARAMETER;
			goto out2;
		}
		
		if(VRGetFavInfo((cqCHAR*)data,cmd) != 0)
		{
			errorcode = VR_ERROR_PARAMETER;
			goto out2;
		}
		
		switch((VR_FAV_TYPE)cq_atoi((const cqCHAR * )(cmd->argv[0])))
		{
		case VR_FAV_TYPE_CURRENT_POINT:
		{
			ptmep = Map_getCarPosition();
			MAP_POI_SEARCH_SERVICE()->getPoiFavoriteByPosition(&ptmep,&fav);
			errorcode = VRAddPoint2Fav(&fav);
			type = VR_FAV_TYPE_CURRENT_POINT;
		}
			break;
		case VR_FAV_TYPE_DEST_POINT:
		{
			type = VR_FAV_TYPE_DEST_POINT;
			if( MbApi::RouteApi_getPlan() != NULL)
			{
				MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
				SVC_MGE_LOG_ERR("[ServiceManager] MbApi::RouteApi_getPlan() is NULL\n");
				if(plan.getPointNum() < 2)
				{
					errorcode = VR_ERROR_NO_PLAN;
					goto out1;
				}
				else
				{
					plan.getPoint(plan.getPointNum() - 1,fav);
					errorcode = VRAddPoint2Fav(&fav);
					goto out1;
				}
			}
			else
			{
				errorcode = VR_ERROR_NO_PLAN;
				goto out1;
			}
		}
			break;
		case VR_FAV_TYPE_POI_POINT:
		{
			type = VR_FAV_TYPE_POI_POINT;
			//printf("log [%s],p [%s],name [%s], addr [%s]\n",cmd->argv[1],cmd->argv[2],cmd->argv[3],cmd->argv[4]);
			fav.type = cq_atoi(cmd->argv[1]);
			fav.pos.x = cq_atoi(cmd->argv[2])^EOR_KEY;
			fav.pos.y = cq_atoi(cmd->argv[3])^EOR_KEY;
			cq_decodeUtf8(cmd->argv[4],SIZE_T_MAX,fav.name,sizeof(fav.name));
			cq_decodeUtf8(cmd->argv[5],SIZE_T_MAX,fav.address,sizeof(fav.address));
			errorcode = VRAddPoint2Fav(&fav);
		}
			break;
		default:
			break;
		}
	out1:
		VRCmdDateDeInit(cmd);
	out2:
		doVRrespAddFAav(type,errorcode);
	}
	
	/**
	*	@brief
	*		添加收藏点回调函数入口
	*	@return
	*		void
	*/

	void VRAddPoint2FavSig(void* data)
	{
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRAddPoint2FavSigAnalysis, data);
	}

	/**
	*	@brief
	*		获取自车位置的逆地理POI信息
	*	@return
	*		void
	*/

	static int32_t VRGetCarPositionPoinameUtf8(cqCHAR * buff,int32_t len)
	{
		Point ptmep;
		PoiFavorite fav = {0};

		if(NULL == buff || len < 0)
			return -1;
		
		ptmep = Map_getCarPosition();
		MAP_POI_SEARCH_SERVICE()->getPoiFavoriteByPosition(&ptmep,&fav);
		return cq_encodeUtf8(fav.name, SIZE_T_MAX, buff, len) > 0 ?0:-1;
	}

	static int32_t VRGetCarPositionRoadName(cqCHAR * buff,int32_t len)
	{
		 SegmentGrabInfo info = {0};
		 NaviDataCenter* dc = MbApi::Session_getDataCenter();
		 DSegmentAttributes attrTmp;

		if(NULL == buff || len < 0)
			return -1;
		if(MbApi::Map_grabSegments(&dc->m_carPos, &info, FALSE))
		{
			MbApi::RouteBaseApi_getDSegmentAttributes(info.dsegId, &attrTmp, MbApi::MbApiSegmentAttributesFlag_All);
			attrTmp.name;
		}
		else
		{
			
		}
	}

	static int32_t VRGetCarPositionNearbyCrossName(void)
	{
		return 0;
	}
	
	/**
	*	@brief
	*		VR我在哪
	*	@return
	*		void
	*/
	void VRWhereAmIsSigAnalysis(void *data)
	{
		UNUSED_VAR(data);
		/* 获取自车位置对应的 POI  名称信息            */
		
	}

	/**
	*	@brief
	*		VR获取历史目的信息
	*	@return
	*		void
	*/
	void VRGetHistoryDestPointInfo(void *data)
	{
		UNUSED_VAR(data);
		PoiFavorite pFav;
		int32 nums,loop;
		
		nums = MbApi::HistoryDestination_getItemNum();
		if(nums == 0 || nums < 0)
		{
			/* 通知主机获取失败 */
		}
		for(loop = 0;loop < nums;loop++)
		{
			if(MbApi::HistoryDestination_getItem(loop, &pFav) == TRUE)
			{
				/* 组织数据 */	
			}
			else
			{
				/* 不做任何处理，继续获取数据 */
				continue;
			}
		}
		/* 给主机发送数据,通知主机获取信息成功 */
		
	}

	static BOOL isNaviHaveDestPoint(void)
	{
		MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
		
		/* 调用mbapi接口获取当前路线 */
		if(plan == NULL|| plan.getPointNum() < 2)
		{
			/* 通知主机当前无目的地 */
		}
		else
		{
			/* 通知主机当前有目的地 */
		}
	}

	/**
	*	@brief
	*		VR获取途径点list信息
	*	@return
	*		void
	*/
	void VRGetViaPointInfo(void *data)
	{
		MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
		PoiFavorite fav;
		int32_t i_loop = 0,errorcode = 0;
		cqCHAR* ptmep;
			
		UNUSED_VAR(data);
		json_t* viapoint = json_object();
		json_t* viapointArray = json_array();

		if( viapoint == NULL || viapointArray == NULL )
			goto out;
		
		/* 调用 mbapi  接口判断当前有无路线,当前无路线回复主机当前无路线 */
		if(plan == NULL)
		{
			/* TODO resp 当前无路线 */
			errorcode = 1;
			goto out;
		}
		
		if(!(plan.getPointNum() > 2))
		{
			/* TODO resp 无途径点信息 */
			errorcode = 1;
			goto out;
		}

		for( i_loop =1; i_loop < plan.getPointNum() -1 ; i_loop++)
		{
			plan.getPoint(i_loop,fav);
			json_object_set_new(viapoint, "index", json_integer(i_loop));
			json_object_set_new(viapoint, "poitype", json_integer(fav.type));
			json_object_set_new(viapoint, "longitude", json_integer(fav.pos.x));
			json_object_set_new(viapoint, "latitude", json_integer(fav.pos.y));
			json_object_set_new(viapoint, "name", json_string_S(fav.name));
			json_object_set_new(viapoint, "address", json_string_S(fav.address));
			json_object_set_new(viapoint, "phonenum", json_string_S(fav.phoneNumber));	
			json_object_set_new(viapoint, "distance", json_integer(-1));
			
			json_array_append_new(viapointArray,viapoint);
		}
		/* 通知主机获取成功 */
		ptmep = json_dumps(viapointArray, JSON_ENCODE_ANY);
		#ifdef MAPBAR_TOYOTA_262AVN_LINUX
		shm_Clean_Write(ptmep,strlen(ptmep));
		#endif
	out:	
		if(viapointArray)
			json_decref(viapointArray);
		/* 给主机应答处理 */
	}


	/**
	*	@brief
	*		VR添加途径点请求
	*	@return
	*		void
	*/
	
	void VRAddViaPoint(void * data)
	{
		int32_t errorcode = -1;
		DBUS_CMD_DATA* cmd = NULL;
		PoiFavorite fav;
		Point point;
		json_t* routePrefer;
		HybridRouteParam param;
		MbApi::RouteService::Plan routePlan;
		RoutePlanErrorCode routeplanerrorcode;
		#define VIA_POINT_MAX_NUMS 5
		#define PLAN_POINT_MAX_NUMS 7
		
		if(!data)
		{
			errorcode = 1;
			goto out;	
		}
		cmd = VRCmdDateInit(VRGetFavDomain((cqCHAR*)data));
		if(!cmd || cmd->argc != 3 )
		{
			errorcode = 1;
			goto out;
		}
		/* 根据经纬度信息,逆地理获取POI位置信息 */
		point.x = cq_atoi(cmd->argv[0])^EOR_KEY;
		point.y = cq_atoi(cmd->argv[1])^EOR_KEY;
		MAP_POI_SEARCH_SERVICE()->getPoiFavoriteByPosition(&point,&fav);
		/* 替换途径点名称信息 */
		cq_decodeUtf8(cmd->argv[2],SIZE_T_MAX,fav.name,sizeof(fav.name));
		// 1、如果当前在算路中，无条件取消当前算路
		if (MbApi::RouteService::getInstance()->isRouting())
		{
			MbApi::RouteService::getInstance()->cancel();
			CQ_LOG_INFO("[ServiceManager] VRPassingPoiAndDestPoiSigAnalysis MbApi::RouteService::getInstance()->cancel().\n");
		}
		
		// 2、判断当前plan能否添加成功
		routePlan = MbApi::RouteApi_getPlan();
		if(PLAN_POINT_MAX_NUMS == routePlan.getPointNum())
		{
			/* 当前途径点已经满了，无法添加 */
			errorcode = 0;
			goto out;	
		}
		else
		{
			routePlan.insertPoint(routePlan.getPointNum()-1, fav);
			/* 检查途径点和前一个的距离 */
			routeplanerrorcode = RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(),routePlan.getPointNum() - 2);
			if(routeplanerrorcode != RoutePlanErr_none)
			{
				
				errorcode = 1;
				goto out;
			}
		}
		
		// 3、如果当前在导航中，无条件取消导航
		if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState()
				|| RealNavigationState_pause == MbApi::Navi_getNavigationState())
		{
			//MbApi::Navi_reqCancelNavigation();
			MBAPI_VR_COMMAND_BACK_SIG* pCmdBackSig = (MBAPI_VR_COMMAND_BACK_SIG*)malloc(sizeof(MBAPI_VR_COMMAND_BACK_SIG));
			*pCmdBackSig = MBAPI_VR_COMMAND_DID_FAILURE;
			MbApi::MessageTransfer::getInstance()->navigationStateNotify(MBAPI_SM2C_EVENT_NAVI_CANCLE, NULL, pCmdBackSig);
			free(pCmdBackSig);
			pCmdBackSig = NULL;
			CQ_LOG_INFO("[ServiceManager] VRPassingPoiAndDestPoiSigAnalysis MbApi::Navi_reqCancelNavigation().\n");
		}

		/***********************VR新算路-携带路线偏好************************/
		// 备份routePlan
		g_VRLastRoutePlan = routePlan;
		// 获取路线偏好json数据
		routePrefer = VRDataLoadFromFile();
		// 将json转换为偏好对象
		if (NULL != routePrefer)
		{
			VRGetHybridRouteType(routePrefer, &param);
		}				
		// 设置开始导航Speaker标识
		MbApi::Navi_notifyNaviBeginSpeakInfo();
		// 开始算路
		VRHybridStartRoute(routePlan, param);
		// 释放json指针
		json_decref(routePrefer);
		/***********************VR新算路-携带路线偏好************************/
		
		// 4,标记当前算路为添加途径点发起
		VRStartRouteBitMask |= 0x01<<0;
		
	out:
		if(cmd)
		{
			VRCmdDateDeInit(cmd);
		}
		/* 根据errorcode 给主机应答*/
	}
	
}
