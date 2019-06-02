#include "serviceManagerCommunicationIpcWrapperToyota.h"
#include <stdio.h>
#include "serviceManagerCommunicationIpcWrapperRevFunc.h"
#include "serviceManagerCommunicationIpcWrapperSendFunc.h"
#include "communication/serviceManagerOutput.h"
#include "CThreadPool.h"

using namespace ServiceManager;

typedef void(*RevFuncType)(cqCHAR* msg, void* data);
typedef void(*SendFuncType)(cqWCHAR* msg, void* data);

NaviSysIPCWrapperToyota* NaviSysIPCWrapperToyota::m_IpcWrapperInstance = NULL;

class IPCWrapperJob : public CJob
{
public:
	IPCWrapperJob(void *data) : CJob(data){}
	
	virtual void run()
	{
		ReceiveData* data = (ReceiveData*)getJobdata();
		if (NULL != NaviSysIPCWrapperToyota::getInstance())
		{
			NaviSysIPCWrapperToyota::getInstance()->mpListener->messageListener(data);
		}		
	}
};

/**
*	@brief
*		NaviSysIPCWrapperToyota构造
*/
NaviSysIPCWrapperToyota::NaviSysIPCWrapperToyota()
{
	mpListener = NULL;
	m_isAccOff = FALSE;
	m_isNaviReady = FALSE;

	m_IpcWrapperInstance = this;
	
	m_Pool = new CThreadPool(5);
	if (m_Pool == NULL)
	{
		CQ_LOG_ERROR("[ServiceManager] m_Pool is NULL.\n");
		Mapbar_drclog_printf("sm-thread pool is NULL \n");
		CQ_ASSERT(m_Pool);
	}
	m_Pool->addListener(new IThreadPoolListener());
	
	m_RevHashMap = hashmap_new(DBUS_WRAPPER_SIG_MAX - DBUS_WRAPPER_SIG_BASE);
	m_SendHashMap = hashmap_new(FuncID_Send_Max);
	initHashMap();
	
	ToyotaDbusService* toyotaDbusService = ToyotaDbusService::getInstance();
	toyotaDbusService->initDbusConnect();
	toyotaDbusService->addListener(this);
}

/**
*	@brief
*		NaviSysIPCWrapperToyota析构
*/
NaviSysIPCWrapperToyota::~NaviSysIPCWrapperToyota()
{
	CQ_LOG_INFO("[ServiceManager] ~NaviSysIPCWrapperToyota.\n");
	ToyotaDbusService::getInstance()->removeListener(this);
	mpListener = NULL;	
	ToyotaDbusService::releaseInstance();
}

void NaviSysIPCWrapperToyota::initHashMap()
{
	CQ_LOG_INFO("[ServiceManager] init RevHashMap \n");
	hashmap_put(m_RevHashMap, DBUS_NAVI_SIG_BASE, NULL);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_GUIDEVOICE_START_ANS, (any_t)Func_GuidancVoice_Start_Ans);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_GUIDEVOICE_STOP_ANS, (any_t)Func_GuidancVoice_Stop_Ans);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_GUIDEVOICE_STATE_CHK, (any_t)Func_GuidancVoice_State_Chk);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_GUIDEVOICE_STOP_EMG, (any_t)Func_GuidancVoice_Stop_Emg);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SHOW, (any_t)Func_Show);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_HIDE, (any_t)Func_Hide);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SHOW_SUCCESS, (any_t)Func_Show_Sucess);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_HIDE_SUCCESS, (any_t)Func_Hide_Sucess);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_ILL_ON, (any_t)Func_Ill_On);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_ILL_OFF, (any_t)Func_Hide_Sucess);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_ACC, (any_t)Func_Acc);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_GPS_TIME_REQ, (any_t)Func_Gps_Time);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_VOLUM, (any_t)Func_Volume);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MUTE, (any_t)Func_Mute);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_UUID_VALUE, (any_t)Func_Uuid_Value);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_NAVIREQUEST, (any_t)Func_Navi_Request);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_RUNSTS, (any_t)Func_Runsts);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_WIFICONNECTED, (any_t)Func_Wifi_Connected);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_WIFIDISCONNECTED, (any_t)Func_Wifi_DisConnected);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_WIFICONNECTING, (any_t)Func_Wifi_Connecting);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_WEATHER_REQ, (any_t)Func_Weather_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MEMORY_CLEAR, (any_t)Func_Memory_Clear);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SYSTIMER_EDIT, (any_t)Func_Systimer_Edit);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MESSAGE_DR, (any_t)Func_Message_Dr);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_USB_MOUNT, (any_t)Func_Usb_Mount);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_USB_UNMOUNT, (any_t)Func_Usb_UnMount);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_DATETIME, (any_t)Func_Usb_DateTime);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_DATETIME_CHANGE, (any_t)Func_Usb_DateTime_Change);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MUSICWIDGETSHOW_SUCCESS, (any_t)Func_MusicWidget_Show_Sucess);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MUSICWIDGETSHOW_FAILURE, (any_t)Func_MusicWidget_Show_Failure);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_NAVI_COMMAND, (any_t)Func_Navi_Command);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_UDE_SEARCH, (any_t)Func_Ude_Search);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SERRCH_NEARBY, (any_t)Func_Search_Nearby);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_PASSING_POINT, (any_t)Func_Passing_Point);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_COMMAND_CONRTOL, (any_t)Func_Command_Control);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SWITCH_ROUTE, (any_t)Func_Switch_Route);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_TRAFFIC_CONTROL, (any_t)Func_Traffic_Control);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_INQUIRY_DISTANCE, (any_t)Func_Inquiry_Distance);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_COLLECTION, (any_t)Func_Collection);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_ITEM_SELECT, (any_t)Func_Item_Select);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_LANGUAGE_SWITCH, (any_t)Func_Language_Switch);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_DATETIME_DVALUE, (any_t)Func_DateTime_Dvalue);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MUSICWIDGETHIDE_SUCCESS, (any_t)Func_MusicWidget_Hide_Sucess);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MUSICWIDGETHIDE_FAILURE, (any_t)Func_MusicWidget_Hide_Failure);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_CONNECT_TYPE, (any_t)Func_Connect_Type);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_REV_INFO, (any_t)Func_Rev_Info);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_GBOOK_INFO, (any_t)Func_Gbook_Info);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SEARCH_DEST_VIA_PASSING_POI, (any_t)Func_Search_Dest_Via_Passing_Poi);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_DAYNIGHT_MODE_SWITCH, (any_t)Func_DayNight_Mode_Switch);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_TOUCH_ENABLE, (any_t)Func_Touch_Enable);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_TOUCH_DISABLE, (any_t)Func_Touch_Disable);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_DEVICE_CONNECT_NTY, (any_t)Func_Device_Connect_NTY);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_DEVICE_DISCONNECT_NTY, (any_t)Func_Device_Disconnect_NTY);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_DEVICE_CONNECT_INFO_RES, (any_t)Func_Device_Connect_Info_Res);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_LANGUAGE_RES_SUCCESS, (any_t)Func_Language_Res_Sucess);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_LANGUAGE_RES_FAILURE, (any_t)Func_Language_Res_Failure);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_ENTRY_PHONE_CONNECT_RES, (any_t)Func_Entry_Phone_Connect_Res);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_SILENTMODE_REQ, (any_t)Func_SilentMode_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_NAVISTART_REQ, (any_t)Func_NaviStart_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_NAVIVERSION_REQ, (any_t)Func_NaviVersion_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_VR_SAVE_CUR_TO_FAV, (any_t)Func_VRSaveCurToFav_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_VR_SAVE_DEST_TO_FAV, (any_t)Func_VRSaveDstToFav_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_VR_SAVE_POI_TO_FAV, (any_t)Func_VRSavePOIToFav_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_VR_GET_DEST_INFO_MAIN, (any_t)Func_VRSavePOIToFav_Req);
	hashmap_put(m_RevHashMap, DBUS_WRAPPER_SIG_MAX, NULL);
	//CQ_LOG_ERROR("[ServiceManager] init SendHashMap \n");
	hashmap_put(m_SendHashMap, FuncID_Send_NaviStartSuc, (any_t)Func_Navi_Start_Suc);
	hashmap_put(m_SendHashMap, FuncID_Send_NaviStopSuc, (any_t)Func_Navi_Stop_Suc);
	hashmap_put(m_SendHashMap, FuncID_Send_GPSRcvSuc, (any_t)Func_GPS_Rcv_Suc);
	hashmap_put(m_SendHashMap, FuncID_Send_CurrentVoiceState, (any_t)Func_Current_Voice_State);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqVoiceStart, (any_t)Func_Req_Voice_Start);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqVoiceStop, (any_t)Func_Req_Voice_Stop);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqPlayBeepSound, (any_t)Func_Req_Play_Beep_Sound);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqShowOtherForm, (any_t)Func_Req_Show_Other_Form);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqHideOtherForm, (any_t)Func_Req_Hide_Other_Form);
	hashmap_put(m_SendHashMap, FuncID_Send_ShowNaviFormSuc, (any_t)Func_Show_Navi_Form_Suc);
	hashmap_put(m_SendHashMap, FuncID_Send_HideNaviFormSuc, (any_t)Func_Hide_Navi_Form_Suc);
	hashmap_put(m_SendHashMap, FuncID_Send_CautionShow, (any_t)Func_Caution_Show);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqCheckILLState, (any_t)Func_Req_Check_ILL_State);
	hashmap_put(m_SendHashMap, FuncID_Send_RepILLStateChanged, (any_t)Func_Rep_ILL_State_Changed);
	hashmap_put(m_SendHashMap, FuncID_Send_NaviACCOFFSuc, (any_t)Func_Navi_ACCOFF_Suc);
	hashmap_put(m_SendHashMap, FuncID_Send_CurrentGPSTimeValue, (any_t)Func_Current_GPS_TimeValue);
	hashmap_put(m_SendHashMap, FuncID_Send_CurrentGPSTimeValueFail, (any_t)Func_Current_GPS_TimeValue_Fail);
	hashmap_put(m_SendHashMap, FuncID_Send_UpdateTBTInfo, (any_t)Func_Update_TBTInfo);
	hashmap_put(m_SendHashMap, FuncID_Send_UpdateRoadName, (any_t)Func_Update_RoadName);
	hashmap_put(m_SendHashMap, FuncID_Send_StartGuidance, (any_t)Func_Start_Guidance);
	hashmap_put(m_SendHashMap, FuncID_Send_StopGuidance, (any_t)Func_Stop_Guidance);
	hashmap_put(m_SendHashMap, FuncID_Send_UpdateNextRoadName, (any_t)Func_Update_Next_RoadName);
	hashmap_put(m_SendHashMap, FuncID_Send_AddrStatus, (any_t)Func_Addr_Status);
	hashmap_put(m_SendHashMap, FuncID_Send_getUUID, (any_t)Func_Get_UUID);
	hashmap_put(m_SendHashMap, FuncID_Send_SetGuidance, (any_t)Func_Set_Guidance);
	hashmap_put(m_SendHashMap, FuncID_Send_BgSwt, (any_t)Func_BgSwt);
	hashmap_put(m_SendHashMap, FuncID_Send_NotifyNaviLiveStatus, (any_t)Func_Notify_NaviLive_Status);
	hashmap_put(m_SendHashMap, FuncID_Send_HistoryClear, (any_t)Func_History_Clear);
	hashmap_put(m_SendHashMap, FuncID_Send_RunStatusRequest, (any_t)Func_RunStatus_Request);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqUsbStatus, (any_t)Func_Req_Usb_Status);
	hashmap_put(m_SendHashMap, FuncID_Send_LanguageSwitchSuccess, (any_t)Func_Language_Switch_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_LanguageSwitchFailure, (any_t)Func_Language_Switch_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_getNetworkStatus, (any_t)Func_Get_Network_Status);
	hashmap_put(m_SendHashMap, FuncID_Send_getGbookInfo, (any_t)Func_Get_Gbook_Info);
	hashmap_put(m_SendHashMap, FuncID_Send_SetVolume, (any_t)Func_Set_Volume);
	hashmap_put(m_SendHashMap, FuncID_Send_GetVolume, (any_t)Func_Get_Volume);
	hashmap_put(m_SendHashMap, FuncID_Send_GetBackLight, (any_t)Func_Get_Back_Light);
	hashmap_put(m_SendHashMap, FuncID_Send_BackToHost, (any_t)Func_Back_ToHost);
	hashmap_put(m_SendHashMap, FuncID_Send_ShowHostMenu, (any_t)Func_Show_Host_Menu);
	hashmap_put(m_SendHashMap, FuncID_Send_RequestAudioGuidancePlay, (any_t)Func_Request_Audio_Guidance_Play);
	hashmap_put(m_SendHashMap, FuncID_Send_ReleaseAudioGuidancePlay, (any_t)Func_Release_Audio_Guidance_Play);
	hashmap_put(m_SendHashMap, FuncID_Send_GetLanguageID, (any_t)Func_Get_LanguageID);
	hashmap_put(m_SendHashMap, FuncID_Send_IsGPSUpdateTimeEnabled, (any_t)Func_Is_GPS_Update_Time_Enabled);
	hashmap_put(m_SendHashMap, FuncID_Send_SetRouteStatus, (any_t)Func_Set_Route_Status);
	hashmap_put(m_SendHashMap, FuncID_Send_IsCarPlayNaviActive, (any_t)Func_Is_CarPlay_NaviActive);
	hashmap_put(m_SendHashMap, FuncID_Send_RoutingStatusRouting, (any_t)Func_Routing_Status_Routing);
	hashmap_put(m_SendHashMap, FuncID_Send_RoutingStatusFailed, (any_t)Func_Routing_Status_Failed);
	hashmap_put(m_SendHashMap, FuncID_Send_RoutingStatusComplete, (any_t)Func_Routing_Status_Complete);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqShowMusicWidget, (any_t)Func_Req_Show_Music_Widget);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqHideMusicWidget, (any_t)Func_Req_Hide_Music_Widget);
	hashmap_put(m_SendHashMap, FuncID_Send_ReqSysHmiTime, (any_t)Func_Req_Sys_Hmi_Time);
	hashmap_put(m_SendHashMap, FuncID_Send_ReadyForForgroud, (any_t)Func_Ready_For_Forgroud);
	hashmap_put(m_SendHashMap, FuncID_Send_GetDayNightMode, (any_t)Func_Get_DayNight_Mode);
	hashmap_put(m_SendHashMap, FuncID_Send_SetDayNightMode, (any_t)Func_Set_DayNight_Mode);
	hashmap_put(m_SendHashMap, FuncID_Send_CommandControlSucess, (any_t)Func_Command_Control_Sucess);
	hashmap_put(m_SendHashMap, FuncID_Send_CommandControlFailure, (any_t)Func_Command_Control_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_CommandTrafficSucess, (any_t)Func_Command_Traffic_Sucess);
	hashmap_put(m_SendHashMap, FuncID_Send_CommandTrafficFailure, (any_t)Func_Command_Traffic_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_PassingPointSucess, (any_t)Func_Passing_Point_Sucess);
	hashmap_put(m_SendHashMap, FuncID_Send_PassingPointFailure, (any_t)Func_Passing_Point_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_SwitchRouteSucess, (any_t)Func_Switch_Route_Sucess);
	hashmap_put(m_SendHashMap, FuncID_Send_SwitchRouteFailure, (any_t)Func_Switch_Route_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_ItemSelectSucess, (any_t)Func_Item_Select_Sucess);
	hashmap_put(m_SendHashMap, FuncID_Send_ItemSelectFailure, (any_t)Func_Item_Select_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_InquiryDistanceSuccess, (any_t)Func_Inquiry_Distance_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_InquiryDistanceFailure, (any_t)Func_Inquiry_Distance_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_NearbySearchSuccess, (any_t)Func_Nearby_Search_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_NearbySearchFailure, (any_t)Func_Nearby_Search_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_KeyWoardSearchSuccess, (any_t)Func_KeyWoard_Search_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_KeyWoardSearchFailure, (any_t)Func_KeyWoard_Search_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_PassingPoiAndDestPoiSuccess, (any_t)Func_Passing_PoiAndDestPoi_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_PassingPoiAndDestPoiFailure, (any_t)Func_Passing_PoiAndDestPoi_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_CollectionSuccess, (any_t)Func_Collection_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_CollectionFailure, (any_t)Func_Collection_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_StartNaviSuccess, (any_t)Func_Start_Navi_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_StartNaviFailure, (any_t)Func_Start_Navi_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_MeterDirectionInfo, (any_t)Func_Meter_Direction_Info);
	hashmap_put(m_SendHashMap, FuncID_Send_MeterNextTBTInfo, (any_t)Func_Meter_Next_TBTInfo);
	hashmap_put(m_SendHashMap, FuncID_Send_getDeviceConnect, (any_t)Func_Get_Device_Connect);
	hashmap_put(m_SendHashMap, FuncID_Send_EntryPhoneConnect, (any_t)Func_Entry_Phone_Connect);
	hashmap_put(m_SendHashMap, FuncID_Send_ExitPhoneConnect, (any_t)Func_Exit_Phone_Connect);
	hashmap_put(m_SendHashMap, FuncID_Send_PhoneConnectStatus, (any_t)Func_Phone_Connect_Status);
	hashmap_put(m_SendHashMap, FuncID_Send_SilentModeChangeSucess, (any_t)Func_SilentMode_Change_Sucess);
	hashmap_put(m_SendHashMap, FuncID_Send_SilentModeChangeFailure, (any_t)Func_SilentMode_Change_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_UuidError, (any_t)Func_UuidError);
	hashmap_put(m_SendHashMap, FuncID_Send_GetVersionSuccess, (any_t)Func_Get_Version_Success);
	hashmap_put(m_SendHashMap, FuncID_Send_GetVersionFailure, (any_t)Func_Get_Version_Failure);
	hashmap_put(m_SendHashMap, FuncID_Send_SaveCurrentToFavResp, (any_t)Func_Add_Current_To_Fav_Resp);
	hashmap_put(m_SendHashMap, FuncID_Send_SaveDestToFavResp, (any_t)Func_Add_Dest_To_Fav_Resp);
	hashmap_put(m_SendHashMap, FuncID_Send_SavePoiToFavResp, (any_t)Func_Add_Poi_To_Fav_Resp);
	hashmap_put(m_SendHashMap, FuncID_Send_GetDestPointInfoResp, (any_t)Func_Is_Navi_Have_Des_Resp);
	hashmap_put(m_SendHashMap, FuncID_Send_Max, NULL);
}
//delete by shenghao for exit timeout
//void NaviSysIPCWrapperToyota::terminateAll()
//{
//	printf("TEST  NaviSysIPCWrapperToyota terminateAll() \n");
//	m_Pool->terminateAll();
//}

/**
*	@brief
*		获取Navi Acc状态
*	@return
*		BOOL TURE：Acc Off; FALSE: Acc On
*/
BOOL NaviSysIPCWrapperToyota::getNaviAccStatus()
{
	return m_isAccOff;
}

/**
*	@brief
*		获取Navi Acc状态
*	@param [in]data
*		BOOL TURE：Acc Off; FALSE: Acc On
*	@return
*		void
*/
void NaviSysIPCWrapperToyota::setNaviAccStatus(BOOL isAccOff)
{
	m_isAccOff = isAccOff;
}

/**
*	@brief
*		获取Navi Ready状态
*	@param [in]data
*		void
*	@return
*		BOOL TURE：Navi is Ready; FALSE: Not Ready
*/
BOOL NaviSysIPCWrapperToyota::getNaviIsReady()
{
	return m_isNaviReady;
}

/**
*	@brief
*		设置Navi Ready状态
*	@param [in]data
*		BOOL TURE：Navi is Ready; FALSE: Not Ready
*	@return
*		void
*/
void NaviSysIPCWrapperToyota::setNaviIsReady(BOOL isNaviReady)
{
	m_isNaviReady = isNaviReady;
}

/**
*	@brief
*		获取线程池变量
*/
CThreadPool* NaviSysIPCWrapperToyota::getThreadPool()
{
	return m_Pool;
}

/**
*	@brief
*		获取监听
*/
NaviSysIPCWrapperListener* NaviSysIPCWrapperToyota::getListener()
{
	return mpListener;
}

NaviSysIPCWrapperToyota* NaviSysIPCWrapperToyota::getInstance()
{
	if (NULL != m_IpcWrapperInstance)
		return m_IpcWrapperInstance;
	return NULL;
}

/**
*	@brief
*		提供给Receiver发送消息接口
*	@return
*		bool true：成功; false：失败
*	@param [in]data
*		data：需要发送的消息 
*		   [in]priority
*		priority：优先级
*/
bool NaviSysIPCWrapperToyota::sendMsg(void *data, int priority)
{
	UNUSED_VAR(priority);
	if (NULL == data)
	{
		CQ_LOG_ERROR("[ServiceManger]Func:%s,Line:%d,Msg:signal is NULL!\n", __FUNCTION__, __LINE__);
		Mapbar_drclog_printf("sm-sendMsg data is NULL \n");
		return false;
	}	

	cqWCHAR* signalValue = (cqWCHAR*)malloc(sizeof(cqWCHAR)* MSG_MAX_SIZE);
	if (NULL == signalValue)
	{
		Mapbar_drclog_printf("sm-sendMsg signalValue malloc failed \n");
		return false;
	}
	memset(signalValue, 0, sizeof(cqWCHAR)* MSG_MAX_SIZE);
	SendData* paramData = (SendData *)data;

	any_t ptrSendFunc;
	if (hashmap_get(m_SendHashMap, paramData->funcId, &ptrSendFunc) == MAP_OK)
	{
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,MessageID:%x.\n", __FUNCTION__, __LINE__, paramData->funcId);
		SendFuncType func = (SendFuncType)ptrSendFunc;
		func(signalValue, paramData->data);
	}
	// free signalValue
	if (NULL != signalValue)
	{
		free(signalValue);
		signalValue = NULL;
	}
	// free paramData
	if (paramData != NULL)
	{
		if (paramData->data != NULL)
		{
			free(paramData->data);
			paramData->data = NULL;
		}
		free(paramData);
		paramData = NULL;
	}
	return true;
}

/**
*	@brief
*		添加listener
*/
void NaviSysIPCWrapperToyota::addListener(NaviSysIPCWrapperListener *listener)
{
	if (NULL != listener){
		mpListener = listener;
	}
	return;
}

/**
*	@brief
*		移除listener
*/
void NaviSysIPCWrapperToyota::removeListener()
{
	if (NULL != mpListener){
		mpListener = NULL;
	}
	return;
}

/**
*	@brief
*		提供给dbus service使用，用于消息向导航通知
*	@return
*		void
*	@param [in]data
*		dbus消息内容
*/
void NaviSysIPCWrapperToyota::notifyMsg(void* data)
{
	//static BOOL isAccOFF = FALSE;
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,notifyMsg!\n", __FUNCTION__, __LINE__);
	if (NULL == data || m_isAccOff == TRUE)
	{
		CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Signal is NULL!\n", __FUNCTION__, __LINE__);
		Mapbar_drclog_printf("sm-notifyMsg data is NULL or acc is off");
		return;
	}
	
	cqCHAR* message = (cqCHAR*)malloc(sizeof(cqCHAR) * (strlen((cqCHAR*)data) + 1));
	if (NULL == message)
	{
		Mapbar_drclog_printf("sm-notifyMsg message is NULL");
		return;
	}
	cq_strcpy(message, (cqCHAR*)data);

	ServiceManager::NaviData naviData;
	cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, message);
	ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
	// 追加DRC LOG
	Mapbar_drclog_printf("sm-rev msg: %s", message);

	// 取消息ID
	cqCHAR* msgSub = (cqCHAR*)malloc(sizeof(cqCHAR)*MSGID_MAX_SIZE);
	if (NULL == msgSub)
	{
		Mapbar_drclog_printf("sm-notifyMsg msgSub malloc failed");
		free(message);
		message = NULL;
		return;
	}
	int index = 0;
	while (index < MSGID_MAX_SIZE - 1)
	{
		msgSub[index] = message[index];
		index++;
	}
	msgSub[index] = '\0';
	uint32 msgID = cq_atoui_hex(msgSub);
	free(msgSub);
	msgSub = NULL;
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,MessageID:%x.\n", __FUNCTION__, __LINE__, msgID);
	any_t ptrFunc;
	if (hashmap_get(m_RevHashMap, msgID, &ptrFunc) == MAP_OK)
	{
		CQ_LOG_INFO("[ServiceManager]hashmap_get Func:%s,Line:%d,MessageID:%x.\n", __FUNCTION__, __LINE__, msgID);
		RevFuncType func = (RevFuncType)ptrFunc;
		func(message,this);
	}

	if (NULL != message)
	{
		free(message);
		message = NULL;
	}
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, return !\n", __FUNCTION__, __LINE__);
	return;
}
