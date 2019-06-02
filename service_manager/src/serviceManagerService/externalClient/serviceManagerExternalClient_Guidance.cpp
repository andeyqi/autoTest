#include "stdafx_serviceManagerClient.h"
//#include "serviceManagerExternalClient_Guidance.h"
//#include "serviceManagerOutput.h"
//#include "serviceManagerEngine_Display.h"
#define INT32_MAX 4294967295
#define INT32_MIN -4294967296
#define CLIENT_CONFIG_FILE_JSON		RegionList_buildFileName(L"userdata/mapbar_client_data.cfg")
using namespace MbApi;

namespace ServiceManager
{
	//=====================================================//
	static GuidanceServiceClient* g_GuidanceServiceClient = NULL;
	static uint32 g_AddrStatus = 0;  // 保存快捷目的地状态（设置:0 or 未设置:1）。第0位代表家的信息，第1位代表公司的信息
	static NextTBTInfo g_lastTbtInfo;
	//=====================================================//
	// 函数声明
	static void GuidanceServiceClient_StartGuidance(void);
	static void GuidanceServiceClient_StopGuidance(void);
	static void GuidanceServiceClient_ReqNavi(void* data);
	void GuidanceServiceClient_AddrStatus(uint32 index, ADDR_STATUS status);
	static void updateRoadName(void);
	void sendRoadName(cqWCHAR* RoadName);
	BIT_Position TBTInfo_BackIcons(TurnDirection directId);
	BIT_Position TBTInfo_RotaryIcons(RotaryTurnDirection RotaryIconId);
	uint32 TBTInfo_SpecialIcon(uint32 trunIconId);

	// 将json数据转为路线偏好
	bool RoutePreferGetHybridRouteType(const json_t* json, HybridRouteParam* value);
	// 从文件中加载路线偏好
	json_t* RoutePreferDataLoadFromFile();

	/**
	*	@brief
	*		初始化
	*/
	void GuidanceServiceClient_Initialize(void)
	{
		if (NULL == g_GuidanceServiceClient)
		{
			g_GuidanceServiceClient = new GuidanceServiceClient();
			g_GuidanceServiceClient->initialize();
		}

		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:By first, send navi status.\n", __FUNCTION__, __LINE__);
		// “导航启动后会发送一次”导航状态
		if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState() ||
			RealNavigationState_pause == MbApi::Navi_getNavigationState())
			GuidanceServiceClient_StartGuidance();
		else if (RealNavigationState_none == MbApi::Navi_getNavigationState())
			GuidanceServiceClient_StopGuidance();

		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:By first, send shortcuts status.\n", __FUNCTION__, __LINE__);
		//“导航启动后会发送一次”家和公司的状态信息
		if (MbApi::NaviShortcuts_isValid(IDX_HOME))
			GuidanceServiceClient_AddrStatus(IDX_HOME, ADDR_ADD);
		else
			GuidanceServiceClient_AddrStatus(IDX_HOME, ADDR_DEL);
		if (MbApi::NaviShortcuts_isValid(IDX_OFFICE))
			GuidanceServiceClient_AddrStatus(IDX_OFFICE, ADDR_ADD);
		else
			GuidanceServiceClient_AddrStatus(IDX_OFFICE, ADDR_DEL);

		// 初次启动，获取当前的导航状态数据
		if (RealNavigationState_inNavigation != MbApi::Navi_getNavigationState()
			&& RealNavigationState_pause != MbApi::Navi_getNavigationState()
			&& !MbApi::Navi_isInSimulation()
			&& !MbApi::Navi_isPauseSimulation())
		{
			updateRoadName();
			// 自车位置，与client保持一致
			//NaviDataCenter* dc = MbApi::Session_getDataCenter();
			//SegmentGrabInfo info = { 0 };
			//DSegmentAttributes attrTmp;	//函数内使用
			//cqWCHAR roadName[MAX_SIZE] = { 0 };
			//uint32 i = MbApi::Map_grabSegments(&dc->m_carPos, &info, FALSE);//实际道路名，无名路也正常取
			//if (i == 0)
			//{
			//	if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
			//	{
			//		cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
			//	}
			//	else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
			//	{
			//		cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
			//	}
			//	updateRoadName(roadName);
			//}
			//else
			//{
			//	MbApi::RouteBaseApi_getDSegmentAttributes(info.dsegId, &attrTmp, MbApi::MbApiSegmentAttributesFlag_All);
			//	if (attrTmp.name[0] == 0)
			//	{
			//		if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
			//		{
			//			cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
			//		}
			//		else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
			//		{
			//			cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
			//		}
			//	}
			//	else
			//	{
			//		cq_wcscpy_s(roadName, element_of(attrTmp.name), attrTmp.name);
			//	}
			//	updateRoadName(roadName);
			//}
			//
			//g_GuidanceServiceClient->setCurrentRoadName(roadName, cq_wcslen(roadName) + 1);
			

			//const NaviSessionData* naviSessionData = MbApi::Navi_getNaviData();
			//// 当前道路名不为空
			//if (cq_wcscmp(naviSessionData->m_roadName, L""))
			//{
			//	CQ_LOG_INFO("[ServiceManager]:By first, got current road name %s.\n", naviSessionData->m_roadName);
			//	g_GuidanceServiceClient->setCurrentRoadName(naviSessionData->m_roadName, element_of(naviSessionData->m_roadName));
			//	updateRoadName(g_GuidanceServiceClient->getCurrentRoadName());
			//}
 		//	else if (naviSessionData->m_roadName[0] == 0) 
 		//	{
			//	cqWCHAR noneRoadName[MAX_SIZE] = { 0 };
			//	
			//	if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
			//	{
			//		cq_wcscat_s(noneRoadName, MAX_SIZE, L"无名路");
			//	}
			//	else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
			//	{
			//		cq_wcscat_s(noneRoadName, MAX_SIZE, L"No name road");
			//	}

			//	g_GuidanceServiceClient->setCurrentRoadName(noneRoadName, cq_wcslen(noneRoadName) + 1);
			//	updateRoadName(noneRoadName);
 		//	}
			//else
			//{
			//	// nothing to do
			//}
		}
		// 向Receiver注册callback(非同期)
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_AddrStatus, GuidanceServiceClient_ReqNavi);

		return;
	}

	/**
	*	@brief
	*		cleanup
	*/

	//delete by shenghao for exit timeout
	//void GuidanceServiceClient_CleanUp(void)
	//{
	//	CQ_LOG_INFO("[ServiceManager] GuidanceServiceClient_CleanUp.\n");
	//	if (NULL != g_GuidanceServiceClient)
	//	{
	//		delete g_GuidanceServiceClient;
	//		g_GuidanceServiceClient = NULL;
	//	}
	//	return;
	//}

	/*=================================guidance===================================*/
	/**
	*	@brief
	*		通知下一条路信息（非同期）
	*	@param [in]
	*		data 路名
	*	@return
	*		void
	*/
	//static void GuidanceServiceClient_nextRoadName(void* data)
	//{
	//	SendData* paramData = (SendData*)malloc(sizeof(SendData));
	//	paramData->funcId = FuncID_Send_UpdateNextRoadName;
	//	paramData->data = (cqWCHAR*)malloc(sizeof(cqWCHAR)* (cq_wcslen((cqWCHAR*)data) + 1));
	//	cq_wcscpy((cqWCHAR*)paramData->data, (cqWCHAR*)data);
	//	NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	//	return;
	//}

	/**
	*	@brief
	*		通知导航状态（非同期）：导航中
	*	@param [in]
	*		void
	*	@return
	*		void
	*/
	static void GuidanceServiceClient_StartGuidance(void)
	{
		//printf("[ServiceManager] GuidanceServiceClient_StartGuidance");
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:In navigation.\n", __FUNCTION__, __LINE__);
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_StartGuidance;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}

	/**
	*	@brief
	*		通知导航状态（非同期）：未导航
	*	@param [in]
	*		void
	*	@return
	*		void
	*/
	static void GuidanceServiceClient_StopGuidance(void)
	{
		//printf("[ServiceManager] GuidanceServiceClient_StopGuidance");
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:Not in navigation.\n", __FUNCTION__, __LINE__);
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_StopGuidance;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}
	/*=================================guidance===================================*/

	/*=================================路径规划===================================*/
	/**
	*	@brief
	*		路径规划中
	*	@param [in]
	*		void
	*	@return
	*		void
	*/
	static void GuidanceServiceClient_routing(void)
	{
		printf("[ServiceManager] GuidanceServiceClient_routing");
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_RoutingStatusRouting;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
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
	static void GuidanceServiceClient_complete(void* data)
	{
		printf("[ServiceManager] GuidanceServiceClient_complete");
		UNUSED_VAR(data);
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_RoutingStatusComplete;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
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
	static void GuidanceServiceClient_failed(void* data)
	{
		printf("[ServiceManager] GuidanceServiceClient_failed");
		UNUSED_VAR(data);
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_RoutingStatusFailed;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}
	/*=================================路径规划===================================*/

	/*===========================常用地址信息取得处理=============================*/
	/**
	*	@brief
	*		通知车机快捷目的地设置（非同期）
	*	@param [in] int
	*		index 快捷目的地下标
	*	@param [in] ADDR_STATUS
	*		status 快捷目的地状态
	*	@return
	*		void
	*/
	void GuidanceServiceClient_AddrStatus(uint32 index, ADDR_STATUS status)
	{
		CQ_LOG_INFO("[ServiceManager]:AddrStatus index %d, status %u.\n", index, status);
		// 第0位代表家的信息，第1位代表公司的信息
		if (ADDR_ADD == status)
			g_AddrStatus |= status << index;
		else if (ADDR_DEL == status)
		{
			if (IDX_HOME == index)
				g_AddrStatus &= index + 2;
			else if (IDX_OFFICE == index)
				g_AddrStatus &= index;
		}

		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:Send shorcuts status %u.\n", __FUNCTION__, __LINE__, g_AddrStatus);
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		paramData->funcId = FuncID_Send_AddrStatus;
		paramData->data = (uint32*)malloc(sizeof(uint32));
		memcpy(paramData->data, &g_AddrStatus, sizeof(uint32));
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		return;
	}

	// 快速导航 
	static void NaviShortcuts_startNavi(void* data)
	{
		int32 index = *(int32*)data;
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:Request navi, index %d.\n", __FUNCTION__, __LINE__, index);
		if (IDX_HOME != index && IDX_OFFICE != index)
		{
			CQ_LOG_ERROR("[ServiceManager]:Invalid index, please confirm!\n");
			return;
		}

		// 判断导航画面的情况，无条件取消
		// 1、如果当前正在算路，无条件取消算路
		if (MbApi::RouteService::getInstance()->isRouting())
		{
			MbApi::RouteService::getInstance()->cancel();
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
		}
		if (MbApi::Navi_isInSimulation() || MbApi::Navi_isPauseSimulation())
		{
			MbApi::Navi_reqCancelSimulation();
		}

		// 构造route plan
		MbApi::RouteService::Plan routePlan;

		// 获取自车位置设置为起点
		Point carPosition = (Point)MbApi::Map_getCarPosition();
		PoiFavorite startPosition;
		MbApi::PoiSrvQuery_getPoiFavoriteByPosition(&carPosition, &startPosition);
		routePlan.insertPoint(0, startPosition);  // 设置起点
		CQ_LOG_INFO("[ServiceManager]routePlan set start position.\n");

		// 获取目的地设置为终点
		PoiFavorite* endPosition = MbApi::NaviShortcuts_getFav(index);
		if (NULL == endPosition)
		{
			CQ_LOG_ERROR("[ServiceManager]:Position info is NULL!\n");
			return;
		}

		size_t pointNum = routePlan.getPointNum();    // routePlan中的所有点个数，包括起点、途经点、终点
		CQ_LOG_INFO("[ServiceManager]routePlan destNum is %d.\n", pointNum);
		if (0 == pointNum)
		{
			CQ_LOG_ERROR("[ServiceManager]:Route plan is error!\n");
			return;
		}
		routePlan.insertPoint(pointNum, *endPosition);  // 设置终点
		CQ_LOG_INFO("[ServiceManager]routePlan set end position.\n");

		// 判断route plan是否有效
		if (RoutePlanErr_none != RoutePlanApi_checkRoutePlan(routePlan.getRoutePlan(), 1))
		{
			CQ_LOG_INFO("[ServiceManager]Route plan is invalid!\n");
			return;
		}
		// 开始算路，同时开始导航
		MbApi::RouteService::getInstance()->route(routePlan, MbApi::RouteService::ResultMode_single, MbApi::ROUTE_SERVICE_USER_ID_SERVICE_MANAGER);
		CQ_LOG_INFO("[ServiceManager]start route.\n");

		return;
	}

	/**
	*	@brief
	*		车机请求导航到家/公司
	*	@param [in] data
	*		data 下标
	*	@return
	*		void
	*/
	static void GuidanceServiceClient_ReqNavi(void* data)
	{
		if (NULL != data)
		{
			MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)NaviShortcuts_startNavi, data);
		}
		return;
	}
	/*===========================常用地址信息取得处理=============================*/

	/**
	@brief			自车朝向、地图模式、目的地方向信息通知
	@param [in] 	void
	@return 		void
	*/
	static void NaviCarMapInfo(void* data)
	{
		CQ_LOG_INFO("[ServiceManger]Func:%s,Line:%d,Msg:signal is not NULL!\n", __FUNCTION__, __LINE__);
		UNUSED_VAR(data);
		if (MbApi::AppService::AppState_running == MbApi::AppService::getInstance()->getCurrentState())
		{
			// 自车信息数据结构
			DirectionInfo dirInfo;

			// 车头朝向变量
			int32 carAngle = 0;
			//int32 carBitPosition = -1;
			//int32 carDivAngle = 0;
			// 目的地朝向变量
			int32 destAngle = 0;
			//int32 destDivAngle = 0;
			//int32 destBitPositon = -1;

			//int32 infoPosition = 0;
			// 1、获取地图模式
			CQ_LOG_INFO("[ServiceManger]Func:%s,Line:%d,Msg:signal MapMode = %d\n", __FUNCTION__, __LINE__, MbApi::Map_getMapShowState());
			if (MapWnd_North == MbApi::Map_getMapShowState())
			{
				dirInfo.mapDirection = 1;
			}
			else
			{
				dirInfo.mapDirection = 2;
			}

			// 2、获取车头方向
			float oriAngle = MbApi::Map_getCarOrientation();
			if (oriAngle < 270)
				carAngle = int32(270 - oriAngle);
			else
				carAngle = int32((270 - oriAngle) + 360);

			//carBitPosition = int(carAngle * 100) / 1125;
			//carDivAngle = int(carAngle * 100) % 1125;

			//// 判断方向是否过半
			//if (carDivAngle * 10 >= 5625)
			//	carBitPosition++;

			dirInfo.carDirection = (uint32)carAngle;

			// 3、获取目的地方向
			// 判断是否在导航中
			if (RealNavigationState_inNavigation == MbApi::Navi_getNavigationState()
				|| RealNavigationState_pause == MbApi::Navi_getNavigationState()
				|| MbApi::Navi_isInSimulation()
				|| MbApi::Navi_isPauseSimulation())
			{
				// 1、获取自车位置坐标
				MapPoint carPoint = MbApi::Map_getCarPosition();
				// 2、获取目的地坐标
				MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
				//const RoutePlan* plan = MbApi::RouteApi_getPlan();
				size_t endIndex = plan.getPointNum() - 1;
				if (endIndex < 0 || endIndex > 6)
					return;
				if (NULL == plan.getPoint(0))
					return;
				MapPoint endPoint = plan.getPoint(endIndex)->pos;
				if ((endPoint.x > INT32_MAX) || (endPoint.x < INT32_MIN) || (endPoint.y > INT32_MAX) || (endPoint.y < INT32_MIN))
					return;
				// 3、计算两点角度
				int32 dy = endPoint.y - carPoint.y;
				int32 dx = endPoint.x - carPoint.x;
				if ((dy > INT32_MAX) || (dy < INT32_MIN) || (dx > INT32_MAX) || (dx < INT32_MIN))
					return;
				int16 SPAngle = 0;
				/*if (dy < 0)
					SPAngle = Math_atan(dy, dx) + 180;
				else if (dy == 0 && dx > 0)
					SPAngle = 0;
				else if (dy == 0 && dx < 0)
					SPAngle = 180;
				else*/
					SPAngle = Math_atan(dy, dx);

				// 计算目的地角度
				if (SPAngle <= 270)
					destAngle = 270 - SPAngle;
				else
					destAngle = (270 - SPAngle) + 360;

				//destBitPositon = int(destAngle * 100) / 1125;
				//destDivAngle = int(destAngle * 100) % 1125;

				//// 判断方向是否过半,过半则加一位
				//if (destDivAngle * 10 >= 5625)
				//	destBitPositon++;
			}
			else
			{
				//destBitPositon = -1;
				destAngle = -1;
			}
			dirInfo.destDirection = (int32)destAngle;

			// 发送消息
			SendData* paramData = (SendData*)malloc(sizeof(SendData));
			paramData->funcId = FuncID_Send_MeterDirectionInfo;
			paramData->data = (DirectionInfo*)malloc(sizeof(DirectionInfo));
			if (NULL == paramData->data)
			{
				CQ_LOG_INFO("[ServiceManager] NaviCarMapInfo paramData->data is NULL!\n");
				free(paramData);
				paramData = NULL;
				return;
			}
			CQ_LOG_INFO("[ServiceManger]Func:%s,Line:%d,Msg:signal dirInfo.mapDirection = %d ,carDirection = %d,destDirection = %d\n", __FUNCTION__, __LINE__, dirInfo.mapDirection, dirInfo.carDirection, dirInfo.destDirection);
			
			/*#ifdef MAPBAR_TOYOTA_262AVN_LINUX
			fs_drc_printf(SYS_ERR_LVL_LIGHT, 8, "[SM] mapDir[%d] ,carDir[%d],destDir[%d]", dirInfo.mapDirection, dirInfo.carDirection, dirInfo.destDirection);
			#endif*/
			
			memcpy(paramData->data, &dirInfo, sizeof(DirectionInfo));
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
		}
	}

	static void TBTInfo_isChanged(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_INFO("[ServiceManager] TBTInfo_isChanged NULL!\n");
			return;
		}

		// 向引擎请求前先判断引擎状态，防止出现引擎已开始析构，请求还在继续的错误出现
		if (MbApi::AppService::AppState_running == MbApi::AppService::getInstance()->getCurrentState())
		{
			NextTBTInfo nextTbtInfo;
			NaviSessionData* sessionData = (NaviSessionData*)data;
			// 路口出边背景路线
			int32 routeBackInfo = 0;
			int32 routeActualOut = 0;

			SendData* paramData = (SendData*)malloc(sizeof(SendData));
			if (NULL == paramData)
			{
				CQ_LOG_INFO("[ServiceManager] TBTInfo_isChanged paramData is NULL!\n");
				return;
			}

			paramData->funcId = FuncID_Send_MeterNextTBTInfo;
			paramData->data = (NextTBTInfo*)malloc(sizeof(NextTBTInfo));
			
			if (NULL == paramData->data)
			{
				CQ_LOG_INFO("[ServiceManager] TBTInfo_isChanged paramData->data is NULL!\n");
				free(paramData);
				return;
			}

			// 是否是环岛出口
			BOOL exitRotory = sessionData->m_iconInfo[0].isExitRotary;
			// 1、下个路线名称
			//nextTbtInfo.nextRoadName[64] = {0};//(cqWCHAR*)malloc(sizeof(cqWCHAR)* (cq_wcslen(sessionData->m_nextRoadName) + 1));
			if (0 == cq_wcscmp(sessionData->m_nextRoadName, L""))
			{
				free(paramData->data);
				paramData->data = NULL;
				free(paramData);
				paramData = NULL;
				return;
			}
			memcpy(nextTbtInfo.nextRoadName, sessionData->m_nextRoadName, sizeof(cqWCHAR)* (cq_wcslen(sessionData->m_nextRoadName) + 1));
			// 2、下个机动点距离
			nextTbtInfo.distance = sessionData->m_iconInfo[0].turnIconRemainDistance;
			// 3、单位 m
			//nextTbtInfo.unit = L'm';
			// 4、路口出入边32方位信息
			
			// 路口是否有16向信息
			BOOL is16TurnIcon = sessionData->m_iconInfo[0].isOffLineTurnIcons;
			// 环岛是否有16向信息
			BOOL is16Rotary = sessionData->m_iconInfo[0].isOffLineTurnIconsOnRotary;

			TNaviTurnIconID turnIcon = sessionData->m_iconInfo[0].turnIcon;

			// 取得16向路口信息
			TurnIcons turnIcons = sessionData->m_iconInfo[0].turnIcons;
			// 取得16向环岛信息
			TurnIconsOnRotary iconRotary = sessionData->m_iconInfo[0].turnIconsOnRotary;
			
			// 特殊转向标：左掉头、右掉头
			ENUM_SPECIAL_ICON specialIcon = ENUM_SPECIAL_ICON_NONE;
			//if (turnIcons.turnIconsCnt == 1 && iconRotary.turnIconsCnt == 0)
			if (is16TurnIcon)
			{

				switch (turnIcons.turnIconsSection[0])
				{
				// 非同路左掉头
				case TurnDirection_127:
				case TurnDirection_128:
				case TurnDirection_129:
					specialIcon = ENUM_SPECIAL_ICON_UTURN_LEFT;
					break;
				// 非同路右掉头
				case TurnDirection_133:
				case TurnDirection_134:
				case TurnDirection_135:
					specialIcon = ENUM_SPECIAL_ICON_UTURN_RIGHT;
					break;
				// 道路尽头左转- 拆解为16向
				case TurnDirection_130:
				{
					turnIcons.turnIconsSection[0] = TurnDirection_115;
					turnIcons.turnIconsSection[1] = TurnDirection_123;
					turnIcons.turnIconsSection[2] = TurnDirection_108;
					turnIcons.turnIconsCnt = 3;
				}
						
					
					break;
				// 道路尽头右转- 拆解为16向
				case TurnDirection_131:
				{
					turnIcons.turnIconsSection[0] = TurnDirection_115;
					turnIcons.turnIconsSection[1] = TurnDirection_123;
					turnIcons.turnIconsSection[2] = TurnDirection_100;
					turnIcons.turnIconsCnt = 3;
				}
					break;
				default:
					break;
				}
			}
			// 特殊图标：途径点、目的地
			else if (!is16TurnIcon && !is16Rotary)	// 非16向图标
			{
				specialIcon = (ENUM_SPECIAL_ICON)TBTInfo_SpecialIcon(turnIcon);
			}

			// 8、特殊转向标
			nextTbtInfo.specialIcon = specialIcon;

			// 32向路口背景路线
			if (is16TurnIcon)
			{
				for (size_t cnt = 0; cnt < turnIcons.turnIconsCnt; cnt++)
				{
					if (cnt < turnIcons.turnIconsCnt - 1)
					{
						// 1、转换路口道路信息
						int32 bitPosition = TBTInfo_BackIcons(turnIcons.turnIconsSection[cnt]);
						if (bitPosition < BIT_Position_invalid)
							routeBackInfo = routeBackInfo | (1 << bitPosition);
					}
					else
					{
						// 2、实际出边路线信息
						int32 bitPosition = TBTInfo_BackIcons(turnIcons.turnIconsSection[cnt]);
						if (bitPosition < BIT_Position_invalid)
							routeActualOut = routeActualOut | (1 << bitPosition);
					}
				}
			}
			
			// 5、是否有环岛 0无，1有
			
			if (is16Rotary) // 有环岛
			{
				if (iconRotary.turnIconsCnt == 1)
				{
					specialIcon = (ENUM_SPECIAL_ICON)TBTInfo_SpecialIcon(iconRotary.turnIconsSection[0]);
					nextTbtInfo.isOnRotary = 0;
				}
				else
				{
					for (size_t cnt = 0; cnt < iconRotary.turnIconsCnt; cnt++)
					{
						// 1、转换环岛路口
						if (cnt < iconRotary.turnIconsCnt - 1)
						{
							int32 bitPosition = TBTInfo_RotaryIcons(iconRotary.turnIconsSection[cnt]);
							if (bitPosition < BIT_Position_invalid)
								routeBackInfo = routeBackInfo | (1 << bitPosition);
						}
						else
						{
							// 2、实际环岛出边
							int32 bitPosition = TBTInfo_RotaryIcons(iconRotary.turnIconsSection[cnt]);
							if (bitPosition < BIT_Position_invalid)
								routeActualOut = routeActualOut | (1 << bitPosition);
						}
					}
					nextTbtInfo.isOnRotary = 1;				// 参数5 有环岛
				}				
			}
			else // 无环岛
			{
				nextTbtInfo.isOnRotary = 0;				// 参数5 无环岛
			}

			// 如果下个路口是环岛出口，并且16向环岛方向count==0
			if (exitRotory && sessionData->m_iconInfo[0].turnIconsOnRotary.turnIconsCnt == 0)
			{
				nextTbtInfo.routeBackInfo = g_lastTbtInfo.routeBackInfo;
				nextTbtInfo.actualOutLine = g_lastTbtInfo.actualOutLine;
				nextTbtInfo.specialIcon = g_lastTbtInfo.specialIcon;
			}
			else
			{
				nextTbtInfo.routeBackInfo = routeBackInfo;	// 参数4 路口路线信息赋值
				nextTbtInfo.actualOutLine = routeActualOut;	// 参数7 实际出边赋值
				nextTbtInfo.specialIcon = specialIcon;
			}

			// 6、环岛内外向 0外向，1内向 暂时不支持，默认是外向
			nextTbtInfo.isOutSide = 0;

			// 7、距终点剩余距离(m)
			nextTbtInfo.remainingDistance = (sessionData->m_routeLength - sessionData->m_travelledDistance);
			// 8、距终点剩余时间(s)
			if (0 == sessionData->m_travelledDistance && MbApi::RouteApi_getRoute() != NULL)
				nextTbtInfo.remainingTime = MbApi::RouteBaseApi_getEstimatedTime(MbApi::RouteApi_getRoute());
			else
				nextTbtInfo.remainingTime = MbApi::Eta_getRemainingTime();
			
			//wprintf(L"%ls\n", nextTbtInfo.nextRoadName);
			// 备份下条路线信息
			memcpy(&g_lastTbtInfo, &nextTbtInfo, sizeof(NextTBTInfo));
			memcpy(paramData->data, &nextTbtInfo, sizeof(NextTBTInfo));		
			NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);			
		}

		return;
	}

	/**
	@brief			8向图标转换特殊转向标
	@param [in] 	TNaviTurnIconID
	trunIconId   8向ID
	@return 		特殊转向标
	*/
	uint32 TBTInfo_SpecialIcon(uint32 trunIconId)
	{
		switch (trunIconId)
		{
		case ENTI_Arrival:
			// 目的地
			return ENUM_SPECIAL_ICON_DEST_FLAG;
		case ENTI_TurnAround:
			// 同路左掉头
			return ENUM_SPECIAL_ICON_UTURN_LEFT;
		case ENTI_WayPoints1:
			return ENUM_SPECIAL_ICON_WAYPOINTS1;
		case ENTI_WayPoints2:
			return ENUM_SPECIAL_ICON_WAYPOINTS2;
		case ENTI_WayPoints3:
			return ENUM_SPECIAL_ICON_WAYPOINTS3;
		case ENTI_WayPoints4:
			return ENUM_SPECIAL_ICON_WAYPOINTS4;
		case ENTI_WayPoints5:
			return ENUM_SPECIAL_ICON_WAYPOINTS5;
			// 途径点
			//return ENUM_SPECIAL_ICON_VIA_FLAG;
		case 1002:
			// 同路右掉头
			return ENUM_SPECIAL_ICON_UTURN_RIGHT;
		case ENTI_EnterTollStation:
			return ENUM_SPECIAL_ICON_TOLL_STATION;
		case ENTI_EnterTunnel:
			return ENUM_SPECIAL_ICON_TUNNEL;
		case RotaryTurnDirection_RightHand_enterRotary:
			return ENUM_SPECIAL_ICON_DEST_ON_ROTARY_RIGHT_HAND;
		case RotaryTurnDirection_RightHand_exitRotary:
			return ENUM_SPECIAL_ICON_START_ON_ROTARY_RIGHT_HAND;
		case RotaryTurnDirection_LeftHand_enterRotary:
			return ENUM_SPECIAL_ICON_DEST_ON_ROTARY_LEFT_HAND;
		case RotaryTurnDirection_LeftHand_exitRotary:
			return ENUM_SPECIAL_ICON_START_ON_ROTARY_LEFT_HAND;
		default:
			break;
		}
		return ENUM_SPECIAL_ICON_NONE;
	}

	/**
	@brief			解析机动点路口16向背景图ID
	@param [in] 	TurnDirection
	directId   16向ID
	@return 		32向 bit position
	*/
	BIT_Position TBTInfo_BackIcons(TurnDirection directId)
	{
		switch (directId)
		{
		case TurnDirection_97:
		case TurnDirection_112:
			return BIT_Position_1;
		case TurnDirection_98:
		case TurnDirection_113:
			return BIT_Position_3;
		case TurnDirection_99:
		case TurnDirection_114:
			return BIT_Position_5;
		case TurnDirection_100:
		case TurnDirection_115:
			return BIT_Position_7;
		case TurnDirection_101:
		case TurnDirection_116:
			return BIT_Position_9;
		case TurnDirection_102:
		case TurnDirection_117:
			return BIT_Position_11;
		case TurnDirection_103:
		case TurnDirection_118:
			return BIT_Position_13;
		case TurnDirection_104:
		case TurnDirection_119:
			return BIT_Position_15;
		case TurnDirection_105:
		case TurnDirection_120:
			return BIT_Position_17;
		case TurnDirection_106:
		case TurnDirection_121:
			return BIT_Position_19;
		case TurnDirection_107:
		case TurnDirection_122:
			return BIT_Position_21;
		case TurnDirection_108:
		case TurnDirection_123:
			return BIT_Position_23;
		case TurnDirection_109:
		case TurnDirection_124:
			return BIT_Position_25;
		case TurnDirection_110:
		case TurnDirection_125:
			return BIT_Position_27;
		case TurnDirection_111:
		case TurnDirection_126:
			return BIT_Position_29;
		default:
			break;
		}
		return BIT_Position_invalid;
	}

	BIT_Position TBTInfo_RotaryIcons(RotaryTurnDirection RotaryIconId)
	{
		switch (RotaryIconId)
		{
		case RotaryTurnDirection_1:
		case RotaryTurnDirection_RightHand_1:
		case RotaryTurnDirection_LeftHand_15:
			return BIT_Position_1;

		case RotaryTurnDirection_2:
		case RotaryTurnDirection_RightHand_2:
		case RotaryTurnDirection_LeftHand_14:
			return BIT_Position_3;

		case RotaryTurnDirection_3:
		case RotaryTurnDirection_RightHand_3:
		case RotaryTurnDirection_LeftHand_13:
			return BIT_Position_5;

		case RotaryTurnDirection_4:
		case RotaryTurnDirection_RightHand_4:
		case RotaryTurnDirection_LeftHand_12:
			return BIT_Position_7;

		case RotaryTurnDirection_5:
		case RotaryTurnDirection_RightHand_5:
		case RotaryTurnDirection_LeftHand_11:
			return BIT_Position_9;

		case RotaryTurnDirection_6:
		case RotaryTurnDirection_RightHand_6:
		case RotaryTurnDirection_LeftHand_10:
			return BIT_Position_11;

		case RotaryTurnDirection_7:
		case RotaryTurnDirection_RightHand_7:
		case RotaryTurnDirection_LeftHand_9:
			return BIT_Position_13;

		case RotaryTurnDirection_8:
		case RotaryTurnDirection_RightHand_8:
		case RotaryTurnDirection_LeftHand_8:
			return BIT_Position_15;

		case RotaryTurnDirection_9:
		case RotaryTurnDirection_RightHand_9:
		case RotaryTurnDirection_LeftHand_7:
			return BIT_Position_17;

		case RotaryTurnDirection_10:
		case RotaryTurnDirection_RightHand_10:
		case RotaryTurnDirection_LeftHand_6:
			return BIT_Position_19;

		case RotaryTurnDirection_11:
		case RotaryTurnDirection_RightHand_11:
		case RotaryTurnDirection_LeftHand_5:
			return BIT_Position_21;

		case RotaryTurnDirection_12:
		case RotaryTurnDirection_RightHand_12:
		case RotaryTurnDirection_LeftHand_4:
			return BIT_Position_23;

		case RotaryTurnDirection_13:
		case RotaryTurnDirection_RightHand_13:
		case RotaryTurnDirection_LeftHand_3:
			return BIT_Position_25;

		case RotaryTurnDirection_14:
		case RotaryTurnDirection_RightHand_14:
		case RotaryTurnDirection_LeftHand_2:
			return BIT_Position_27;

		case RotaryTurnDirection_15:
		case RotaryTurnDirection_RightHand_15:
		case RotaryTurnDirection_LeftHand_1:
			return BIT_Position_29;
		default:
			break;
		}
		return BIT_Position_invalid;
	}

	/*========================3.4 turn by turn信息通知============================*/
	/*==========================3.5 自车位置信息通知==============================*/
	/**
	@brief			turn by turn信息变化通知
	@param [in] 	void
					data 导航状态数据
	@return 		void
	*/
	//static void TBTInfo_isChanged(void* data)
	//{
	//	if (NULL == data)
	//	{
	//		CQ_LOG_ERROR("[ServiceManager] TBTInfo_isChanged NULL!\n");
	//		return;
	//	}
	//	// fix by liubo
	//	// 向引擎请求前先判断引擎状态，防止出现引擎已开始析构，请求还在继续的错误出现
	//	if (MbApi::AppService::AppState_running == MbApi::AppService::getInstance()->getCurrentState())
	//	{
	//		SendData* paramData = (SendData*)malloc(sizeof(SendData));
	//		if (NULL == paramData)
	//		{
	//			CQ_LOG_ERROR("[ServiceManager] TBTInfo_isChanged paramData is NULL!\n");
	//			return;
	//		}
	//		
	//		paramData->funcId = FuncID_Send_UpdateTBTInfo;
	//		paramData->data = (TBTInfo*)malloc(sizeof(TBTInfo));
	//		if (NULL == paramData->data)
	//		{
	//			CQ_LOG_ERROR("[ServiceManager] TBTInfo_isChanged paramData->data is NULL!\n");
	//			free(paramData);
	//			return;
	//		}
	//		
	//		((TBTInfo*)paramData->data)->turnIcon = ((NaviSessionData*)data)->m_iconInfo[0].turnIcon;										//当前需要显示的转向标图标ID
	//		((TBTInfo*)paramData->data)->turnIconsOnRotary = ((NaviSessionData*)data)->m_iconInfo[0].turnIconsOnRotary; 					//当前需要显示的环岛转向标图标ID
	//		((TBTInfo*)paramData->data)->isOffLineTurnIcons = ((NaviSessionData*)data)->m_iconInfo[0].isOffLineTurnIcons;					//是否是离线转向标
	//		((TBTInfo*)paramData->data)->isOffLineTurnIconsOnRotary = ((NaviSessionData*)data)->m_iconInfo[0].isOffLineTurnIconsOnRotary;	//是否是环岛离线转向标
	//		((TBTInfo*)paramData->data)->turnIconRemainDistance = ((NaviSessionData*)data)->m_iconInfo[0].turnIconRemainDistance;			//显示在转向标上的距离值，单位：米。
	//		((TBTInfo*)paramData->data)->routeLength = ((NaviSessionData*)data)->m_routeLength; 											//路线的总长度，单位：米。
	//		((TBTInfo*)paramData->data)->travelledDistance = ((NaviSessionData*)data)->m_travelledDistance; 								//自车在路线上已经行驶过的距离，单位：米。
	//		if (0 == ((NaviSessionData*)data)->m_travelledDistance && MbApi::RouteApi_getRoute() != NULL)
	//			((TBTInfo*)paramData->data)->remainingTime = MbApi::RouteBaseApi_getEstimatedTime(MbApi::RouteApi_getRoute());
	//		else
	//			((TBTInfo*)paramData->data)->remainingTime = MbApi::Eta_getRemainingTime();
	//		if (ENTI_None == ((NaviSessionData*)data)->m_iconInfo[0].turnIcon && 0 == ((NaviSessionData*)data)->m_iconInfo[0].turnIconRemainDistance)
	//		{
	//			free(paramData->data);
	//			free(paramData);
	//			return;
	//		}

	//		//ServiceManager::NaviData naviData;
	//		//naviData.DebusInfo.tbtId = (int32)((NaviSessionData*)data)->m_iconInfo[0].turnIcon;
	//		//ServiceManagerOutput::getInstance()->outputLog(NaviDataType_tbt, &naviData);
	//		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	//	}
	//	
	//	return;
	//}

	/**
	@brief			当前道路名称通知
	@param [in] 	void
					data 导航状态数据
	@return 		void
	*/
	static void updateRoadName(void)
	{

		// 自车位置，与client保持一致
		NaviDataCenter* dc = MbApi::Session_getDataCenter();
		SegmentGrabInfo info = { 0 };
		DSegmentAttributes attrTmp;	//函数内使用
		cqWCHAR roadName[MAX_SIZE] = { 0 };
		uint32 i = MbApi::Map_grabSegments(&dc->m_carPos, &info, FALSE);//实际道路名，无名路也正常取
		if (i == 0)
		{
			if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
			{
				cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
			}
			else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
			{
				cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
			}
		}
		else
		{
			MbApi::RouteBaseApi_getDSegmentAttributes(info.dsegId, &attrTmp, MbApi::MbApiSegmentAttributesFlag_All);
			if (attrTmp.name[0] == 0)
			{
				if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
				{
					cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
				}
				else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
				{
					cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
				}
			}
			else
			{
				cq_wcscpy_s(roadName, element_of(attrTmp.name), attrTmp.name);
			}
		}

		sendRoadName(roadName);

		return;
	}

	void sendRoadName(cqWCHAR* RoadName)
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
	/*========================3.4 turn by turn信息通知============================*/
	/*==========================3.5 自车位置信息通知==============================*/

	/**
	*	@brief
	*		构造函数
	*/
	GuidanceServiceClient::GuidanceServiceClient()
	{
		// 下一条路名
		m_nextRoadName = (cqWCHAR*)malloc(sizeof(MbApi::Navi_getNaviData()->m_nextRoadName));
		memset(m_nextRoadName, 0, sizeof(MbApi::Navi_getNaviData()->m_nextRoadName));
		cq_wcscpy(m_nextRoadName, MbApi::Navi_getNaviData()->m_nextRoadName);

		m_currentRoadName = (cqWCHAR*)malloc(sizeof(MbApi::Navi_getNaviData()->m_roadName));
		memset(m_currentRoadName, 0, sizeof(MbApi::Navi_getNaviData()->m_roadName));
		cq_wcscpy(m_currentRoadName, MbApi::Navi_getNaviData()->m_roadName);

		m_NaviDataTimer = invalidHandle;
		m_NaviCarMapInfoTimer = invalidHandle;
	}

	/**
	*	@brief
	*		析构函数
	*/
	GuidanceServiceClient::~GuidanceServiceClient()
	{
		// 下一条路名
		if (NULL != m_nextRoadName)
		{
			free(m_nextRoadName);
			m_nextRoadName = NULL;
		}

		if (NULL != m_currentRoadName)
		{
			free(m_currentRoadName);
			m_currentRoadName = NULL;
		}
		if (m_NaviCarMapInfoTimer != invalidHandle)
		{
			MbApi::Stop_timer(m_NaviCarMapInfoTimer);
			m_NaviCarMapInfoTimer = invalidHandle;
		}
	}

	/**
	*	@brief
	*		initialize
	*/
	void GuidanceServiceClient::initialize()
	{
		MbApi::MessageDistribution::getInstance()->addGeneralEventListener(MbApi::MessageDistribution::Level_medium, this);
		MbApi::MessageDistribution::getInstance()->addNaviSessionEventListener(MbApi::MessageDistribution::Level_medium, this);
		MbApi::MessageDistribution::getInstance()->addNavigationStateListener(MbApi::MessageDistribution::Level_medium, this);
		MbApi::MessageDistribution::getInstance()->addMapViewListener(MbApi::MessageDistribution::Level_medium, this);
		// 启动自车位置信息Timer
		m_NaviCarMapInfoTimer = MbApi::Start_timer(invalidHandle, TIMER_TBT_1_SECOND, NaviCarMapInfo, NULL);
		return;
	}

	BOOL GuidanceServiceClient::onNaviSessionEventNotify(MbApi::MessageDistribution::NaviSessionEvent event, void* data)
	{
		switch (event)
		{
		case MbApi::MessageDistribution::NaviSessionEvent_naviBegin:
		case MbApi::MessageDistribution::NaviSessionEvent_simNaviBegin:
		{
			// 导航状态信息通知：导航中
			GuidanceServiceClient_StartGuidance();
			// 先发送一次仪表信息，防止timer 1s延迟 - Bug #4208
			TBTInfo_isChanged((void*)MbApi::Navi_getNaviData());
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:naviBegin or simNaviBegin send navi status.\n", __FUNCTION__, __LINE__);
			// 诱导启动和模拟导航启动，每1s发送导航状态数据
			if (m_NaviDataTimer == invalidHandle)
			{
				m_NaviDataTimer = MbApi::Start_timer(invalidHandle, TIMER_TBT_1_SECOND, TBTInfo_isChanged, (void*)MbApi::Navi_getNaviData());
			}
		}
			break;
		case MbApi::MessageDistribution::NaviSessionEvent_destArrived:
		//case MbApi::MessageDistribution::NaviSessionEvent_routeRemoved:  // 结束导航&取消路线
		case MbApi::MessageDistribution::NaviSessionEvent_simNaviEnd:
		case MbApi::MessageDistribution::NaviSessionEvent_naviCancel:
		{
			// 导航状态信息通知：未导航
			GuidanceServiceClient_StopGuidance();
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:simNaviEnd or naviCancel send navi status.\n", __FUNCTION__, __LINE__);

			if (m_NaviDataTimer != invalidHandle)
			{
				MbApi::Stop_timer(m_NaviDataTimer);
				m_NaviDataTimer = invalidHandle;		
			}

			updateRoadName();
		}
			break;
		case MbApi::MessageDistribution::NaviSessionEvent_tracking:
		{
			// 前提：未导航中 
			// 比较前后两次m_roadName，如果不同则通知车机
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_tracking.\n", __FUNCTION__, __LINE__);
			if (RealNavigationState_inNavigation != MbApi::Navi_getNavigationState()
 				&& RealNavigationState_pause != MbApi::Navi_getNavigationState()
 				&& !MbApi::Navi_isInSimulation()
 				&& !MbApi::Navi_isPauseSimulation())
 			{
				
				CQ_LOG_INFO("[ServiceManager]:Tracking in.Current road name is %s.\n", ((NaviSessionData*)data)->m_roadName);
				NaviDataCenter* dc = MbApi::Session_getDataCenter();
				SegmentGrabInfo info = { 0 };
				DSegmentAttributes attrTmp;	//函数内使用
				cqWCHAR roadName[MAX_SIZE] = { 0 };
				uint32 i = MbApi::Map_grabSegments(&dc->m_carPos, &info, FALSE);//实际道路名，无名路也正常取
				if (i == 0)
				{
					if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
					{
						cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
					}
					else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
					{
						cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
					}

					if (cq_wcscmp(m_currentRoadName, roadName))
					{
						cq_wcscpy_s(m_currentRoadName, cq_wcslen(roadName) + 1, roadName);
						sendRoadName(m_currentRoadName);
					}

				}
				else
				{
					MbApi::RouteBaseApi_getDSegmentAttributes(info.dsegId, &attrTmp, MbApi::MbApiSegmentAttributesFlag_All);
					if (attrTmp.name[0] == 0)
					{
						if (ALanguage_chs == SETTING_SERVICE()->getLanguage())
						{
							cq_wcscat_s(roadName, MAX_SIZE, L"无名路");
						}
						else if (ALanguage_enu == SETTING_SERVICE()->getLanguage())
						{
							cq_wcscat_s(roadName, MAX_SIZE, L"No name road");
						}

						if (cq_wcscmp(m_currentRoadName, roadName))
						{
							cq_wcscpy_s(m_currentRoadName, cq_wcslen(roadName) + 1, roadName);
							sendRoadName(m_currentRoadName);
						}
					}
					else
					{
						if (cq_wcscmp(m_currentRoadName, attrTmp.name))
						{
							cq_wcscpy_s(m_currentRoadName, element_of(attrTmp.name), attrTmp.name);
							sendRoadName(m_currentRoadName);
						}
						
					}
					
				}
			}
			// 比较前后两次nextRoadName，如果不同则通知车机
			//if (cq_wcsncmp(m_nextRoadName, ((NaviSessionData*)data)->m_nextRoadName, cq_wcslen(((NaviSessionData*)data)->m_nextRoadName)))
			//	GuidanceServiceClient_nextRoadName((void*)m_nextRoadName);
		}
			break;
		// 路径规划信息通知
		case MbApi::MessageDistribution::NaviSessionEvent_routing:
		case MbApi::MessageDistribution::NaviSessionEvent_sm_routing:
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_routing.\n", __FUNCTION__, __LINE__);
			GuidanceServiceClient_routing();
			break;
		case MbApi::MessageDistribution::NaviSessionEvent_routeComplete:
		case MbApi::MessageDistribution::NaviSessionEvent_sm_routeComplete:
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_routeComplete.\n", __FUNCTION__, __LINE__);
			GuidanceServiceClient_complete(data);
			break;
		case MbApi::MessageDistribution::NaviSessionEvent_routeFailed:
		//case MbApi::MessageDistribution::NaviSessionEvent_sm_routeFailed:
		case MbApi::MessageDistribution::NaviSessionEvent_routeCancelled:
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_routeFailed.\n", __FUNCTION__, __LINE__);
			// TYTAVNNEW-424 如果正在算路中，丢弃本次结果
			if (MbApi::RouteService::getInstance()->isRouting())
			{
				CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Msg:NaviSessionEvent_sm_routeFailed but is isRouting return.\n", __FUNCTION__, __LINE__);
				//MbApi::RouteService::getInstance()->setRouting(FALSE);
				return true;
			}
			GuidanceServiceClient_failed(data);
			break;
		// ServiceManager发起的算路失败，需要判断
		case MbApi::MessageDistribution::NaviSessionEvent_sm_routeFailed:
			{
				switch (((RouterErrorInfo*)data)->errCode)
				{
					case ERouterError_NetworkError:
					case ERouterError_onlineCalcTimeout:
					{
						json_t* routePrefer = RoutePreferDataLoadFromFile();
						HybridRouteParam routeParam;
						// 将json转换为偏好对象
						if (NULL != routePrefer)
						{
							RoutePreferGetHybridRouteType(routePrefer, &routeParam);
						}
						if (routeParam.operator[](HybridRouteType_avoidJam))
						{
							// 释放json指针	
							json_decref(routePrefer);
							return true;
						}
						json_decref(routePrefer);
					}
						break;
					default:
						break;
				}
				GuidanceServiceClient_failed(data);
			}
			break;
		default:
			break;
		}

		return true;
	}
	BOOL GuidanceServiceClient::onNavigationStateNotify(RealNavigationState state)
	{
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:onNavigationStateNotify state = %d.\n", __FUNCTION__, __LINE__, state);
		switch (state)
		{
		case RealNavigationState_none:
			break;
		case RealNavigationState_inNavigation:
			break;
		case RealNavigationState_pause:
			GuidanceServiceClient_StopGuidance();
			if (m_NaviDataTimer != invalidHandle)
			{
				MbApi::Stop_timer(m_NaviDataTimer);
				m_NaviDataTimer = invalidHandle;
			}
			updateRoadName();
			break;
		case RealNavigationState_max:
			break;
		default:
			break;
		}
		return true;
	}

	BOOL GuidanceServiceClient::onGeneralEventNotify(MbApi::MessageDistribution::GeneralEvent event, void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] onGeneralEventNotify data NULL!\n");
			return false;
		}
		int index = *((int*)data);
		switch (event)
		{
		// 常用地址信息状态
		case MbApi::MessageDistribution::GeneralEvent_shortcutsAdded:
			GuidanceServiceClient_AddrStatus(index, ADDR_ADD);
			break;
		case MbApi::MessageDistribution::GeneralEvent_shortcutsRemoved:
			GuidanceServiceClient_AddrStatus(index, ADDR_DEL);
			break;
		case MbApi::MessageDistribution::GeneralEvent_languageChanged:
			{
				if (RealNavigationState_inNavigation != MbApi::Navi_getNavigationState()
				&& RealNavigationState_pause != MbApi::Navi_getNavigationState()
				&& !MbApi::Navi_isInSimulation()
				&& !MbApi::Navi_isPauseSimulation())
				{
					updateRoadName();	
				}
			}
			break;
		default:
			break;
		}
		return true;
	}
	
	BOOL GuidanceServiceClient::onMapViewEventNotify(MessageDistribution::MapViewEvent event, void* data)
	{
		switch (event)
		{
		case MbApi::MessageDistribution::MapViewEvent_setLockCar:
			if (RealNavigationState_inNavigation != MbApi::Navi_getNavigationState()
				&& RealNavigationState_pause != MbApi::Navi_getNavigationState()
				&& !MbApi::Navi_isInSimulation()
				&& !MbApi::Navi_isPauseSimulation())
			{
				if (true == *((bool*)data))
				{
					updateRoadName();
				}
			}
			break;
		default:
			break;
		}
		return true;
	}

	/**
	*	@brief
	*		从文件中load路线偏好
	*	@return
	*		void
	*/
	json_t* RoutePreferDataLoadFromFile()
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
	bool RoutePreferGetHybridRouteType(const json_t* json, HybridRouteParam* value)
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
}

