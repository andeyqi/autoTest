#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_FUNC_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_FUNC_H__
#include "stdafx_serviceManager.h"
#include "serviceManagerCommunicationIpcWrapperToyota.h"
//using namespace ServiceManager;

enum DBUS_WRAPPER_SIG{
	DBUS_WRAPPER_SIG_BASE = 0x20000000,						//0x20000000	
	DBUS_WRAPPER_SIG_GUIDEVOICE_START_ANS,					//0x20000001	
	DBUS_WRAPPER_SIG_GUIDEVOICE_STOP_ANS,					//0x20000002	
	DBUS_WRAPPER_SIG_GUIDEVOICE_STATE_CHK,					//0x20000003	
	DBUS_WRAPPER_SIG_GUIDEVOICE_STOP_EMG,					//0x20000004	
	DBUS_WRAPPER_SIG_SHOW,									//0x20000005	
	DBUS_WRAPPER_SIG_HIDE,									//0x20000006	
	DBUS_WRAPPER_SIG_SHOW_SUCCESS,							//0x20000007	
	DBUS_WRAPPER_SIG_HIDE_SUCCESS,							//0x20000008	
	DBUS_WRAPPER_SIG_ILL_ON,								//0x20000009	
	DBUS_WRAPPER_SIG_ILL_OFF,								//0x2000000A	
	DBUS_WRAPPER_SIG_ACC,									//0x2000000B	
	DBUS_WRAPPER_GPS_TIME_REQ,								//0x2000000C	
	DBUS_WRAPPER_SIG_VOLUM,									//0x2000000D	
	DBUS_WRAPPER_SIG_MUTE,									//0x2000000E	
	DBUS_WRAPPER_SIG_UUID_VALUE,							//0x2000000F	
	DBUS_WRAPPER_SIG_NAVIREQUEST,							//0x20000010	
	DBUS_WRAPPER_SIG_RUNSTS,								//0x20000011	
	DBUS_WRAPPER_SIG_WIFICONNECTED,							//0x20000012	
	DBUS_WRAPPER_SIG_WIFIDISCONNECTED,						//0x20000013	
	DBUS_WRAPPER_SIG_WIFICONNECTING,						//0x20000014	
	DBUS_WRAPPER_SIG_WEATHER_REQ,							//0x20000015	
	DBUS_WRAPPER_SIG_MEMORY_CLEAR,							//0x20000016	
	DBUS_WRAPPER_SIG_SYSTIMER_EDIT,							//0x20000017	
	DBUS_WRAPPER_SIG_MESSAGE_DR,							//0x20000018	
	DBUS_WRAPPER_SIG_USB_MOUNT,								//0x20000019
	DBUS_WRAPPER_SIG_USB_UNMOUNT,							//0x2000001A
	DBUS_WRAPPER_SIG_DATETIME,								//0x2000001B	
	DBUS_WRAPPER_SIG_DATETIME_CHANGE,		                //0x2000001C						
	DBUS_WRAPPER_SIG_MUSICWIDGETSHOW_SUCCESS,				//0x2000001D						
	DBUS_WRAPPER_SIG_MUSICWIDGETSHOW_FAILURE,			    //0x2000001E						
	DBUS_WRAPPER_SIG_NAVI_COMMAND,		                    //0x2000001F						
	DBUS_WRAPPER_SIG_UDE_SEARCH,	                        //0x20000020						
	DBUS_WRAPPER_SIG_SERRCH_NEARBY,							//0x20000021						
	DBUS_WRAPPER_SIG_PASSING_POINT,		                    //0x20000022						
	DBUS_WRAPPER_SIG_COMMAND_CONRTOL,						//0x20000023						
	DBUS_WRAPPER_SIG_SWITCH_ROUTE,		                    //0x20000024						
	DBUS_WRAPPER_SIG_TRAFFIC_CONTROL,						//0x20000025	
	DBUS_WRAPPER_SIG_INQUIRY_DISTANCE,						//0x20000026						
	DBUS_WRAPPER_SIG_COLLECTION,							//0x20000027						
	DBUS_WRAPPER_SIG_ITEM_SELECT,							//0x20000028						
	DBUS_WRAPPER_SIG_LANGUAGE_SWITCH,		                //0x20000029						
	DBUS_WRAPPER_SIG_DATETIME_DVALUE,	                    //0x2000002A	
	DBUS_WRAPPER_SIG_MUSICWIDGETHIDE_SUCCESS,				//0x2000002B	
	DBUS_WRAPPER_SIG_MUSICWIDGETHIDE_FAILURE,				//0x2000002C	
	DBUS_WRAPPER_SIG_CONNECT_TYPE = 0x20000030,				//0x20000030	
	DBUS_WRAPPER_SIG_REV_INFO,								//0x20000031	
	DBUS_WRAPPER_SIG_GBOOK_INFO,							//0x20000032
	DBUS_WRAPPER_SIG_SEARCH_DEST_VIA_PASSING_POI,			//0x20000033
	DBUS_WRAPPER_DAYNIGHT_MODE_SWITCH,						//0x20000034
	DBUS_WRAPPER_SIG_TOUCH_ENABLE,							//0x20000035
	DBUS_WRAPPER_SIG_TOUCH_DISABLE,							//0x20000036
	DBUS_WRAPPER_DEVICE_CONNECT_NTY,						//0x20000037
	DBUS_WRAPPER_DEVICE_DISCONNECT_NTY,						//0x20000038
	DBUS_WRAPPER_SIG_DEVICE_CONNECT_INFO_RES,				//0x20000039
	DBUS_WRAPPER_SIG_LANGUAGE_RES_SUCCESS,					//0x2000003A
	DBUS_WRAPPER_SIG_LANGUAGE_RES_FAILURE,					//0x2000003B
	DBUS_WRAPPER_SIG_ENTRY_PHONE_CONNECT_RES,				//0x2000003C
	DBUS_WRAPPER_SIG_SILENTMODE_REQ,						//0x2000003D
	DBUS_WRAPPER_SIG_NAVISTART_REQ,							//0x2000003E	
	DBUS_WRAPPER_SIG_NAVIVERSION_REQ,						//0x2000003F
	DBUS_WRAPPER_SIG_VR_SAVE_CUR_TO_FAV = 0x2000003F,	    //0x2000003F
	DBUS_WRAPPER_SIG_VR_SAVE_DEST_TO_FAV,					//0x20000040
	DBUS_WRAPPER_SIG_VR_INQUIRE_CUR_LOC,					//0x20000041
	DBUS_WRAPPER_SIG_VR_SHOW_MAP_NEAR_POI,					//0x20000042
	DBUS_WRAPPER_SIG_VR_SHOW_MAP_ENTIRE_ROUTE,				//0x20000043
	DBUS_WRAPPER_SIG_VR_SHOW_PREV_DEST,						//0x20000044
	DBUS_WRAPPER_SIG_VR_INQUIRE_MANEUVER_POINT,				//0x20000045
	DBUS_WRAPPER_SIG_VR_ADD_VIAPOINT_MAIN,					//0x20000046
	DBUS_WRAPPER_SIG_VR_REMOVE_VIAPOINT_MAIN,				//0x20000047
	DBUS_WRAPPER_SIG_VR_REPLACE_VIAPOINT_MAIN,				//0x20000048
	DBUS_WRAPPER_SIG_VR_GET_DVIAPOINT_LIST_MAIN,			//0x20000049
	DBUS_WRAPPER_SIG_VR_SAVE_POI_TO_FAV,					//0x20000050
	DBUS_WRAPPER_SIG_VR_GET_DEST_INFO_MAIN,					//0x20000051
	DBUS_WRAPPER_SIG_MAX
};

enum IDX_INFO
{
	IDX_HOME = 0,   ///< 表示“家”的下标
	IDX_OFFICE,		///< 表示“公司”的下标
	IDX_ADDR_1,		///< 表示“常用地址1”的下标
	IDX_ADDR_2,		///< 表示“常用地址2”的下标
	IDX_ADDR_3,		///< 表示“常用地址3”的下标
	IDX_ADDR_INVALID,
};

void Func_GuidancVoice_Start_Ans(cqCHAR* msg, void* ipcToyota);
void Func_GuidancVoice_Stop_Ans(cqCHAR* msg, void* ipcToyota);
void Func_GuidancVoice_State_Chk(cqCHAR* msg, void* ipcToyota);
void Func_GuidancVoice_Stop_Emg(cqCHAR* msg, void* ipcToyota);
void Func_Show(cqCHAR* msg, void* ipcToyota);
void Func_Hide(cqCHAR* msg, void* ipcToyota);
void Func_Show_Sucess(cqCHAR* msg, void* ipcToyota);
void Func_Hide_Sucess(cqCHAR* msg, void* ipcToyota);
void Func_Ill_On(cqCHAR* msg, void* ipcToyota);
void Func_Ill_Off(cqCHAR* msg, void* ipcToyota);
void Func_Acc(cqCHAR* msg, void* ipcToyota);
void Func_Gps_Time(cqCHAR* msg, void* ipcToyota);
void Func_Volume(cqCHAR* msg, void* ipcToyota);
void Func_Mute(cqCHAR* msg, void* ipcToyota);
void Func_Uuid_Value(cqCHAR* msg, void* ipcToyota);
void Func_Navi_Request(cqCHAR* msg, void* ipcToyota);
void Func_Runsts(cqCHAR* msg, void* ipcToyota);
void Func_Wifi_Connected(cqCHAR* msg, void* ipcToyota);
void Func_Wifi_DisConnected(cqCHAR* msg, void* ipcToyota);
void Func_Wifi_Connecting(cqCHAR* msg, void* ipcToyota);
void Func_Weather_Req(cqCHAR* msg, void* ipcToyota);
void Func_Memory_Clear(cqCHAR* msg, void* ipcToyota);
void Func_Systimer_Edit(cqCHAR* msg, void* ipcToyota);
void Func_Message_Dr(cqCHAR* msg, void* ipcToyota);
void Func_Usb_Mount(cqCHAR* msg, void* ipcToyota);
void Func_Usb_UnMount(cqCHAR* msg, void* ipcToyota);
void Func_Usb_DateTime(cqCHAR* msg, void* ipcToyota);
void Func_Usb_DateTime_Change(cqCHAR* msg, void* ipcToyota);
void Func_MusicWidget_Show_Sucess(cqCHAR* msg, void* ipcToyota);
void Func_MusicWidget_Show_Failure(cqCHAR* msg, void* ipcToyota);
void Func_Navi_Command(cqCHAR* msg, void* ipcToyota);
void Func_Ude_Search(cqCHAR* msg, void* ipcToyota);
void Func_Search_Nearby(cqCHAR* msg, void* ipcToyota);
void Func_Passing_Point(cqCHAR* msg, void* ipcToyota);
void Func_Command_Control(cqCHAR* msg, void* ipcToyota);
void Func_Switch_Route(cqCHAR* msg, void* ipcToyota);
void Func_Traffic_Control(cqCHAR* msg, void* ipcToyota);
void Func_Inquiry_Distance(cqCHAR* msg, void* ipcToyota);
void Func_Collection(cqCHAR* msg, void* ipcToyota);
void Func_Item_Select(cqCHAR* msg, void* ipcToyota);
void Func_Language_Switch(cqCHAR* msg, void* ipcToyota);
void Func_DateTime_Dvalue(cqCHAR* msg, void* ipcToyota);
void Func_MusicWidget_Hide_Sucess(cqCHAR* msg, void* ipcToyota);
void Func_MusicWidget_Hide_Failure(cqCHAR* msg, void* ipcToyota);
void Func_Connect_Type(cqCHAR* msg, void* ipcToyota);
void Func_Rev_Info(cqCHAR* msg, void* ipcToyota);
void Func_Gbook_Info(cqCHAR* msg, void* ipcToyota);
void Func_Search_Dest_Via_Passing_Poi(cqCHAR* msg, void* ipcToyota);
void Func_DayNight_Mode_Switch(cqCHAR* msg, void* ipcToyota);
void Func_Touch_Enable(cqCHAR* msg, void* ipcToyota);
void Func_Touch_Disable(cqCHAR* msg, void* ipcToyota);
void Func_Device_Connect_NTY(cqCHAR* msg, void* ipcToyota);
void Func_Device_Disconnect_NTY(cqCHAR* msg, void* ipcToyota);
void Func_Device_Connect_Info_Res(cqCHAR* msg, void* ipcToyota);
void Func_Language_Res_Sucess(cqCHAR* msg, void* ipcToyota);
void Func_Language_Res_Failure(cqCHAR* msg, void* ipcToyota);
void Func_Entry_Phone_Connect_Res(cqCHAR* msg, void* ipcToyota);
void Func_SilentMode_Req(cqCHAR* msg, void* ipcToyota);
void Func_NaviStart_Req(cqCHAR* msg, void* ipcToyota);
void Func_NaviVersion_Req(cqCHAR* msg, void* ipcToyota);
void Func_VRSaveCurToFav_Req(cqCHAR* message, void* ipcToyota);
void Func_VRSaveDstToFav_Req(cqCHAR* message, void* ipcToyota);
void Func_VRSavePOIToFav_Req(cqCHAR* message, void* ipcToyota);
void Func_VRNaviIsHaveDestPoint_Req(cqCHAR* message, void* ipcToyota);
void Func_Max(cqCHAR* msg);
#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_FUNC_H__ */