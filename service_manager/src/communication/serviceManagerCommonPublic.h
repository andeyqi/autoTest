/***************************************************
	file name:	serviceManagerCommonPublic.h
	Create Data:	2017/06/28
***************************************************/
#ifndef	__SERVICE_MANAGER_COMMON_PUBLIC_H__
#define __SERVICE_MANAGER_COMMON_PUBLIC_H__
#define INVALIDE_VALUE 0xffffffff;
typedef void (*ReceiverCallback)(void *data);

namespace ServiceManager
{
	enum SERVICE_MANAGER_SEND_FUNC_ID
	{
		FuncID_Send_NaviStartSuc,
		FuncID_Send_NaviStopSuc,
		FuncID_Send_GPSRcvSuc,
		FuncID_Send_CurrentVoiceState,
		FuncID_Send_ReqVoiceStart,
		FuncID_Send_ReqVoiceStop,
		FuncID_Send_ReqPlayBeepSound,
		FuncID_Send_ReqShowOtherForm,
		FuncID_Send_ReqHideOtherForm,
		FuncID_Send_ShowNaviFormSuc,
		FuncID_Send_HideNaviFormSuc,
		FuncID_Send_CautionShow,
		FuncID_Send_ReqCheckILLState,
		FuncID_Send_RepILLStateChanged,
		FuncID_Send_NaviACCOFFSuc,
		FuncID_Send_CurrentGPSTimeValue,
		FuncID_Send_CurrentGPSTimeValueFail,
		FuncID_Send_UpdateTBTInfo,		
		FuncID_Send_UpdateRoadName,
		FuncID_Send_StartGuidance,
		FuncID_Send_StopGuidance,
		FuncID_Send_UpdateNextRoadName,
		FuncID_Send_AddrStatus,
		FuncID_Send_getUUID,
		FuncID_Send_SetGuidance,
		FuncID_Send_BgSwt,
		FuncID_Send_NotifyNaviLiveStatus,
		FuncID_Send_HistoryClear,
		FuncID_Send_RunStatusRequest,
		FuncID_Send_ReqUsbStatus,
		FuncID_Send_LanguageSwitchSuccess,
		FuncID_Send_LanguageSwitchFailure,
		FuncID_Send_getNetworkStatus,
		//FuncID_Send_getRevStatus,
		FuncID_Send_getGbookInfo,
		// DeSai send function id
		FuncID_Send_SetVolume,
		FuncID_Send_GetVolume,
		FuncID_Send_GetBackLight,
		FuncID_Send_BackToHost,
		FuncID_Send_ShowHostMenu,
		FuncID_Send_RequestAudioGuidancePlay,
		FuncID_Send_ReleaseAudioGuidancePlay,
		FuncID_Send_GetLanguageID,
		FuncID_Send_IsGPSUpdateTimeEnabled,
		FuncID_Send_SetRouteStatus,
		FuncID_Send_IsCarPlayNaviActive,


		// toyota send function id
		FuncID_Send_RoutingStatusRouting,
		FuncID_Send_RoutingStatusFailed,
		FuncID_Send_RoutingStatusComplete,
		FuncID_Send_ReqShowMusicWidget,
		FuncID_Send_ReqHideMusicWidget,
		FuncID_Send_ReqSysHmiTime,
		FuncID_Send_ReadyForForgroud,
		FuncID_Send_GetDayNightMode,
		FuncID_Send_SetDayNightMode,
		// VR FUNC ID
		FuncID_Send_CommandControlSucess,
		FuncID_Send_CommandControlFailure,
		FuncID_Send_CommandTrafficSucess,
		FuncID_Send_CommandTrafficFailure,
		FuncID_Send_PassingPointSucess,
		FuncID_Send_PassingPointFailure,
		FuncID_Send_SwitchRouteSucess,
		FuncID_Send_SwitchRouteFailure,
		FuncID_Send_ItemSelectSucess,
		FuncID_Send_ItemSelectFailure,
		FuncID_Send_InquiryDistanceSuccess,
		FuncID_Send_InquiryDistanceFailure,
		FuncID_Send_NearbySearchSuccess,
		FuncID_Send_NearbySearchFailure,
		FuncID_Send_KeyWoardSearchSuccess,
		FuncID_Send_KeyWoardSearchFailure,		
		FuncID_Send_PassingPoiAndDestPoiSuccess,
		FuncID_Send_PassingPoiAndDestPoiFailure,		
		FuncID_Send_CollectionSuccess,
		FuncID_Send_CollectionFailure,
		FuncID_Send_StartNaviSuccess,
		FuncID_Send_StartNaviFailure,
		FuncID_Send_SaveCurrentToFavResp,
		FuncID_Send_SaveDestToFavResp,
		FuncID_Send_SavePoiToFavResp,		
		FuncID_Send_GetDestPointInfoResp,
		// METER FUNC ID
		FuncID_Send_MeterDirectionInfo,
		FuncID_Send_MeterNextTBTInfo,
		FuncID_Send_getDeviceConnect,
		FuncID_Send_EntryPhoneConnect, 
		FuncID_Send_ExitPhoneConnect,
		FuncID_Send_PhoneConnectStatus,
		FuncID_Send_SilentModeChangeSucess,
		FuncID_Send_SilentModeChangeFailure,
		FuncID_Send_UuidError,
		FuncID_Send_GetVersionSuccess,
		FuncID_Send_GetVersionFailure,
		FuncID_Send_Max
	};

	enum SERVICE_MANAGER_RECEIVE_FUNC_ID
	{	
		FuncID_Receive_reqStopVoiceEMG,
		FuncID_Receive_checkVoiceState,
		FuncID_Receive_AnswerVoiceStart,
		FuncID_Receive_showOtherFormSuc,
		FuncID_Receive_hideOtherFormSuc,
		FuncID_Receive_ReqShowNaviForm,
		FuncID_Receive_ReqHideNaviForm,
		FuncID_Receive_sgnILLStateChanged,
		FuncID_Receive_reqACCOFF,
		FuncID_Receive_AnswerSendAccOffSucess,
		//FuncID_Receive_AnswerSendStopSucess,
		FuncID_Receive_reqGetGPSTimeValue,

		FuncID_Receive_AnswerVolume,
		FuncID_Receive_AnswerMute,
		
		FuncID_Receive_getUUID,
		FuncID_Receive_HistoryClear,
		FuncID_Receive_RunStatusChanged,
		FuncID_Receive_AddrStatus,
		FuncID_Receive_AnswerSendShowNaviFormSuc,
		FuncID_Receive_GetUsbStatus,
		FuncID_Receive_NetworkStatusChanged,
		//FuncID_Receive_RevStatusChanged,
		FuncID_Receive_LanguageSwitchChanged,
		FuncID_Receive_GbookInfoChanged,
		// DeSai Receive Function ID
		FuncID_Receive_BackLightChanged,
		FuncID_Receive_getBackLight,
		FuncID_Receive_ResponseAudioGuidancePlay,
		FuncID_Receive_GetLanguageID,
		FuncID_Receive_IsGPSUpdateTimeEnabled,
		FuncID_Receive_IsCarPlayNaviActive,
		FuncID_Receive_MapCardChanged,
		FuncID_Receive_MusicWidgeShowChanged,	// MusicWidget show
		FuncID_Receive_MusicWidgeHideChanged,	// MusicWidget hide
		FuncID_Receive_SysHmiTimeChanged,		// SysHmiTime
		FuncID_Receive_SysDayNightLinkAge,		//day night linkage
		FuncID_Receive_TouchStatus,				
		// VR FUNC ID
		FuncID_Receive_VRUdeSearch,				// 关键字搜索	
		FuncID_Receive_VRSearchNearby,			// 周边搜索
		FuncID_Receive_VRPassingPoint,			// 途径点
		FuncID_Receive_VRCommandControl,		// 命令控制
		FuncID_Receive_VRSwitchRoute,			// 切换路线偏好
		FuncID_Receive_VRTrafficControl,		// 路况开关
		FuncID_Receive_VRInquiryDistance,		// 距离查询
		FuncID_Receive_VRItemSelect,			// 选单操作
		FuncID_Receive_VRPassPointAndDestPoint,	// 导航到指定途径点的目的地
		FuncID_Receive_VRCollection,			// 回家/回公司
		FuncID_Receive_VRStartNavi,				// 开始导航
		FuncID_Receive_DeviceConnectChanged,	//设备插拔通知
		FuncID_Receive_EntryPhoneConnectResponse,//连携消息的反馈
		FuncID_Receive_SilentModeChanged,		// 静默模式通知
		FuncID_Receive_ReqNaviVersion,			// 导航版本
		FuncID_Receive_AddFavPoint,             // 添加途径点
		FuncID_Receive_NaviIsHaveDest,
		FuncID_Receive_Max,
	};

	// send and receive data
	typedef struct SendData
	{
		SERVICE_MANAGER_SEND_FUNC_ID funcId;
		void * data;
	}SendData;

	typedef struct ReceiveData
	{
		SERVICE_MANAGER_RECEIVE_FUNC_ID funcId;
		void * data;
	}ReceiveData;

	//backlight
#if 1 //TODO desai macro
	enum BackLightMode
	{
		BLM_MANUAL = 0,
		BLM_AUTO	//1
	};
	enum BackLightType
	{
		BLT_DAY = 0,
		BLT_NIGHT	//1
	};

	typedef struct BL_DeSai
	{
		BackLightMode mode;
		BackLightType type;
		uint8 level;
	}BL_DeSai;

	typedef struct Guidance_DeSai
	{
		uint32 uiID;
		uint32 uiDistance;
	}Guidance_DeSai;

	typedef struct RoadName_DeSai
	{
		char* pUTF8RoadName;
		uint32 uiLen;
	}RoadName_DeSai;

	typedef struct MapbarVersion
	{
		cqWCHAR* naviVersion;
		cqWCHAR* mapVersion;
	}MapbarVersion;

	// POI信息
	typedef struct VRPoiInfo
	{
		int32 lon;
		int32 lat;
		cqCHAR name[128];
	}VRPoiInfo;

	// POI信息
	typedef struct VRViaAndDestPoiInfo
	{
		int32 viaLon;
		int32 viaLat;
		cqCHAR viaName[128];
		int32 destLon;
		int32 destLat;
		cqCHAR destName[128];
	}VRViaAndDestPoiInfo;

	// 途径点信息
	typedef struct VRPassPointInfo
	{
		uint32 type;
		int32 lon;
		int32 lat;
		cqCHAR* name;
	}VRPassPointInfo;

	// 删除、查询途径点结果
	typedef struct VRPassPointRepInfo
	{
		uint32 repType;
		int32 passPointNum;
	}VRPassPointRepInfo;

	enum VR_COMMAND_ID
	{
		//VR_COMMAND_SHOW_NAVI = 0,			//0
		//VR_COMMAND_HIDE_NAVI,				//1
		VR_COMMAND_CANCLE_NAVI = 0,			//0
		VR_COMMAND_CHANGE_MAP_MODE,			//1
		VR_COMMAND_CHANGE_MAP_TO,			//2
		VR_COMMAND_MAP_ZOOMIN,				//3
		VR_COMMAND_MAP_ZOOMOUT,				//4
		VR_COMMAND_INVALIDE,				//5
	};

	enum VR_COMMAND_MAP_MODE
	{
		VR_COMMAND_MAP_NORTH_UP = 0,		//北向上
		VR_COMMAND_MAP_HEAD_UP,				//车头向上
		VR_COMMAND_MAP_3D,					//3D
		VR_COMMAND_MAP_INVALIDE,			//无效值
	};

	typedef struct CommandControlSig
	{
		VR_COMMAND_ID uiCommand;
		VR_COMMAND_MAP_MODE uiMapMode;
	}CommandControlSig;


	enum VR_COMMAND_ROUTE_PREFER
	{
		VR_COMMAND_ROUTE_AVOID_JAMS = 0,		//避让拥堵
		VR_COMMAND_ROUTE_AVOID_HIGH_SPEED,		//避让高速
		VR_COMMAND_ROUTE_FEWER_FEES,			//少收费
		VR_COMMAND_ROUTE_FIRST_HIGH_SPEED,		//高速优先
		VR_COMMAND_ROUTE_SHORTEST,				//最短路线
		VR_COMMAND_ROUTE_AVOID_FERRY,			//避让轮渡
		VR_COMMAND_ROUTE_PREFER_INVALIDE,		//无效值
	};

	enum VR_COMMAND_TRAFFIC_SIG
	{
		VR_COMMAND_TRAFFIC_OFF = 0,				//开启实时路况
		VR_COMMAND_TRAFFIC_ON,					//关闭实时路况
		VR_COMMAND_TRAFFIC_INVALIDE,			//无效值
	};
	typedef struct  InquiryDistance
	{
		uint32 remainingTime;
		int32 remainingDistance;
	}InquiryDistance;

	enum VR_PASSING_POINT_REQTYPE
	{
		VR_PASSING_POINT_DELETE_ALL = 0,		//删除所有途径点
		VR_PASSING_POINT_INQUIRY_NUM,			//查询途径点个数
		VR_PASSING_POINT_INVALIDE,				//无效值
	};

	enum VR_COMMAND_CONTROL_FAILED_TYPE
	{
		VR_COMMAND_CONTROL_SAME_STATUS = 0,		//已是当前状态
		VR_COMMAND_CONTROL_LIMIT_VALUE,			//极限值
		VR_COMMAND_CONTROL_FAILED_OTHER,		//其他失败情况
	};

	enum VR_TRAFFIC_COMMAND_FAILED_TYPE
	{
		VR_TRAFFIC_COMMAND_HAS_OPENED = 0,		//路况已打开
		VR_TRAFFIC_COMMAND_HAS_CLOSED,			//路况已关闭
		VR_TRAFFIC_COMMAND_FAILED_OTHER,		//其他失败情况
	};
	
	enum VR_SEEARCH_STATUS
	{
		VR_SEARCH_NONE = 0,
		VR_SEARCH_UDE,
		VR_SEARCH_NEARBY,
		VR_SEARCH_NEARBY_WAIT_UDERESP,
		VR_SEARCH_ROUTE,
	};
	
	enum VR_ROUTE_PLAN
	{
		VR_ROUTE_PLAN_EXIST = 0,	//存在路线
		VR_ROUTE_PLAN_NONE,			//无路线
		VR_ROUTE_PLAN_INVALIDE,		//无效值
	};

	enum VR_ITEM_SELECT
	{
		VR_ITEM_SELECT_TOO_CLOSE = 0,
		VR_ITEM_SELECT_OTHER,
		VR_ITEM_SELECT_INVALIDE,
	};

	enum VR_COLLECTION_FAILED_TYPE
	{
		VR_COLLECTION_TOO_CLOSE = 0,
		VR_COLLECTION_OTHER,
		VR_COLLECTION_INVALIDE,
	};

	enum VR_PASSINGPOI_FAILED_TYPE
	{
		VR_PASSINGPOI_TOO_CLOSE = 0,
		VR_PASSINGPOI_OTHER,
		VR_PASSINGPOI_INVALIDE,
	};

	typedef struct VRSearchNearbyTypeMap
	{
		const cqWCHAR * nearbyType;
		const cqWCHAR * nearbyMapType;
	}VRSearchNearbyTypeMap;
	
	#define SHM_WRITE_MAX_LEN 8192
	#define VR_SEARCH_RESULT_MAX_NUMS	(20)
	#define EOR_KEY 0x55aa55aa
	#define SERCH_NESTED_ABORT_PRE
	#define DAYNIGHT_MODE_SWITCH_LINKAGE	(0)
	#define DAYNIGHT_MODE_SWITCH_UNLINKAGE	(1)
	
	#define LINKAGE_DAY_MODE	(0)
	#define LINKAGE_NIGHT_MODE	(1)
	#define LINKAGE_AUTO_MODE	(2)
	typedef struct VRShmData
	{
		int32_t lon;
		int32_t lat;
		int32_t dis;
		int32_t len;
		int32_t counter;
		char buff[SHM_WRITE_MAX_LEN];
	}VRShmData;
	#endif
}
	/*start add by qihc chang log out*/
	//#define USE_LOG_TO_FILE  (1)
	//#if (USE_LOG_TO_FILE) && defined (MAPBAR_TOYOTA_262AVN_LINUX)
	////#include "serviceManagerLog.h"
	////#define SVC_MGE_LOG_DEBUG(fmt, ...)		fs_drc_printf(SYS_ERR_LVL_LIGHT,8,fmt,##__VA_ARGS__)
	//#define SVC_MGE_LOG_INFO(fmt, ...)			fs_drc_printf(SYS_ERR_LVL_LIGHT,8,fmt,##__VA_ARGS__)
	//#define SVC_MGE_LOG_WARNING(fmt, ...)		fs_drc_printf(SYS_ERR_LVL_LIGHT,8,fmt,##__VA_ARGS__)
	//#define SVC_MGE_LOG_ERR(fmt, ...)			fs_drc_printf(SYS_ERR_LVL_LIGHT,8,fmt,##__VA_ARGS__)
	//#else
	#define SVC_MGE_LOG_INFO  CQ_LOG_INFO
	#define SVC_MGE_LOG_WARNING CQ_LOG_WARNING
	#define SVC_MGE_LOG_ERR CQ_LOG_ERROR
	//#endif	
	/*end add by qihc chang log out*/

#endif //__SERVICE_MANAGER_COMMON_PUBLIC_H__
