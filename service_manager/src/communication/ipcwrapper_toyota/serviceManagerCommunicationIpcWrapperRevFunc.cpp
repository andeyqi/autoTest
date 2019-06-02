#include"serviceManagerCommunicationIpcWrapperRevFunc.h"
#include "CThreadPool.h"
#include "serviceManagerOutput.h"
class IPCWrapperJob : public CJob
{
public:
	IPCWrapperJob(void *data) : CJob(data)
	{

	}

	virtual void run()
	{
		ReceiveData* data = (ReceiveData*)getJobdata();
		if (NULL != NaviSysIPCWrapperToyota::getInstance())
		{
			NaviSysIPCWrapperToyota::getInstance()->mpListener->messageListener(data);
		}
	}
};


void Func_GuidancVoice_Start_Ans(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	if (NULL == ipcToyota)
		return;

	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_AnswerVoiceStart;

	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_GuidancVoice_Stop_Ans(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	return;
}

void Func_GuidancVoice_State_Chk(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_checkVoiceState;
	//mpListener->messageListener(paramData);

	// TEST
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_GuidancVoice_Stop_Emg(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_reqStopVoiceEMG;
	//mpListener->messageListener(paramData);

	// TEST
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Show(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_ReqShowNaviForm;
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Hide(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_ReqHideNaviForm;
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Show_Sucess(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_showOtherFormSuc;
	//mpListener->messageListener(paramData);

	// TEST
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Hide_Sucess(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_hideOtherFormSuc;
	//mpListener->messageListener(paramData);

	// TEST
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Ill_On(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_sgnILLStateChanged;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = TRUE;
	//mpListener->messageListener(paramData);	
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Ill_Off(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_sgnILLStateChanged;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = FALSE;
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Acc(cqCHAR* message, void* ipcToyota){
	//isAccOFF = TRUE;	//FIXME
	((NaviSysIPCWrapperToyota*)ipcToyota)->setNaviAccStatus(TRUE);
	CQ_LOG_ERROR("[ServiceManager] Get Debus Message DBUS_WRAPPER_SIG_ACC\n");
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_reqACCOFF;
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Gps_Time(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_reqGetGPSTimeValue;
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Volume(cqCHAR* message, void* ipcToyota){
	int32 reqVolume = -1;
	if (':' == message[8] && '\0' != message[9])
		reqVolume = cq_atoui_hex(&message[9]);
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_VOLUM index is %d.\n", __FUNCTION__, __LINE__, reqVolume);

	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_AnswerVolume;
	paramData->data = (int32*)malloc(sizeof(int32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &reqVolume, sizeof(int32));
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Mute(cqCHAR* message, void* ipcToyota){
	uint32 reqMuteVolume = 0;
	if (':' == message[8])
		reqMuteVolume = cq_atoui_hex(&message[9]);
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_MUTE index is %d.\n", __FUNCTION__, __LINE__, reqMuteVolume);

	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_AnswerMute;
	paramData->data = (uint32*)malloc(sizeof(uint32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &reqMuteVolume, sizeof(uint32));
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Uuid_Value(cqCHAR* message, void* ipcToyota){
	cqCHAR uuidValue[32] = { 0 };
	int index = 0;
	if (':' == message[8])
	{
		for (; message[index + 9] != '\0'; index++)
			uuidValue[index] = message[index + 9];
		uuidValue[index] = '\0';
		CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_UUID_VALUE ,%s.\n", __FUNCTION__, __LINE__, uuidValue);
	}
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_getUUID;
	if (index == 0)
	{
		paramData->data = NULL;
	}
	else
	{
		paramData->data = (cqCHAR*)malloc(sizeof(cqCHAR)* (cq_strlen(uuidValue) + 1));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		cq_strcpy((cqCHAR*)paramData->data, uuidValue);
	}
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Navi_Request(cqCHAR* message, void* ipcToyota){
	uint32 index = IDX_ADDR_INVALID;
	// 返回signal数据示例： 20000010:01
	if (':' == message[8])
	{
		int32 reqIndex = cq_atoi(&message[10]);
		if (1 == reqIndex)   // 请求导航到家
		{
			index = IDX_HOME;
		}
		else if (2 == reqIndex)   // 请求导航到公司
		{
			index = IDX_OFFICE;
		}
	}
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_NAVIREQUEST index is %u.\n", __FUNCTION__, __LINE__, index);

	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_AddrStatus;
	paramData->data = (uint32*)malloc(sizeof(uint32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &index, sizeof(uint32));
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Runsts(cqCHAR* message, void* ipcToyota){

	int32 runStatus = 0;
	if (':' == message[8] && '\0' != message[9])
		runStatus = cq_atoi(&message[9]);
	else
		runStatus = -1;
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_RUNSTS ,%d.\n", __FUNCTION__, __LINE__, runStatus);

	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_RunStatusChanged;
	paramData->data = (int32*)malloc(sizeof(int32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &runStatus, sizeof(int32));
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Wifi_Connected(cqCHAR* message, void* ipcToyota){
	EN_ADDITIONSVCENGINE_NETWORK_STATE networkStatus = EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID;
	if (':' == message[8] && '\0' != message[9])
	{
		networkStatus = (EN_ADDITIONSVCENGINE_NETWORK_STATE)cq_atoui_hex(&message[9]);
		CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_REV_INFO index is %d.\n", __FUNCTION__, __LINE__, networkStatus);
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_NetworkStatusChanged;
		paramData->data = (EN_ADDITIONSVCENGINE_NETWORK_STATE*)malloc(sizeof(EN_ADDITIONSVCENGINE_NETWORK_STATE));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &networkStatus, sizeof(EN_ADDITIONSVCENGINE_NETWORK_STATE));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
	else
	{
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_NetworkStatusChanged;
		paramData->data = NULL;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
}
void Func_Wifi_DisConnected(cqCHAR* message, void* ipcToyota){}
void Func_Wifi_Connecting(cqCHAR* message, void* ipcToyota){}
void Func_Weather_Req(cqCHAR* message, void* ipcToyota){}
void Func_Memory_Clear(cqCHAR* message, void* ipcToyota){
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_MEMORY_CLEAR.\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_HistoryClear;
	paramData->data = NULL;
	//mpListener->messageListener(paramData);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Systimer_Edit(cqCHAR* message, void* ipcToyota){}
void Func_Message_Dr(cqCHAR* message, void* ipcToyota){}
void Func_Usb_Mount(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_GetUsbStatus;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = TRUE;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Usb_UnMount(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_GetUsbStatus;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = FALSE;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Usb_DateTime(cqCHAR* message, void* ipcToyota){}
void Func_Usb_DateTime_Change(cqCHAR* message, void* ipcToyota){}
void Func_MusicWidget_Show_Sucess(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_MusicWidgeShowChanged;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = TRUE;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_MusicWidget_Show_Failure(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_MusicWidgeShowChanged;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = FALSE;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Navi_Command(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRStartNavi;
	paramData->data = NULL;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Ude_Search(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRUdeSearch;
	char buff_temp[100] = { 0 };
	//cqCHAR ptmp;
	//int temp_data = 0;
	//sscanf(message,"%x:%[^/0]",&temp_data,buff_temp);
	if(cq_strchr(message, ':'))
	{
		cq_strncpy(buff_temp, cq_strchr(message, ':')+1,100);
	}
	if (!strlen(buff_temp))
	{
		paramData->data = NULL;
	}
	else
	{
		paramData->data = malloc(strlen(buff_temp) + 1);
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, buff_temp, strlen(buff_temp) + 1);
	}
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	CQ_LOG_INFO("[ServiceManager]function id [%x] data is [%s]\n", paramData->funcId, paramData->data);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Search_Nearby(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRSearchNearby;
	char buff_temp[100] = { 0 };
	//int temp_data = 0;
	//sscanf(message,"%x:%[^/0]",&temp_data,buff_temp);
	if(cq_strchr(message, ':'))
	{
		cq_strncpy(buff_temp, cq_strchr(message, ':')+1,100);
	}
	if (!strlen(buff_temp))
	{
		paramData->data = NULL;
	}
	else
	{
		paramData->data = malloc(strlen(buff_temp) + 1);
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, buff_temp, strlen(buff_temp) + 1);
	}
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	CQ_LOG_INFO("[ServiceManager]function id [%x] data is [%d]\n", paramData->funcId, paramData->data);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Passing_Point(cqCHAR* message, void* ipcToyota){
	int32 reqType = -1;
	if (':' == message[8] && '\0' != message[9])
	{
		cqCHAR cqtype = message[9];
		reqType = cq_atoi(&cqtype);
	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRPassingPoint;
	paramData->data = (int32 *)malloc(sizeof(int32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &reqType, sizeof(int32));
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Command_Control(cqCHAR* message, void* ipcToyota){
	CommandControlSig* commandStruct = NULL;
	if (':' == message[8] && '\0' != message[9])
	{
		commandStruct = (CommandControlSig*)malloc(sizeof(CommandControlSig));
		if (NULL == commandStruct)
		{
			return;
		}
		commandStruct->uiCommand = VR_COMMAND_INVALIDE;
		commandStruct->uiMapMode = VR_COMMAND_MAP_INVALIDE;

		cqCHAR comID = message[9];
		if (cq_atoi(&comID) < VR_COMMAND_INVALIDE)
			commandStruct->uiCommand = (VR_COMMAND_ID)cq_atoi(&comID);
		if (',' == message[10] && '\0' != message[11] && commandStruct->uiCommand == VR_COMMAND_CHANGE_MAP_TO)
		{
			cqCHAR modID = message[11];
			if (cq_atoi(&modID) < VR_COMMAND_MAP_INVALIDE)
				commandStruct->uiMapMode = (VR_COMMAND_MAP_MODE)cq_atoi(&modID);
		}
	}
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != commandStruct)
		{
			free(commandStruct);
			commandStruct = NULL;
		}
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRCommandControl;
	paramData->data = commandStruct;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Switch_Route(cqCHAR* message, void* ipcToyota){
	VR_COMMAND_ROUTE_PREFER routeprefer = VR_COMMAND_ROUTE_PREFER_INVALIDE;
	if (':' == message[8] && '\0' != message[9])
	{
		routeprefer = (VR_COMMAND_ROUTE_PREFER)cq_atoui_hex(&message[9]);
	}
	CQ_LOG_INFO("[ServiceManager]message[9] is %d routeprefer  is %d \n", message[9], routeprefer);
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRSwitchRoute;
	paramData->data = (VR_COMMAND_ROUTE_PREFER*)malloc(sizeof(VR_COMMAND_ROUTE_PREFER));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &routeprefer, sizeof(VR_COMMAND_ROUTE_PREFER));
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	CQ_LOG_INFO("[ServiceManager]function id [%x] data is [%d]\n", paramData->funcId, paramData->data);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Traffic_Control(cqCHAR* message, void* ipcToyota){
	VR_COMMAND_TRAFFIC_SIG trafficSig = VR_COMMAND_TRAFFIC_INVALIDE;
	if (':' == message[8] && '\0' != message[9])
	{
		if (cq_atoi(&message[9]) < VR_COMMAND_TRAFFIC_INVALIDE)
		{
			trafficSig = (VR_COMMAND_TRAFFIC_SIG)cq_atoi(&message[9]);
		}
	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRTrafficControl;
	paramData->data = (VR_COMMAND_TRAFFIC_SIG*)malloc(sizeof(VR_COMMAND_TRAFFIC_SIG));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &trafficSig, sizeof(VR_COMMAND_TRAFFIC_SIG));
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Inquiry_Distance(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRInquiryDistance;
	paramData->data = NULL;
	CQ_LOG_INFO("[ServiceManager]function id [%x]\n", paramData->funcId);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Collection(cqCHAR* message, void* ipcToyota){
	CQ_LOG_INFO("[ServiceManager] DBUS_WRAPPER_SIG_COLLECTION message = %s \n", message);
	int32 collection = -1;

	if (':' == message[8] && '\0' != message[9])
	{
		collection = cq_atoi(&message[9]);
	}
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_REV_INFO index is %d.\n", __FUNCTION__, __LINE__, collection);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRCollection;
	paramData->data = (uint32*)malloc(sizeof(uint32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &collection, sizeof(uint32));
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Item_Select(cqCHAR* message, void* ipcToyota){
	int index = 8;
	VRPoiInfo* poiInfo = (VRPoiInfo*)malloc(sizeof(VRPoiInfo));
	if (':' == message[8] && '\0' != message[9])
	{
		//poiInfo = (VRPoiInfo*)malloc(sizeof(VRPoiInfo));
		if (NULL == poiInfo)
		{
			Mapbar_drclog_printf("sm-sendMsg poiInfo malloc failed \n");
			free(message);
			return;
		}
		int indexLon = 0;
		int indexLat = 0;
		int indexName = 0;
		cqCHAR lon[12] = { 0 };
		cqCHAR lat[12] = { 0 };
		cqCHAR name[128] = { 0 };

		// 经度1
		index++; //跳过':'
		for (; message[index] != '\0' && message[index] != ','; indexLon++, index++)
			lon[indexLon] = message[index];
		lon[indexLon] = '\0';
		poiInfo->lon = cq_atoi(lon);

		// 纬度1
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexLat++, index++)
			lat[indexLat] = message[index];
		lat[indexLat] = '\0';
		poiInfo->lat = cq_atoi(lat);

		// 名称1
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexName++, index++)
			name[indexName] = message[index];
		name[indexName] = '\0';
		memcpy(poiInfo->name, name, cq_strlen(name) + 1);
	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != poiInfo)
		{
			free(poiInfo);
			poiInfo = NULL;
		}
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRItemSelect;
	paramData->data = poiInfo;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Language_Switch(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	int32 languageStatus = -1;

	if (':' == message[8] && '\0' != message[9])
	{
		languageStatus = cq_atoi(&message[9]);
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_MUTE index is %d.\n", __FUNCTION__, __LINE__, languageStatus);
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_LanguageSwitchChanged;
		paramData->data = (int32*)malloc(sizeof(int32));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &languageStatus, sizeof(int32));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
	else
	{
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_LanguageSwitchChanged;
		paramData->data = NULL;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
}
void Func_DateTime_Dvalue(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	int index = 8;
	TimeInfo* timeInfo = (TimeInfo*)malloc(sizeof(TimeInfo));
	if (NULL == timeInfo)
	{
		return;
	}
	if (':' == message[8] && '\0' != message[9])
	{
		//timeInfo = (TimeInfo*)malloc(sizeof(TimeInfo));			
		timeInfo->timeValue = 0;
		timeInfo->timeFormat = EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN;

		int indexTimeValue = 0;
		int indexTimeFormat = 0;
		cqCHAR timeValue[64] = { 0 };
		cqCHAR timeFormat[12] = { 0 };

		// 时间
		index++; //跳过':'
		for (; message[index] != '\0' && message[index] != ','; indexTimeValue++, index++)
			timeValue[indexTimeValue] = message[index];
		timeValue[indexTimeValue] = '\0';
		timeInfo->timeValue = cq_atoui_hex(timeValue);

		if (message[index] == ',')
		{
			// 时制
			index++; //跳过','
			for (; message[index] != '\0' && message[index] != ','; indexTimeFormat++, index++)
				timeFormat[indexTimeFormat] = message[index];
			timeFormat[indexTimeFormat] = '\0';
			int  timeFormatTemp = cq_atoi(timeFormat);

			if (0 == timeFormatTemp)
			{
				timeInfo->timeFormat = EN_DISPLAYSVCENGINE_TIME_FORMAT_24HOUR;
			}
			else if (1 == timeFormatTemp)
			{
				timeInfo->timeFormat = EN_DISPLAYSVCENGINE_TIME_FORMAT_12HOUR;
			}
		}
	}
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != timeInfo)
		{
			free(timeInfo);
			timeInfo = NULL;
		}
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_SysHmiTimeChanged;
	paramData->data = timeInfo;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	SM_INFO_LOG("IPCWrapper data paramData = %p", paramData);
	SM_INFO_LOG("IPCWrapper data paramData->data = %p", paramData->data);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_MusicWidget_Hide_Sucess(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_MusicWidgeHideChanged;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = TRUE;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_MusicWidget_Hide_Failure(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_MusicWidgeHideChanged;
	paramData->data = (BOOL*)malloc(sizeof(BOOL));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	*((BOOL*)paramData->data) = FALSE;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_Connect_Type(cqCHAR* message, void* ipcToyota){
	EN_ADDITIONSVCENGINE_NETWORK_STATE networkStatus = EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID;
	if (':' == message[8] && '\0' != message[9])
	{
		networkStatus = (EN_ADDITIONSVCENGINE_NETWORK_STATE)cq_atoui_hex(&message[9]);
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_REV_INFO index is %d.\n", __FUNCTION__, __LINE__, networkStatus);
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_NetworkStatusChanged;
		paramData->data = (EN_ADDITIONSVCENGINE_NETWORK_STATE*)malloc(sizeof(EN_ADDITIONSVCENGINE_NETWORK_STATE));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &networkStatus, sizeof(EN_ADDITIONSVCENGINE_NETWORK_STATE));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
	else
	{
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_NetworkStatusChanged;
		paramData->data = NULL;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
}
void Func_Rev_Info(cqCHAR* message, void* ipcToyota){}
void Func_Gbook_Info(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	cqCHAR gbookInfo[256] = { 0 };
	int index = 0;
	if (':' == message[8] && '\0' != message[9])
	{
		for (; message[index + 9] != '\0'; index++)
			gbookInfo[index] = message[index + 9];
		gbookInfo[index] = '\0';
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_GBOOK_INFO ,%s.\n", __FUNCTION__, __LINE__, gbookInfo);
	}

	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_GbookInfoChanged;
	if (index == 0)
	{
		paramData->data = NULL;
	}
	else
	{
		paramData->data = (cqCHAR*)malloc(sizeof(cqCHAR)* (cq_strlen(gbookInfo) + 1));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		cq_strcpy((cqCHAR*)paramData->data, gbookInfo);
	}
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Search_Dest_Via_Passing_Poi(cqCHAR* message, void* ipcToyota)
{
	CQ_LOG_INFO("[ServiceManager] DBUS_WRAPPER_SIG_SEARCH_DEST_VIA_PASSING_POI message = %s \n", message);
	SM_INFO_LOG("Func_Search_Dest_Via_Passing_Poi");
	VRViaAndDestPoiInfo* poiInfo = NULL;
	int32 index = 8;
	if (':' == message[8] && '\0' != message[9])
	{
		SM_INFO_LOG("Func_Search_Dest_Via_Passing_Poi 1");
		poiInfo = (VRViaAndDestPoiInfo*)malloc(sizeof(VRViaAndDestPoiInfo));
		if (NULL == poiInfo)
		{
			SM_INFO_LOG("Func_Search_Dest_Via_Passing_Poi poiInfo = NULL 2 ");
			free(message);
			return;
		}
		//int index = 9;
		int indexLon = 0;
		int indexLat = 0;
		int indexName = 0;
		cqCHAR lon[12] = { 0 };
		cqCHAR lat[12] = { 0 };
		cqCHAR name[128] = { 0 };

		// 经度1
		index++; //跳过':'
		for (; message[index] != '\0' && message[index] != ','; indexLon++, index++)
			lon[indexLon] = message[index];
		lon[indexLon] = '\0';
		poiInfo->viaLon = cq_atoi(lon);

		// 纬度1
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexLat++, index++)
			lat[indexLat] = message[index];
		lat[indexLat] = '\0';
		poiInfo->viaLat = cq_atoi(lat);

		// 名称1
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexName++, index++)
			name[indexName] = message[index];
		name[indexName] = '\0';
		memcpy(poiInfo->viaName, name, cq_strlen(name) + 1);
		//poiInfo->viaName = name;
		SM_INFO_LOG("Func_Search_Dest_Via_Passing_Poi 3 ");
		int indexLon2 = 0;
		int indexLat2 = 0;
		int indexName2 = 0;
		cqCHAR lon2[12] = { 0 };
		cqCHAR lat2[12] = { 0 };
		cqCHAR name2[128] = { 0 };

		// 经度2
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexLon2++, index++)
			lon2[indexLon2] = message[index];
		lon2[indexLon2] = '\0';
		poiInfo->destLon = cq_atoi(lon2);

		// 纬度2
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexLat2++, index++)
			lat2[indexLat2] = message[index];
		lat2[indexLat2] = '\0';
		poiInfo->destLat = cq_atoi(lat2);

		// 名称2
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexName2++, index++)
			name2[indexName2] = message[index];
		name2[indexName2] = '\0';
		memcpy(poiInfo->destName, name2, cq_strlen(name2) + 1);
		//poiInfo->destName = name2;
		SM_INFO_LOG("Func_Search_Dest_Via_Passing_Poi 4 ");
	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != poiInfo)
		{
			free(poiInfo);
			poiInfo = NULL;
		}
		return;
	}
	SM_INFO_LOG("Func_Search_Dest_Via_Passing_Poi 5 ");
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_VRPassPointAndDestPoint;
	paramData->data = poiInfo;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_DayNight_Mode_Switch(cqCHAR* message, void* ipcToyota){
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_SysDayNightLinkAge;
	char buff_temp[100] = { 0 };
	//int temp_data = 0;
	//sscanf(message,"%x:%s",&temp_data,buff_temp);
	if(cq_strchr(message, ':'))
	{
		cq_strncpy(buff_temp, cq_strchr(message, ':')+1,100);
	}
	if (!strlen(buff_temp))
	{
		paramData->data = NULL;
	}
	else
	{
		paramData->data = malloc(strlen(buff_temp) + 1);
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, buff_temp, strlen(buff_temp) + 1);
	}
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	CQ_LOG_INFO("[ServiceManager]function id [%x] data is [%d]\n", paramData->funcId, paramData->data);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Touch_Enable(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_TouchStatus;
	TouchInfo* touch = (TouchInfo*)malloc(sizeof(TouchInfo));
	if (NULL == touch)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	touch->status = true;
	touch->tickCount = Mapbar_getTickCount();
	paramData->data = touch;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Touch_Disable(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_TouchStatus;
	TouchInfo * touch2 = (TouchInfo*)malloc(sizeof(TouchInfo));
	if (NULL == touch2)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	touch2->status = false;
	touch2->tickCount = Mapbar_getTickCount();
	paramData->data = touch2;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	Mapbar_sleep(1);
}
void Func_Device_Connect_NTY(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	int index = 8;
	DeviceInfo* deviceInfo = NULL;
	if (':' == message[8] && '\0' != message[9])
	{
		deviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
		if (NULL == deviceInfo)
		{
			return;
		}
		deviceInfo->connectType = EN_ADDITONSVCENGINE_DEVICE_STATE_CONNECT;
		deviceInfo->VendorId = 0;
		deviceInfo->ProductId = 0;
		deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID;

		int indexVendor = 0;
		int indexProduct = 0;
		int indexdeviceType = 0;
		int indexprotocolType = 0;
		cqCHAR Vendor[12] = { 0 };
		cqCHAR Product[12] = { 0 };
		cqCHAR DeviceType[12] = { 0 };
		cqCHAR ProtocolType[12] = { 0 };

		// VendorId
		index++; //跳过':'
		for (; message[index] != '\0' && message[index] != ','; indexVendor++, index++)
			Vendor[indexVendor] = message[index];
		Vendor[indexVendor] = '\0';
		deviceInfo->VendorId = strtoul(Vendor, NULL, 16);


		// ProductId
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexProduct++, index++)
			Product[indexProduct] = message[index];
		Product[indexProduct] = '\0';
		deviceInfo->ProductId = strtoul(Product, NULL, 16);

		// 设备型号
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexdeviceType++, index++)
			DeviceType[indexdeviceType] = message[index];
		DeviceType[indexdeviceType] = '\0';
		int32 deviceTypeTemp = (int32)cq_atoi(&DeviceType[0]);

		if (1 == deviceTypeTemp)
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID;
		}
		else if (2 == deviceTypeTemp)
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_IOS;
		}
		else
		{
			CQ_LOG_ERROR("[ServiceManger]Func:%s,Line:%d,Msg:device type is invalid!\n", __FUNCTION__, __LINE__);
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		}

		//设备支持的协议
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexprotocolType++, index++)
			ProtocolType[indexprotocolType] = message[index];
		ProtocolType[indexprotocolType] = '\0';
		int32 protocolTypeTemp2 = (int32)cq_atoi(&ProtocolType[0]);

		if (1 == protocolTypeTemp2)
		{
			if (deviceInfo->deviceType == EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID)
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_AOA1;
			}
			else
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_iAP1;
			}
		}
		else if (2 == protocolTypeTemp2)
		{
			if (deviceInfo->deviceType == EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID)
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_AOA2;
			}
			else
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_iAP2;
			}
		}
		else
		{
			CQ_LOG_ERROR("[ServiceManger]Func:%s,Line:%d,Msg:device protocol is invalid!\n", __FUNCTION__, __LINE__);
			deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID;
		}

	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != deviceInfo)
		{
			free(deviceInfo);
			deviceInfo = NULL;
		}
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_DeviceConnectChanged;
	paramData->data = deviceInfo;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Device_Disconnect_NTY(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	int index = 8;
	DeviceInfo* deviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
	if (':' == message[8] && '\0' != message[9])
	{
		//deviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
		if (NULL == deviceInfo)
		{
			return;
		}
		deviceInfo->connectType = EN_ADDITONSVCENGINE_DEVICE_STATE_DISCONNECT;
		deviceInfo->VendorId = 0;
		deviceInfo->ProductId = 0;
		deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID;

		int indexVendor = 0;
		int indexProduct = 0;
		int indexdeviceType = 0;
		cqCHAR Vendor[12] = { 0 };
		cqCHAR Product[12] = { 0 };
		cqCHAR DeviceType[12] = { 0 };

		// VendorId
		index++; //跳过':'
		for (; message[index] != '\0' && message[index] != ','; indexVendor++, index++)
			Vendor[indexVendor] = message[index];
		Vendor[indexVendor] = '\0';
		deviceInfo->VendorId = strtoul(Vendor, NULL, 16);


		// ProductId
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexProduct++, index++)
			Product[indexProduct] = message[index];
		Product[indexProduct] = '\0';
		deviceInfo->ProductId = strtoul(Product, NULL, 16);

		// 设备型号
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexdeviceType++, index++)
			DeviceType[indexdeviceType] = message[index];
		DeviceType[indexdeviceType] = '\0';
		int32 deviceTypeTemp = (int32)cq_atoi(&DeviceType[0]);

		if (1 == deviceTypeTemp)
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID;
		}
		else if (2 == deviceTypeTemp)
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_IOS;
		}
		else
		{
			CQ_LOG_ERROR("[ServiceManger]Func:%s,Line:%d,Msg:device type is invalid!\n", __FUNCTION__, __LINE__);
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		}
	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != deviceInfo)
		{
			free(deviceInfo);
			deviceInfo = NULL;
		}
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_DeviceConnectChanged;
	paramData->data = deviceInfo;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Device_Connect_Info_Res(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	int index = 8;
	DeviceInfo* deviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
	if (':' == message[8] && '\0' != message[9])
	{
		//deviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
		if (NULL == deviceInfo)
		{
			return;
		}
		deviceInfo->connectType = EN_ADDITONSVCENGINE_DEVICE_STATE_CONNECT;
		deviceInfo->VendorId = 0;
		deviceInfo->ProductId = 0;
		deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID;

		int indexVendor = 0;
		int indexProduct = 0;
		int indexdeviceType = 0;
		int indexprotocolType = 0;
		cqCHAR Vendor[12] = { 0 };
		cqCHAR Product[12] = { 0 };
		cqCHAR DeviceType[12] = { 0 };
		cqCHAR ProtocolType[12] = { 0 };

		// VendorId
		index++; //跳过':'
		for (; message[index] != '\0' && message[index] != ','; indexVendor++, index++)
			Vendor[indexVendor] = message[index];
		Vendor[indexVendor] = '\0';
		deviceInfo->VendorId = strtoul(Vendor, NULL, 16);


		// ProductId
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexProduct++, index++)
			Product[indexProduct] = message[index];
		Product[indexProduct] = '\0';
		deviceInfo->ProductId = strtoul(Product, NULL, 16);

		// 设备型号
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexdeviceType++, index++)
			DeviceType[indexdeviceType] = message[index];
		DeviceType[indexdeviceType] = '\0';
		int32 deviceTypeTemp = (int32)cq_atoi(&DeviceType[0]);

		if (1 == deviceTypeTemp)
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID;
		}
		else if (2 == deviceTypeTemp)
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_IOS;
		}
		else
		{
			deviceInfo->deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		}

		//设备支持的协议
		index++; //跳过','
		for (; message[index] != '\0' && message[index] != ','; indexprotocolType++, index++)
			ProtocolType[indexprotocolType] = message[index];
		ProtocolType[indexprotocolType] = '\0';
		int32 protocolTypeTemp2 = (int32)cq_atoi(&ProtocolType[0]);

		if (1 == protocolTypeTemp2)
		{
			if (deviceInfo->deviceType == EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID)
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_AOA1;
			}
			else
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_iAP1;
			}
		}
		else if (2 == protocolTypeTemp2)
		{
			if (deviceInfo->deviceType == EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID)
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_AOA2;
			}
			else
			{
				deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_iAP2;
			}
		}
		else
		{
			deviceInfo->protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID;
		}

	}
	//ReceiveData* paramData = NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		if (NULL != deviceInfo)
		{
			free(deviceInfo);
			deviceInfo = NULL;
		}
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_DeviceConnectChanged;
	paramData->data = deviceInfo;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Language_Res_Sucess(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	int32 languageStatus = -1;
	if (':' == message[8] && '\0' != message[9])
	{
		languageStatus = cq_atoi(&message[9]);
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_MUTE index is %d.\n", __FUNCTION__, __LINE__, languageStatus);
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_GetLanguageID;
		paramData->data = (int32*)malloc(sizeof(int32));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &languageStatus, sizeof(int32));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
	else
	{
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_GetLanguageID;
		paramData->data = NULL;
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
}
void Func_Language_Res_Failure(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_GetLanguageID;
	paramData->data = NULL;
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_Entry_Phone_Connect_Res(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	Mapbar_sleep(1);
	EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE entryPhoneStatus = EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_INVALID;
	if (':' == message[8] && '\0' != message[9])
	{
		entryPhoneStatus = (EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE)cq_atoui_hex(&message[9]);
		CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_REV_INFO index is %d.\n", __FUNCTION__, __LINE__, entryPhoneStatus);
		ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
		if (NULL == paramData)
		{
			return;
		}
		memset(paramData, 0, sizeof(ReceiveData));
		paramData->funcId = FuncID_Receive_EntryPhoneConnectResponse;
		paramData->data = (EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE*)malloc(sizeof(EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE));
		if (NULL == paramData->data)
		{
			free(paramData);
			paramData = NULL;
			return;
		}
		memcpy(paramData->data, &entryPhoneStatus, sizeof(EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE));
		IPCWrapperJob* job = new IPCWrapperJob(paramData);
		((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
	}
}
void Func_SilentMode_Req(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager] DBUS_WRAPPER_SIG_SILENTMODE_REQ message = %s \n", message);
	int32 silentMode = -1;

	if (':' == message[8] && '\0' != message[9])
	{
		silentMode = cq_atoi(&message[9]);
	}
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message: DBUS_WRAPPER_SIG_SILENTMODE_REQ index is %d.\n", __FUNCTION__, __LINE__, silentMode);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_SilentModeChanged;
	paramData->data = (uint32*)malloc(sizeof(uint32));
	if (NULL == paramData->data)
	{
		free(paramData);
		paramData = NULL;
		return;
	}
	memcpy(paramData->data, &silentMode, sizeof(uint32));
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}
void Func_NaviStart_Req(cqCHAR* message, void* ipcToyota){}
void Func_NaviVersion_Req(cqCHAR* message, void* ipcToyota){
	CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message\n", __FUNCTION__, __LINE__);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == paramData)
	{
		return;
	}
	memset(paramData, 0, sizeof(ReceiveData));
	paramData->funcId = FuncID_Receive_ReqNaviVersion;
	paramData->data = NULL;
	CQ_LOG_INFO("[ServiceManager]function id [%x]\n", paramData->funcId);
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}



void Func_VRSaveCurToFav_Req(cqCHAR* message, void* ipcToyota)
{
	cqCHAR * data;

	data = (cqCHAR*)malloc(sizeof(cqCHAR)+1);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if(NULL == paramData || NULL == data)
	{
		return;
	}
	data[0] = '0';
	data[1] = '/0';

	paramData->funcId = FuncID_Receive_AddFavPoint;
	paramData->data = NULL;

	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_VRSaveDstToFav_Req(cqCHAR* message, void* ipcToyota)
{
	cqCHAR * data;

	data = (cqCHAR*)malloc(sizeof(cqCHAR)+1);
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if(NULL == paramData || NULL == data)
	{
		return;
	}
	data[0] = '1';
	data[1] = '/0';	

	paramData->funcId = FuncID_Receive_AddFavPoint;
	paramData->data = NULL;

	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_VRSavePOIToFav_Req(cqCHAR* message, void* ipcToyota)
{
	cqCHAR * data,* ptmp;
	uint16_t  len;
	#define VR_ADD_FAV_TYPE_LEN 2
	
	ptmp = cq_strchr(message, ':');
	len = cq_strlen(ptmp+1);
	data = ptmp ? (cqCHAR * )malloc(len+1+VR_ADD_FAV_TYPE_LEN) : NULL;
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));

	if(NULL == paramData || NULL == data)
	{
		return;
	}

	data[0] = '2';
	data[1] = ',';
	cq_strncpy(data+VR_ADD_FAV_TYPE_LEN, ptmp + 1,len);
	data[len+VR_ADD_FAV_TYPE_LEN] = '/0';
	
	paramData->funcId = FuncID_Receive_AddFavPoint;
	paramData->data = data;

	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);
}

void Func_VRNaviIsHaveDestPoint_Req(cqCHAR* message, void* ipcToyota)
{
	ReceiveData* paramData = (ReceiveData*)malloc(sizeof(ReceiveData));

	if( NULL == paramData )
	{
		return;
	}
	paramData->funcId = FuncID_Receive_NaviIsHaveDest;
	paramData->data = NULL;
	
	IPCWrapperJob* job = new IPCWrapperJob(paramData);
	((NaviSysIPCWrapperToyota*)ipcToyota)->getThreadPool()->run(job);	
}

void Func_Max(cqCHAR* message, void* ipcToyota){} 
