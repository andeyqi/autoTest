#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_SEND_FUNC_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_SEND_FUNC_H__
#include "stdafx_serviceManager.h"
#include "serviceManagerCommunicationIpcWrapperToyota.h"
#include "serviceManagerCommunicationIpcWrapperToyotaDbusServer.h"
#define MSG_MAX_SIZE 256
#define MSGID_MAX_SIZE 9

enum DBUS_NAVI_SIG{
	DBUS_NAVI_SIG_BASE = 0x10000000,						//0x10000000		
	DBUS_NAVI_SIG_START_SUCCESS,							//0x10000001		
	DBUS_NAVI_SIG_STOP_SUCCESS,								//0x10000002		
	DBUS_NAVI_SIG_GPS_SUCCESS,								//0x10000003		
	DBUS_NAVI_SIG_GUIDEVOICE_START_REQ,						//0x10000004		
	DBUS_NAVI_SIG_GUIDEVOICE_STOP_REQ,						//0x10000005		
	DBUS_NAVI_SIG_GUIDEVOICE_STATE_ON,						//0x10000006		
	DBUS_NAVI_SIG_GUIDEVOICE_STATE_OFF,						//0x10000007		
	DBUS_NAVI_SIG_BEEP,										//0x10000008		
	DBUS_NAVI_SIG_SHOW,										//0x10000009		
	DBUS_NAVI_SIG_HIDE,										//0x1000000A		
	DBUS_NAVI_SIG_SHOW_SUCCESS,								//0x1000000B		
	DBUS_NAVI_SIG_HIDE_SUCCESS,								//0x1000000C		
	DBUS_NAVI_SIG_ILL_REQ,									//0x1000000D		
	DBUS_NAVI_SIG_ILL_REP,									//0x1000000E		
	DBUS_NAVI_SIG_ACC_SUCCESS,								//0x1000000F		
	DBUS_NAVI_SIG_GPS_TIME_VALUE,							//0x10000010		
	DBUS_NAVI_SIG_GPS_TIME_FAILURE,							//0x10000011		
	DBUS_NAVI_SIG_LIVE,										//0x10000012		
	DBUS_NAVI_SIG_VERUP_START,								//0x10000013		
	DBUS_NAVI_SIG_VERUP_END,								//0x10000014		
	DBUS_NAVI_SIG_VOLUM,									//0x10000015		
	DBUS_NAVI_SIG_UUID_GET,									//0x10000016		
	DBUS_NAVI_SIG_CAUTION_SHOW,								//0x10000017		
	DBUS_NAVI_SIG_NAVISTATUS,								//0x10000018		
	DBUS_NAVI_SIG_ADDRSTATUS,								//0x10000019		
	DBUS_NAVI_SIG_ROUTINGSTATUS,							//0x1000001A		
	DBUS_NAVI_SIG_SELFPOSINFO,								//0x1000001B		
	DBUS_NAVI_SIG_TBTINFO,									//0x1000001C		
	DBUS_NAVI_SIG_SYSRESTART,								//0x1000001D		
	DBUS_NAVI_SIG_GUIDEVOICE_CNT_REQ,						//0x1000001E		
	DBUS_NAVI_SIG_WEATHER_NONE,								//0X1000001F		
	DBUS_NAVI_SIG_WEATHER_VALUE,							//0X10000020		
	DBUS_NAVI_SIG_RUNSTS_REQ,								//0X10000021		
	DBUS_NAVI_SIG_WIFISTS_REQ,								//0X10000022		
	DBUS_NAVI_SIG_MEMORY_CLEAR_SUCCESS,						//0x10000023		
	DBUS_NAVI_SIG_WIFISET_REQ,								//0x10000024
	DBUS_NAVI_SIG_NAVI_BACKGROUND,							//0x10000025		
	//DBUS_NAVI_SIG_REQUEST_DR,								//0x10000026		
	// 262AVN
	DBUS_NAVI_SIG_REQUEST_USB,								//0x10000026
	DBUS_NAVI_SIG_REQUEST_DATETIME,							//0x10000027
	DBUS_NAVI_SIG_MUSICWIDGET_REQ,							//0x10000028
	DBUS_NAVI_SIG_REQUEST_DATETIME_DVALUE,					//0x10000029
	DBUS_NAVI_SIG_NAVI_SUCCESS,								//0x1000002A
	DBUS_NAVI_SIG_NAVI_FAILURE,								//0x1000002B
	DBUS_NAVI_SIG_UDE_SEARCH_SUCCESS,						//0x1000002C
	DBUS_NAVI_SIG_UDE_SEARCH_FAILURE,						//0x1000002D
	DBUS_NAVI_SIG_SERRCH_NEARBY_SUCCESS,					//0x1000002E
	DBUS_NAVI_SIG_SERRCH_NEARBY_FAILURE,					//0x1000002F
	DBUS_NAVI_SIG_PASSING_POINT_SUCCESS,					//0x10000030
	DBUS_NAVI_SIG_PASSING_POINT_FAILURE,					//0x10000031
	DBUS_NAVI_SIG_COMMAND_CONRTOL_SUCCESS,					//0x10000032
	DBUS_NAVI_SIG_COMMAND_CONRTOL_FAILURE,					//0x10000033
	DBUS_NAVI_SIG_TRAFFIC_CONTROL_SUCCESS,					//0x10000034
	DBUS_NAVI_SIG_TRAFFIC_CONTROL_FAILURE,					//0x10000035
	DBUS_NAVI_SIG_RESPONSE_DISTANCE_SUCCESS,				//0x10000036
	DBUS_NAVI_SIG_RESPONSE_DISTANCE_FAILURE,				//0x10000037
	DBUS_NAVI_SIG_COLLECTION_SUCCESS,						//0x10000038
	DBUS_NAVI_SIG_COLLECTION_FAILURE,						//0x10000039
	DBUS_NAVI_SIG_ITEM_SELECT_SUCCESS,						//0x1000003A
	DBUS_NAVI_SIG_ITEM_SELECT_FAILURE,						//0x1000003B
	DBUS_NAVI_SIG_LANGUAGE_SWITCH_SUCCESS,					//0x1000003C	
	DBUS_NAVI_SIG_LANGUAGE_SWITCH_FAILURE,					//0x1000003D	
	DBUS_NAVI_SIG_SWITCH_ROUTE_SUCCESS,						//0x1000003E	
	DBUS_NAVI_SIG_SWITCH_ROUTE_FAILURE,						//0x1000003F	
	DBUS_NAVI_SIG_MUSICWIDGET_HIDE,							//0x10000040	

	DBUS_NAVI_SIG_HEAD_AUTOMOVE_DES_DIRECTION,				//0x10000041	
	DBUS_NAVI_SIG_NEXT_STREET_DIRECTION = 0x10000044,		//0x10000044	
	DBUS_NAVI_SIG_SEARCH_DEST_VIA_PASSING_POI_SUCCESS = 0x10000047,	//0x10000047	
	DBUS_NAVI_SIG_SEARCH_DEST_VIA_PASSING_POI_FAILURE,		//0x10000048	
	DBUS_NAVI_SIG_CONNECT_TYPE_REQUEST,						//0x10000049	
	DBUS_NAVI_SIG_REV_INFO_REQUEST,							//0x1000004A	
	DBUS_NAVI_SIG_GBOOK_INFO_REQUEST,						//0x1000004B
	DBUS_NAVI_SIG_DAYNIGHT_MODE_CHANGE,						//0x1000004C	
	DBUS_NAVI_SIG_READY_FOR_FORGROUD,						//0x1000004D	
	DBUS_NAVI_SIG_DAYNIGHT_MODE_REQUEST,					//0x1000004E
	DBUS_NAVI_SIG_ENTRY_PHONE_CONNECT,			            //0x1000004F
	DBUS_NAVI_SIG_EXIT_PHONE_CONNECT,			            //0x10000050
	DBUS_NAVI_SIG_PHONE_CONNECT_STATUS_NTY,			   	    //0x10000051
	DBUS_NAVI_SIG_DEVICE_CONNECT_INFO_REQ,			        //0x10000052
	DBUS_NAVI_SIG_LANGUAGE_REQ,								//0x10000053
	DBUS_NAVI_SIG_SILENTMODE_RES_SUCCESS,					//0x10000054
	DBUS_NAVI_SIG_SILENTMODE_RES_FAILURE,					//0x10000055
	DBUS_NAVI_SIG_UUID_ERROR,								//0x10000056	
	DBUS_NAVI_SIG_VERSION_SUCCESS,							//0x10000057	
	DBUS_NAVI_SIG_VERSION_FAILURE,							//0x10000058
	DBUS_NAVI_SIG_VR_SAVE_CUR_TO_FAV_RSP = 0x10000057,		//0x10000057
	DBUS_NAVI_SIG_VR_SAVE_DEST_TO_FAV_RSP,					//0x10000058
	DBUS_NAVI_SIG_VR_INQUIRE_CUR_LOC_RSP,					//0x10000059
	DBUS_NAVI_SIG_VR_SHOW_MAP_NEAR_POI_RSP,					//0x1000005a
	DBUS_NAVI_SIG_VR_SHOW_MAP_ENTIRE_ROUTE_RSP,				//0x1000005b
	DBUS_NAVI_SIG_VR_SHOW_PREV_DEST_RSP,					//0x1000005c
	DBUS_NAVI_SIG_VR_INQUIRE_MANEUVER_POINT_RSP,			//0x1000005d
	DBUS_NAVI_SIG_VR_ADD_VIAPOINT_MAIN_RSP,					//0x1000005e
	DBUS_NAVI_SIG_VR_REMOVE_VIAPOINT_MAIN_RSP,				//0x1000005f
	DBUS_NAVI_SIG_VR_REPLACE_VIAPOINT_MAIN_RSP,				//0x10000060
	DBUS_NAVI_SIG_VR_SAVE_POI_TO_FAV_RSP,					//0x10000062
	DBUS_NAVI_SIG_VR_VR_GET_DEST_INFO_MAIN_RSP,				//0x10000063
};
typedef enum TNaviTurnIconID {
	ENTI_None = 0,
	ENTI_Arrival = 1,
	ENTI_TurnAround = 2,
	ENTI_EnterMainRoute = 3,
	ENTI_EnterRotary = 4,
	ENTI_GoStraight = 5,
	ENTI_LeaveMainRoute = 6,
	ENTI_LeaveRotary = 7,
	ENTI_TurnLeft = 8,
	ENTI_TurnRight = 9,
	ENTI_TurnSlightlyLeft = 10,	// 10
	ENTI_TurnSlightlyRight = 11,
	ENTI_Rotary1 = 12,
	ENTI_Rotary2 = 13,
	ENTI_Rotary3 = 14,
	ENTI_Rotary4 = 15,
	ENTI_Rotary5 = 16,
	ENTI_Rotary6 = 17,
	ENTI_Rotary7 = 18,
	ENTI_Rotary8 = 19,
	ENTI_Rotary9 = 20,	// 20
	ENTI_KeepLeft = 21,
	ENTI_KeepRight = 22,
	ENTI_TurnHardLeft = 23,
	ENTI_TurnHardRight = 24,
	ENTI_TurnLeftKeepLeft = 25,
	ENTI_TurnLeftKeepRight = 26,
	ENTI_TurnRightKeepLeft = 27,
	ENTI_TurnRightKeepRight = 28,
	ENTI_EnterTunnel = 29,
	ENTI_TakeFerry = 30,
	ENTI_Start = 31,
	ENTI_WayPoints1 = 32,
	ENTI_WayPoints2 = 33,
	ENTI_WayPoints3 = 34,
	ENTI_IC = 35,
	ENTI_DR = 36,
	ENTI_Overpass = 37,
	ENTI_WindOverpass = 38,
	ENTI_StraightKeepLeft = 39,
	ENTI_StraightKeepRight = 40,

	ENTI_Stair = 43,
	ENTI_Underpass = 44,
	ENTI_Footbridge = 45,
	ENTI_Square = 46,
	ENTI_Bridge = 47,
	ENTI_Park = 48,

	ENTI_3WayForkLeft = 49,
	ENTI_3WayForkMiddle = 50,
	ENTI_3WayForkRight = 51,

	ENTI_WayPoints4 = 52,

	ENTI_WayPoints5 = 132,

	//[丰田仪表盘转向标显示对应][2017/07/21][wangbin][add][start]
	ENTI_RightHandTrafficDirection_Rotary1 = 1012,
	ENTI_RightHandTrafficDirection_Rotary2 = 1013,
	ENTI_RightHandTrafficDirection_Rotary3 = 1014,
	ENTI_RightHandTrafficDirection_Rotary4 = 1015,
	ENTI_RightHandTrafficDirection_Rotary5 = 1016,
	ENTI_RightHandTrafficDirection_Rotary6 = 1017,
	ENTI_RightHandTrafficDirection_Rotary7 = 1018,
	ENTI_RightHandTrafficDirection_Rotary8 = 1019,
	ENTI_RightHandTrafficDirection_Rotary9 = 1020,

	ENTI_EnterTollStation = 1021,
	//[丰田仪表盘转向标显示对应][2017/07/21][wangbin][add][end]

	ENTI_Max
} TNaviTurnIconID;

typedef enum RotaryTurnDirection
{
	RotaryTurnDirection_none = 0,
	RotaryTurnDirection_1 = 53,
	RotaryTurnDirection_2 = 54,
	RotaryTurnDirection_3 = 55,
	RotaryTurnDirection_4 = 56,
	RotaryTurnDirection_5 = 57,
	RotaryTurnDirection_6 = 58,
	RotaryTurnDirection_7 = 59,
	RotaryTurnDirection_8 = 60,
	RotaryTurnDirection_9 = 61,
	RotaryTurnDirection_10 = 62,
	RotaryTurnDirection_11 = 63,
	RotaryTurnDirection_12 = 64,
	RotaryTurnDirection_13 = 65,
	RotaryTurnDirection_14 = 66,
	RotaryTurnDirection_15 = 67,
	RotaryTurnDirection_RightHand_1 = 68,
	RotaryTurnDirection_RightHand_2 = 69,
	RotaryTurnDirection_RightHand_3 = 70,
	RotaryTurnDirection_RightHand_4 = 71,
	RotaryTurnDirection_RightHand_5 = 72,
	RotaryTurnDirection_RightHand_6 = 73,
	RotaryTurnDirection_RightHand_7 = 74,
	RotaryTurnDirection_RightHand_8 = 75,
	RotaryTurnDirection_RightHand_9 = 76,
	RotaryTurnDirection_RightHand_10 = 77,
	RotaryTurnDirection_RightHand_11 = 78,
	RotaryTurnDirection_RightHand_12 = 79,
	RotaryTurnDirection_RightHand_13 = 80,
	RotaryTurnDirection_RightHand_14 = 81,
	RotaryTurnDirection_RightHand_15 = 82,
	RotaryTurnDirection_RightHand_enterRotary = 83,
	RotaryTurnDirection_RightHand_exitRotary = 84,
	RotaryTurnDirection_LeftHand_1 = 1022,
	RotaryTurnDirection_LeftHand_2 = 1023,
	RotaryTurnDirection_LeftHand_3 = 1024,
	RotaryTurnDirection_LeftHand_4 = 1025,
	RotaryTurnDirection_LeftHand_5 = 1026,
	RotaryTurnDirection_LeftHand_6 = 1027,
	RotaryTurnDirection_LeftHand_7 = 1028,
	RotaryTurnDirection_LeftHand_8 = 1029,
	RotaryTurnDirection_LeftHand_9 = 1030,
	RotaryTurnDirection_LeftHand_10 = 1031,
	RotaryTurnDirection_LeftHand_11 = 1032,
	RotaryTurnDirection_LeftHand_12 = 1033,
	RotaryTurnDirection_LeftHand_13 = 1034,
	RotaryTurnDirection_LeftHand_14 = 1035,
	RotaryTurnDirection_LeftHand_15 = 1036,
	RotaryTurnDirection_LeftHand_enterRotary = 1037,
	RotaryTurnDirection_LeftHand_exitRotary = 1038,
	RotaryTurnDirection_placeHolder = 0xfffffff
}RotaryTurnDirection;

typedef uint64 DSegmentId;

typedef struct TurnIconsOnRotary
{
	size_t turnIconsCnt;
	RotaryTurnDirection turnIconsSection[16];
	DSegmentId outRotaryIDs[16];
} TurnIconsOnRotary;

typedef struct TBTInfo
{
	BOOL isOffLineTurnIconsOnRotary;		//是否是环岛离线转向标
	BOOL isOffLineTurnIcons; 				//是否是离线转向标
	TNaviTurnIconID turnIcon; 				//当前需要显示的转向标图标ID
	TurnIconsOnRotary turnIconsOnRotary;  	//环岛转向标图标信息	
	int32 turnIconRemainDistance;   		//显示在转向标上的距离值，单位：米。
	int32 routeLength;		  				//路线的总长度，单位：米。如果没有路线，则为：0
	int32 travelledDistance;  				//自车在路线上已经行驶过的距离，单位：米。如果没有路线，则为：0
	uint32 remainingTime;
} TBTInfo;

// 地图朝向、自车行进方向、目的地方向
typedef struct DirectionInfo
{
	uint32 mapDirection;					//0未确定，1北向上，2车头向上，3未确定     
	uint32 carDirection; 					//自车行进方向
	int32 destDirection; 					//目的地方向
} DirectionInfo;

// 路口路线信息
typedef struct NextTBTInfo
{
	cqWCHAR nextRoadName[64];					//下一条路名称
	uint32 distance; 						//距路口距离
	//cqWCHAR unit; 							//距离单位 m
	uint32 routeBackInfo;  					//前方道路32方位信息，参数1是32bit的int型整数
	uint32 isOnRotary;   					//是否有环岛 0:无,1：有
	uint32 isOutSide;		  				//环岛内外向 默认外向 0
	uint32 actualOutLine;  					//实际出边
	uint32 specialIcon;						//特殊转向标
	uint32 remainingDistance;				//距终点距离 单位m
	uint32 remainingTime;					//距终点剩余时间 单位s
} NextTBTInfo;

int16 ChangeturnIcon(TNaviTurnIconID turnIcon);

void Func_Navi_Start_Suc(cqWCHAR* msg,void* data);
void Func_Navi_Stop_Suc(cqWCHAR* msg, void* data);
void Func_GPS_Rcv_Suc(cqWCHAR* msg, void* data);
void Func_Current_Voice_State(cqWCHAR* msg, void* data);
void Func_Req_Voice_Start(cqWCHAR* msg, void* data);
void Func_Req_Voice_Stop(cqWCHAR* msg, void* data);
void Func_Req_Play_Beep_Sound(cqWCHAR* msg, void* data);
void Func_Req_Show_Other_Form(cqWCHAR* msg, void* data);
void Func_Req_Hide_Other_Form(cqWCHAR* msg, void* data);
void Func_Show_Navi_Form_Suc(cqWCHAR* msg, void* data);
void Func_Hide_Navi_Form_Suc(cqWCHAR* msg, void* data);
void Func_Caution_Show(cqWCHAR* msg, void* data);
void Func_Req_Check_ILL_State(cqWCHAR* msg, void* data);
void Func_Rep_ILL_State_Changed(cqWCHAR* msg, void* data);
void Func_Navi_ACCOFF_Suc(cqWCHAR* msg, void* data);
void Func_Current_GPS_TimeValue(cqWCHAR* msg, void* data);
void Func_Current_GPS_TimeValue_Fail(cqWCHAR* msg, void* data);
void Func_Update_TBTInfo(cqWCHAR* msg, void* data);
void Func_Update_RoadName(cqWCHAR* msg, void* data);
void Func_Start_Guidance(cqWCHAR* msg, void* data);
void Func_Stop_Guidance(cqWCHAR* msg, void* data);
void Func_Update_Next_RoadName(cqWCHAR* msg, void* data);
void Func_Addr_Status(cqWCHAR* msg, void* data);
void Func_Get_UUID(cqWCHAR* msg, void* data);
void Func_Set_Guidance(cqWCHAR* msg, void* data);
void Func_BgSwt(cqWCHAR* msg, void* data);
void Func_Notify_NaviLive_Status(cqWCHAR* msg, void* data);
void Func_History_Clear(cqWCHAR* msg, void* data);
void Func_RunStatus_Request(cqWCHAR* msg, void* data);
void Func_Req_Usb_Status(cqWCHAR* msg, void* data);
void Func_Language_Switch_Success(cqWCHAR* msg, void* data);
void Func_Language_Switch_Failure(cqWCHAR* msg, void* data);
void Func_Get_Network_Status(cqWCHAR* msg, void* data);
void Func_Get_Gbook_Info(cqWCHAR* msg, void* data);
void Func_Set_Volume(cqWCHAR* msg, void* data);
void Func_Get_Volume(cqWCHAR* msg, void* data);
void Func_Get_Back_Light(cqWCHAR* msg, void* data);
void Func_Back_ToHost(cqWCHAR* msg, void* data);
void Func_Show_Host_Menu(cqWCHAR* msg, void* data);
void Func_Request_Audio_Guidance_Play(cqWCHAR* msg, void* data);
void Func_Release_Audio_Guidance_Play(cqWCHAR* msg, void* data);
void Func_Get_LanguageID(cqWCHAR* msg, void* data);
void Func_Is_GPS_Update_Time_Enabled(cqWCHAR* msg, void* data);
void Func_Set_Route_Status(cqWCHAR* msg, void* data);
void Func_Is_CarPlay_NaviActive(cqWCHAR* msg, void* data);
void Func_Routing_Status_Routing(cqWCHAR* msg, void* data);
void Func_Routing_Status_Failed(cqWCHAR* msg, void* data);
void Func_Routing_Status_Complete(cqWCHAR* msg, void* data);
void Func_Req_Show_Music_Widget(cqWCHAR* msg, void* data);
void Func_Req_Hide_Music_Widget(cqWCHAR* msg, void* data);
void Func_Req_Sys_Hmi_Time(cqWCHAR* msg, void* data);
void Func_Ready_For_Forgroud(cqWCHAR* msg, void* data);
void Func_Get_DayNight_Mode(cqWCHAR* msg, void* data);
void Func_Set_DayNight_Mode(cqWCHAR* msg, void* data);
void Func_Command_Control_Sucess(cqWCHAR* msg, void* data);
void Func_Command_Control_Failure(cqWCHAR* msg, void* data);
void Func_Command_Traffic_Sucess(cqWCHAR* msg, void* data);
void Func_Command_Traffic_Failure(cqWCHAR* msg, void* data);
void Func_Passing_Point_Sucess(cqWCHAR* msg, void* data);
void Func_Passing_Point_Failure(cqWCHAR* msg, void* data);
void Func_Switch_Route_Sucess(cqWCHAR* msg, void* data);
void Func_Switch_Route_Failure(cqWCHAR* msg, void* data);
void Func_Item_Select_Sucess(cqWCHAR* msg, void* data);
void Func_Item_Select_Failure(cqWCHAR* msg, void* data);
void Func_Inquiry_Distance_Success(cqWCHAR* msg, void* data);
void Func_Inquiry_Distance_Failure(cqWCHAR* msg, void* data);
void Func_Nearby_Search_Success(cqWCHAR* msg, void* data);
void Func_Nearby_Search_Failure(cqWCHAR* msg, void* data);
void Func_KeyWoard_Search_Success(cqWCHAR* msg, void* data);
void Func_KeyWoard_Search_Failure(cqWCHAR* msg, void* data);
void Func_Passing_PoiAndDestPoi_Success(cqWCHAR* msg, void* data);
void Func_Passing_PoiAndDestPoi_Failure(cqWCHAR* msg, void* data);
void Func_Collection_Success(cqWCHAR* msg, void* data);
void Func_Collection_Failure(cqWCHAR* msg, void* data);
void Func_Start_Navi_Success(cqWCHAR* msg, void* data);
void Func_Start_Navi_Failure(cqWCHAR* msg, void* data);
// METER FUNC ID
void Func_Meter_Direction_Info(cqWCHAR* msg, void* data);
void Func_Meter_Next_TBTInfo(cqWCHAR* msg, void* data);
void Func_Get_Device_Connect(cqWCHAR* msg, void* data);
void Func_Entry_Phone_Connect(cqWCHAR* msg, void* data);
void Func_Exit_Phone_Connect(cqWCHAR* msg, void* data);
void Func_Phone_Connect_Status(cqWCHAR* msg, void* data);
void Func_SilentMode_Change_Sucess(cqWCHAR* msg, void* data);
void Func_SilentMode_Change_Failure(cqWCHAR* msg, void* data);
void Func_UuidError(cqWCHAR* msg, void* data);
void Func_Get_Version_Success(cqWCHAR* msg, void* data);
void Func_Get_Version_Failure(cqWCHAR* msg, void* data);
void Func_Add_Current_To_Fav_Resp(cqWCHAR* , void* );
void Func_Add_Dest_To_Fav_Resp(cqWCHAR* , void* );
void Func_Add_Poi_To_Fav_Resp(cqWCHAR* , void* );
void Func_Is_Navi_Have_Des_Resp(cqWCHAR* , void* );
#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_SEND_FUNC_H__ */