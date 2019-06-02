#include"serviceManagerCommunicationIpcWrapperSendFunc.h"

int16 ChangeturnIcon(TNaviTurnIconID turnIcon)
{
	int16 result = 0;
	switch (turnIcon)
	{
	case ENTI_Arrival:
	{
		result = 6;
	}
		break;
	case ENTI_TurnAround:
	{
		result = 25;
	}
		break;
	case ENTI_EnterMainRoute:
	{
		result = 0;
	}
		break;
	case ENTI_EnterRotary:
	{
		result = 0;
	}
		break;
	case ENTI_GoStraight:
	{
		result = 20;
	}
		break;
	case ENTI_LeaveMainRoute:
	{
		result = 0;
	}
		break;
	case ENTI_LeaveRotary:
	{
		result = 275;
	}
		break;
	case ENTI_TurnLeft:
	{
		result = 27;
	}
		break;
	case ENTI_TurnRight:
	{
		result = 22;
	}
		break;
	case ENTI_TurnSlightlyLeft:
	{
		result = 28;
	}
		break;
	case ENTI_TurnSlightlyRight:
	{
		result = 21;
	}
		break;
	case ENTI_Rotary1:
	{
		result = 235;
	}
		break;
	case ENTI_Rotary2:
	{
		result = 239;
	}
		break;
	case ENTI_Rotary3:
	{
		result = 243;
	}
		break;
	case ENTI_Rotary4:
	{
		result = 247;
	}
		break;
	case ENTI_Rotary5:
	{
		result = 251;
	}
		break;
	case ENTI_Rotary6:
	{
		result = 255;
	}
		break;
	case ENTI_Rotary7:
	{
		result = 259;
	}
		break;
	case ENTI_Rotary8:
	{
		result = 263;
	}
		break;
	case ENTI_Rotary9:
	{
		result = 267;
	}
		break;
	case ENTI_KeepLeft:
	{
		result = 29;
	}
		break;
	case ENTI_KeepRight:
	{
		result = 30;
	}
		break;
	case ENTI_TurnHardLeft:
	{
		result = 26;
	}
		break;
	case ENTI_TurnHardRight:
	{
		result = 23;
	}
		break;
	case ENTI_TurnLeftKeepLeft:
	{
		result = 37;
	}
		break;
	case ENTI_TurnLeftKeepRight:
	{
		result = 38;
	}
		break;
	case ENTI_TurnRightKeepLeft:
	{
		result = 39;
	}
		break;
	case ENTI_TurnRightKeepRight:
	{
		result = 40;
	}
		break;
	case ENTI_EnterTunnel:
	{
		result = 13;
	}
		break;
	case ENTI_TakeFerry:
	{
		result = 0;
	}
		break;
	case ENTI_Start:
	{
		result = 0;
	}
		break;
	case ENTI_WayPoints1:
	{
		result = 306;
	}
		break;
	case ENTI_WayPoints2:
	{
		result = 307;
	}
		break;
	case ENTI_WayPoints3:
	{
		result = 308;
	}
		break;
	case ENTI_IC:
	{
		result = 0;
	}
		break;
	case ENTI_DR:
	{
		result = 0;
	}
		break;
	case ENTI_Overpass:
	{
		result = 0;
	}
		break;
	case ENTI_WindOverpass:
	{
		result = 0;
	}
		break;
	case ENTI_StraightKeepLeft:
	{
		result = 18;
	}
		break;
	case ENTI_StraightKeepRight:
	{
		result = 19;
	}
		break;
	case ENTI_Stair:
	{
		result = 0;
	}
		break;
	case ENTI_Underpass:
	{
		result = 0;
	}
		break;
	case ENTI_Footbridge:
	{
		result = 0;
	}
		break;
	case ENTI_Square:
	{
		result = 0;
	}
		break;
	case ENTI_Bridge:
	{
		result = 0;
	}
		break;
	case ENTI_Park:
	{
		result = 0;
	}
		break;
	case ENTI_3WayForkLeft:
	{
		result = 55;
	}
		break;
	case ENTI_3WayForkMiddle:
	{
		result = 56;
	}
		break;
	case ENTI_3WayForkRight:
	{
		result = 57;
	}
		break;
	case ENTI_WayPoints4:
	{
		result = 309;
	}
		break;
	case ENTI_WayPoints5:
	{
		result = 310;
	}
		break;
	case ENTI_EnterTollStation:
	{
		result = 11;
	}
		break;
	default:

		break;
	}
	CQ_LOG_INFO("[ServiceManager] ChangeturnIcon result = %d \n", result);
	return result;
}

void Func_Navi_Start_Suc(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_CAUTION_SHOW);
	cqCHAR signalValueBufferCautionShow[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBufferCautionShow, MSG_MAX_SIZE);
	//最终构造形式是："10000017"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBufferCautionShow);

	memset(signalValue, 0, sizeof(cqWCHAR)* MSG_MAX_SIZE);

	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_START_SUCCESS);
	cqCHAR signalValueBufferStartSuc[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBufferStartSuc, MSG_MAX_SIZE);
	//最终构造形式是："10000001"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBufferStartSuc);
}

void Func_Navi_Stop_Suc(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_STOP_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000002"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_GPS_Rcv_Suc(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GPS_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000003"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Current_Voice_State(cqWCHAR* signalValue, void* data)
{
	BOOL state = FALSE;
	if (NULL != data)
	{
		state = *((BOOL*)data);
	}

	if (state)
	{
		cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GUIDEVOICE_STATE_ON);
	}
	else
	{
		cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GUIDEVOICE_STATE_OFF);
	}

	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000005"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Voice_Start(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GUIDEVOICE_START_REQ);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000004"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Voice_Stop(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GUIDEVOICE_STOP_REQ);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000005"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Play_Beep_Sound(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_BEEP);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000008"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Show_Other_Form(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SHOW);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000009"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Hide_Other_Form(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_HIDE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000A"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Show_Navi_Form_Suc(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SHOW_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000B"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);

	// 回调通知消息发送成功
	//ReceiveData* LocalparamData = NULL;
	ReceiveData* LocalparamData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == LocalparamData)
	{
		return;
	}
	memset(LocalparamData, 0, sizeof(ReceiveData));
	LocalparamData->funcId = FuncID_Receive_AnswerSendShowNaviFormSuc;
	// 延迟100ms
	Mapbar_sleep(100);
	NaviSysIPCWrapperToyota::getInstance()->getListener()->messageListener(LocalparamData);
	//mpListener->messageListener(LocalparamData);
}

void Func_Hide_Navi_Form_Suc(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_HIDE_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000C"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}
void Func_Caution_Show(cqWCHAR* signalValue, void* data){}

void Func_Req_Check_ILL_State(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ILL_REQ);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000D"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Rep_ILL_State_Changed(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ILL_REP);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000E"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Navi_ACCOFF_Suc(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ACC_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000E"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);

	// 回调通知消息发送成功
	//ReceiveData* paramData = NULL;
	ReceiveData* revData = (ReceiveData*)malloc(sizeof(ReceiveData));
	if (NULL == revData)
	{
		return;
	}
	memset(revData, 0, sizeof(ReceiveData));
	revData->funcId = FuncID_Receive_AnswerSendAccOffSucess;
	NaviSysIPCWrapperToyota::getInstance()->getListener()->messageListener(revData);
}

void Func_Current_GPS_TimeValue(cqWCHAR* signalValue, void* data)
{
	UnixTimestamp timeValue = 0;
	if (NULL != data)
	{
		timeValue = *((UnixTimestamp*)data);
	}
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GPS_TIME_VALUE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");

	cqWCHAR strTimeValue[16];
	cq_swprintf(strTimeValue, L"%x", timeValue);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, strTimeValue);

	cqCHAR signalValueBuffer[256];
	cq_encodeUtf8((const cqWCHAR*)signalValue, SIZE_T_MAX, (cqCHAR*)signalValueBuffer, 256);
}

void Func_Current_GPS_TimeValue_Fail(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GPS_TIME_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："10000011"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Update_TBTInfo(cqWCHAR* signalValue, void* data)
{
	TBTInfo tbtInfo;
	memset(&tbtInfo, 0, sizeof(TBTInfo));
	if (NULL != data)
	{
		memcpy(&tbtInfo, data, sizeof(TBTInfo));
	}
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_TBTINFO);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	cqCHAR signalValueBuffer[512];
	/********************************************************************/
	cqCHAR tempSignalValue[512];
	/********************************************************************/
	cq_encodeUtf8((const cqWCHAR*)signalValue, SIZE_T_MAX, (cqCHAR*)signalValueBuffer, 512);

	//【转向ID】两个字节
	CQ_LOG_INFO("========================FuncID_Send_UpdateTBTInfo tbtInfo.turnIcon============================, %u\n", tbtInfo.turnIcon);
	int16 toyotaTurnIcon = ChangeturnIcon(tbtInfo.turnIcon);
	cqWCHAR tempValueTurnIcons[4];
	cq_swprintf(tempValueTurnIcons, L"%04x", toyotaTurnIcon);
	CQ_LOG_INFO("========================FuncID_Send_UpdateTBTInfo toyotaTurnIcon = %u\n", toyotaTurnIcon);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempValueTurnIcons);
	//CQ_LOG_INFO("========================FuncID_Send_UpdateTBTInfo signalValue = %s \n", signalValue);
	/********************************************************************/
	cqCHAR tempTurnIcons[3] = { 0 };
	cq_sprintf(tempTurnIcons, "%02", toyotaTurnIcon);
	cq_strcat_s(tempSignalValue, MSG_MAX_SIZE, tempTurnIcons);
	CQ_LOG_INFO("========================FuncID_Send_UpdateTBTInfo tempSignalValue = %s \n", tempSignalValue);
	/********************************************************************/
	//【转向剩余距离】四个字节
	cqWCHAR tempValueIconRemainDistance[8];
	cq_swprintf(tempValueIconRemainDistance, L"%08x", tbtInfo.turnIconRemainDistance);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempValueIconRemainDistance);
	//CQ_LOG_INFO("========================FuncID_Send_UpdateTBTInfo turnIconRemainDistance, tbtInfo.turnIconRemainDistance = %s  ,tempValueIconRemainDistance = %x \n", tbtInfo.turnIconRemainDistance, tempValueIconRemainDistance);
	//【总剩余距离】四个字节
	uint32 totalRemainDistance = uint32(tbtInfo.routeLength - tbtInfo.travelledDistance);
	cqWCHAR tempValueTotalRemainDistance[8];
	cq_swprintf(tempValueTotalRemainDistance, L"%08x", totalRemainDistance);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempValueTotalRemainDistance);

	//【剩余时间】两个字节,单位：时：分
	uint32 remainingTime = 0;
	if (tbtInfo.remainingTime != 0)
	{
		remainingTime = cq_max(tbtInfo.remainingTime + 30, 60);
	}
	else
	{
		remainingTime = tbtInfo.remainingTime;
	}
	uint32 hour = remainingTime / 3600;
	uint32 min = (remainingTime % 3600) / 60;

	cqWCHAR tempValueRemainingTimeHour[2];
	cq_swprintf(tempValueRemainingTimeHour, L"%02x", hour);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempValueRemainingTimeHour);
	cqWCHAR tempValueRemainingTimeMin[2];
	cq_swprintf(tempValueRemainingTimeMin, L"%02x", min);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempValueRemainingTimeMin);


	int32 len2 = cq_encodeUtf8((const cqWCHAR*)signalValue, SIZE_T_MAX, (cqCHAR*)signalValueBuffer, 512);
	CQ_LOG_INFO("========================FuncID_Send_UpdateTBTInfo signalValueBuffer length = %d  signalValueBuffer = %s \n", len2, signalValueBuffer);
	//最终构造形式是："1000001C:"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Update_RoadName(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SELFPOSINFO);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");

	cqWCHAR* roadName = (cqWCHAR*)data;
	if (roadName == NULL)
	{
		return;
	}

	CQ_LOG_INFO("[ServiceManager]:Got current road name is %s.\n", roadName);

	//取得数据长度
	size_t roadNameLength = cq_encodeUtf8((const cqWCHAR*)roadName, cq_wcslen(roadName), NULL, 0);
	CQ_LOG_INFO("[ServiceManager]:Current road name size is %u.\n", roadNameLength);
	// 要求：数据长度不能超过64字节
	if (roadNameLength > 64)
	{
		CQ_LOG_ERROR("[ServiceManager]:The size of road name beyond 64 bytes.\n");

		return;
	}

	cqWCHAR tempRoadNameLen[3] = { 0 };
	cq_swprintf(tempRoadNameLen, L"%02x", roadNameLength);

	// signalValue like "1b:0a"
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempRoadNameLen);

	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, MSG_MAX_SIZE, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeof(cqCHAR)* (sizeCount + roadNameLength) + 1);
	if (NULL == convertSignalValue)
	{
		return;
	}
	memset(convertSignalValue, 0x00, sizeof(cqCHAR)* (sizeCount + roadNameLength) + 1);

	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);

	cqCHAR* convertRoadName = (cqCHAR*)malloc(sizeof(cqCHAR)* roadNameLength);
	if (NULL == convertRoadName)
	{
		free(convertSignalValue);
		convertSignalValue = NULL;
		return;
	}
	cq_encodeUtf8((cqWCHAR*)roadName, cq_wcslen(roadName) + 1, (cqCHAR*)convertRoadName, roadNameLength);
	for (size_t i = 0; i < roadNameLength; i++)
	{
		int byteValue = convertRoadName[i] < 0 ? convertRoadName[i] + 256 : convertRoadName[i];
		cqCHAR tempRoadName[3] = { 0 };
		cq_sprintf(tempRoadName, "%02x", byteValue);
		cq_strcat(convertSignalValue, tempRoadName);
	}
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertRoadName);
	convertRoadName = NULL;

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Start_Guidance(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_NAVISTATUS:IN NAVIGATION.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_NAVISTATUS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L"02");
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Stop_Guidance(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_NAVISTATUS:NOT IN NAVIGATION.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_NAVISTATUS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L"01");
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Update_Next_RoadName(cqWCHAR* signalValue, void* data){}

void Func_Addr_Status(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ADDRSTATUS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":0");

	cqWCHAR strArr[2] = { 0 };
	if (NULL != data)
		cq_uitow(*(uint32*)data, strArr, 10);

	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_ADDRSTATUS.\n", __FUNCTION__, __LINE__);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, strArr);
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Get_UUID(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_UUID_GET.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_UUID_GET);
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Set_Guidance(cqWCHAR* signalValue, void* data){}

void Func_BgSwt(cqWCHAR* signalValue, void* data){
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_NAVI_BACKGROUND);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeof(cqCHAR)* MSG_MAX_SIZE);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeof(cqCHAR)* MSG_MAX_SIZE);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Notify_NaviLive_Status(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_LIVE.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_LIVE);
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_History_Clear(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_MEMORY_CLEAR_SUCCESS.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_MEMORY_CLEAR_SUCCESS);
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_RunStatus_Request(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_RUNSTS_REQ.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_RUNSTS_REQ);
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);
	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Req_Usb_Status(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_REQUEST_USB);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000026"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Language_Switch_Success(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_LANGUAGE_SWITCH_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000003B"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Language_Switch_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_LANGUAGE_SWITCH_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000003C"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Get_Network_Status(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_CONNECT_TYPE_REQUEST);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000049"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Get_Gbook_Info(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_GBOOK_INFO_REQUEST);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager]:G-BOOKInfo send message is %s.\n", signalValueBuffer);
	//最终构造形式是："0x1000004B"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Set_Volume(cqWCHAR* signalValue, void* data)
{
	if (NULL == data)
	{
		CQ_LOG_ERROR("[ServiceManager]Volume is NULL!\n");
		return;
	}

	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_VOLUM.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_VOLUM);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":0");
	CQ_LOG_INFO("[ServiceManager]:Volume message here.\n");
	//0 - 10级转换
	uint32 volume = *((uint32*)data);
	uint32 volumeValue = volume / 10;
	CQ_LOG_INFO("[ServiceManager]:Volume is %u.\n", volumeValue);

	cqWCHAR volumeStr[2] = { 0 };
	cq_swprintf(volumeStr, L"%x", volumeValue);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, volumeStr);

	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Get_Volume(cqWCHAR* signalValue, void* data){}	// null
void Func_Get_Back_Light(cqWCHAR* signalValue, void* data){}	// null
void Func_Back_ToHost(cqWCHAR* signalValue, void* data){}		// null
void Func_Show_Host_Menu(cqWCHAR* signalValue, void* data){}	// null
void Func_Request_Audio_Guidance_Play(cqWCHAR* signalValue, void* data){}	// null
void Func_Release_Audio_Guidance_Play(cqWCHAR* signalValue, void* data){}	// null

void Func_Get_LanguageID(cqWCHAR* signalValue, void* data){
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_LANGUAGE_REQ);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000053"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}
void Func_Is_GPS_Update_Time_Enabled(cqWCHAR* signalValue, void* data){}	// null
void Func_Set_Route_Status(cqWCHAR* signalValue, void* data){}				// null
void Func_Is_CarPlay_NaviActive(cqWCHAR* signalValue, void* data){}			// null

void Func_Routing_Status_Routing(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_ROUTINGSTATUS:ROUTING.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ROUTINGSTATUS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":01");
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Routing_Status_Failed(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_ROUTINGSTATUS:FAILED.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ROUTINGSTATUS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":02");
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Routing_Status_Complete(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_ROUTINGSTATUS:COMPLETE.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ROUTINGSTATUS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":03");
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, NULL, 0);
	cqCHAR* convertSignalValue = (cqCHAR*)malloc(sizeCount);
	if (NULL == convertSignalValue)
	{
		return;
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	free(convertSignalValue);
	convertSignalValue = NULL;
}

void Func_Req_Show_Music_Widget(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_MUSICWIDGET_REQ);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000E"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Hide_Music_Widget(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_MUSICWIDGET_HIDE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："1000000E"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Req_Sys_Hmi_Time(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_REQUEST_DATETIME_DVALUE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Ready_For_Forgroud(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_READY_FOR_FORGROUD);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_READY_FOR_FORGROUD signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x1000004D"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Get_DayNight_Mode(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_DAYNIGHT_MODE_REQUEST);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000004e"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Set_DayNight_Mode(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x:%d", DBUS_NAVI_SIG_DAYNIGHT_MODE_CHANGE, *((int*)data));
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000004c"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Command_Control_Sucess(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_COMMAND_CONRTOL_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000032"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Command_Control_Failure(cqWCHAR* signalValue, void* data)
{
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	VR_COMMAND_CONTROL_FAILED_TYPE type = *(VR_COMMAND_CONTROL_FAILED_TYPE*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_COMMAND_CONRTOL_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	if (NULL != data)
	{
		cqWCHAR failedType[128];
		cq_swprintf(failedType, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Command_Traffic_Sucess(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_TRAFFIC_CONTROL_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000034"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Command_Traffic_Failure(cqWCHAR* signalValue, void* data)
{
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	VR_TRAFFIC_COMMAND_FAILED_TYPE type = *(VR_TRAFFIC_COMMAND_FAILED_TYPE*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_TRAFFIC_CONTROL_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	if (NULL != data)
	{
		cqWCHAR failedType[128];
		cq_swprintf(failedType, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Passing_Point_Sucess(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_PassingPointSucess \n");
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_PASSING_POINT_SUCCESS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	VRPassPointRepInfo repInfo;
	memset(&repInfo, 0, sizeof(VRPassPointRepInfo));
	if (NULL != data)
	{
		memcpy(&repInfo, data, sizeof(VRPassPointRepInfo));
		if (repInfo.repType == 0)
		{
			// 删除所有途径点成功 
			cqWCHAR repTypeStr[32];
			cq_swprintf(repTypeStr, L"%x", repInfo.repType);
			cq_wcscat_s(signalValue, MSG_MAX_SIZE, repTypeStr);
		}
		else if (repInfo.repType == 1)
		{
			// 途经点个数查询成功 
			cqWCHAR repTypeStr[32];
			cq_swprintf(repTypeStr, L"%x", repInfo.repType);
			cq_wcscat_s(signalValue, MSG_MAX_SIZE, repTypeStr);
			cq_wcscat_s(signalValue, MSG_MAX_SIZE, L",");

			cqWCHAR repTypeNumStr[32];
			cq_swprintf(repTypeNumStr, L"%x", repInfo.passPointNum);
			cq_wcscat_s(signalValue, MSG_MAX_SIZE, repTypeNumStr);
		}
	}
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_PassingPointSucess signalValue = %s \n", signalValue);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000030"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Passing_Point_Failure(cqWCHAR* signalValue, void* data)
{
	VRPassPointRepInfo repInfo;
	memset(&repInfo, 0, sizeof(VRPassPointRepInfo));
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_PASSING_POINT_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	if (NULL != data)
	{
		memcpy(&repInfo, data, sizeof(VRPassPointRepInfo));
		cqWCHAR repTypeStr[32];
		cq_swprintf(repTypeStr, L"%x", repInfo.repType);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, repTypeStr);

		CQ_LOG_INFO("[ServiceManager] FuncID_Send_PassingPointFailure signalValue = %s \n", signalValue);
		cqCHAR signalValueBuffer[MSG_MAX_SIZE];
		cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
		//最终构造形式是："0x10000031"
		ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
	}
}

void Func_Switch_Route_Sucess(cqWCHAR* signalValue, void* data)
{
	VR_ROUTE_PLAN type = *(VR_ROUTE_PLAN*)data;
	printf("[serviceManager] ipcwrapper FuncID_Send_SwitchRouteSucess");
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SWITCH_ROUTE_SUCCESS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");

	if (NULL != data)
	{
		cqWCHAR hasRoutePlan[MSG_MAX_SIZE];
		cq_swprintf(hasRoutePlan, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, hasRoutePlan);
	}

	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000003e"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Switch_Route_Failure(cqWCHAR* signalValue, void* data)
{
	printf("[serviceManager] ipcwrapper FuncID_Send_SwitchRouteFailure");
	VR_ROUTE_PLAN type = *(VR_ROUTE_PLAN*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SWITCH_ROUTE_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");

	if (NULL != data)
	{
		cqWCHAR hasRoutePlan[MSG_MAX_SIZE];
		cq_swprintf(hasRoutePlan, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, hasRoutePlan);
	}

	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000003f"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Item_Select_Sucess(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ITEM_SELECT_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000003A"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Item_Select_Failure(cqWCHAR* signalValue, void* data)
{
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	VR_ITEM_SELECT type = *(VR_ITEM_SELECT*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ITEM_SELECT_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	if (NULL != data)
	{
		cqWCHAR failedType[128];
		cq_swprintf(failedType, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000003B"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Inquiry_Distance_Success(cqWCHAR* signalValue, void* data){
	if (NULL == data)
	{
		CQ_LOG_ERROR("[ServiceManager]FuncID_Send_InquiryDistanceSuccess data is null\n");
		return;
	}
	//距离
	InquiryDistance* inquiry = (InquiryDistance*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_RESPONSE_DISTANCE_SUCCESS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	uint32 remainingTime = 0;
	remainingTime = inquiry->remainingTime;
	cqWCHAR RemainingTime[512];
	cq_swprintf(RemainingTime, L"%x", remainingTime);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, RemainingTime);
	//时间
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L",");
	int32 remainDistance = inquiry->remainingDistance;
	cqWCHAR TotalRemainDistance[512];
	cq_swprintf(TotalRemainDistance, L"%x", remainDistance);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, TotalRemainDistance);

	cqCHAR signalValueBuffer[512];
	cq_encodeUtf8((const cqWCHAR*)signalValue, SIZE_T_MAX, (cqCHAR*)signalValueBuffer, 512);
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_InquiryDistanceSuccess signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："10000036:"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Inquiry_Distance_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_RESPONSE_DISTANCE_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000037"
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_RESPONSE_DISTANCE_FAILURE signalValueBuffer is %s\n", signalValueBuffer);
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Nearby_Search_Success(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SERRCH_NEARBY_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000002e"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Nearby_Search_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x:1", DBUS_NAVI_SIG_SERRCH_NEARBY_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000002f"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_KeyWoard_Search_Success(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_UDE_SEARCH_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000002c"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_KeyWoard_Search_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x:1", DBUS_NAVI_SIG_UDE_SEARCH_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x1000002d"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Passing_PoiAndDestPoi_Success(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SEARCH_DEST_VIA_PASSING_POI_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_SEARCH_DEST_VIA_PASSING_POI_SUCCESS signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x1000003A"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Passing_PoiAndDestPoi_Failure(cqWCHAR* signalValue, void* data)
{
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	VR_PASSINGPOI_FAILED_TYPE type = *(VR_PASSINGPOI_FAILED_TYPE*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SEARCH_DEST_VIA_PASSING_POI_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	if (NULL != data)
	{
		cqWCHAR failedType[128];
		cq_swprintf(failedType, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000048"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Collection_Success(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_COLLECTION_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_COLLECTION_SUCCESS signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000038"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Collection_Failure(cqWCHAR* signalValue, void* data)
{
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	VR_COLLECTION_FAILED_TYPE type = *(VR_COLLECTION_FAILED_TYPE*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_COLLECTION_FAILURE);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	if (NULL != data)
	{
		cqWCHAR failedType[128];
		cq_swprintf(failedType, L"%x", type);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, failedType);
	}
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	//最终构造形式是："0x10000039"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Start_Navi_Success(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_NAVI_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_NAVI_SUCCESS signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x1000002A"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Start_Navi_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_NAVI_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_NAVI_FAILURE signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x1000002A"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}
// METER FUNC ID
void Func_Meter_Direction_Info(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_MeterDirectionInfo \n");
	DirectionInfo dirInfo;
	memset(&dirInfo, 0, sizeof(DirectionInfo));
	if (NULL != data)
	{
		memcpy(&dirInfo, data, sizeof(DirectionInfo));
	}

	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_HEAD_AUTOMOVE_DES_DIRECTION);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");

	cqWCHAR mapDirStr[MSG_MAX_SIZE];
	cq_swprintf(mapDirStr, L"%x", dirInfo.mapDirection);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, mapDirStr);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L",");

	cqWCHAR carDirStr[MSG_MAX_SIZE];
	cq_swprintf(carDirStr, L"%02x", dirInfo.carDirection);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, carDirStr);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L",");

	cqWCHAR destDirStr[MSG_MAX_SIZE];
	cq_swprintf(destDirStr, L"%x", dirInfo.destDirection);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, destDirStr);

	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);

	//最终构造形式是："0x10000037"
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_HEAD_AUTOMOVE_DES_DIRECTION signalValueBuffer is %s\n", signalValueBuffer);
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Meter_Next_TBTInfo(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_MeterNextTBTInfo \n");
	NextTBTInfo nextTbtInfo;
	memset(&nextTbtInfo, 0, sizeof(NextTBTInfo));
	if (NULL != data)
	{
		memcpy(&nextTbtInfo, data, sizeof(NextTBTInfo));
	}
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_NEXT_STREET_DIRECTION);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");
	// 参数1 下一条路名
	CQ_LOG_INFO("[ServiceManager]  nextRoadName = %s \n", nextTbtInfo.nextRoadName);
	cqWCHAR* roadName = (cqWCHAR*)nextTbtInfo.nextRoadName;
	if (roadName == NULL)
	{
		return;
	}
	size_t roadNameLength = cq_encodeUtf8((const cqWCHAR*)roadName, cq_wcslen(roadName), NULL, 0);
	size_t sizeCount = cq_encodeUtf8((cqWCHAR*)signalValue, MSG_MAX_SIZE, NULL, 0);
	cqCHAR convertSignalValue[512] = { 0 };

	cq_encodeUtf8((const cqWCHAR*)signalValue, cq_wcslen(signalValue) + 1, (cqCHAR*)convertSignalValue, sizeCount);
	cqCHAR* convertRoadName = (cqCHAR*)malloc(sizeof(cqCHAR)* roadNameLength);
	if (NULL == convertRoadName)
	{
		return;
	}
	cq_encodeUtf8((cqWCHAR*)roadName, cq_wcslen(roadName) + 1, (cqCHAR*)convertRoadName, roadNameLength);

	for (size_t i = 0; i < roadNameLength; i++)
	{
		// this is to prevent negative number.
		int byteValue = convertRoadName[i] < 0 ? convertRoadName[i] + 256 : convertRoadName[i];
		//int byteValue = convertRoadName[i] + 256;
		cqCHAR tempRoadName[3] = { 0 };
		cq_sprintf(tempRoadName, "%02x", byteValue);
		cq_strcat(convertSignalValue, tempRoadName);
	}

	cqWCHAR* tempSignalValue = (cqWCHAR*)malloc(sizeof(cqWCHAR)* MSG_MAX_SIZE);
	if (NULL == tempSignalValue)
	{
		free(convertRoadName);
		convertRoadName = NULL;
		return;
	}
	memset(tempSignalValue, 0, sizeof(cqWCHAR)* MSG_MAX_SIZE);

	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数2 距离 
	cqWCHAR distance[MSG_MAX_SIZE];
	cq_swprintf(distance, L"%x", nextTbtInfo.distance);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, distance);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数3 单位m
	int32 uint = 0x000b;
	cqWCHAR uintChar[MSG_MAX_SIZE];
	cq_swprintf(uintChar, L"%04x", uint);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, uintChar);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数4 道路信息
	cqWCHAR routeInfo[MSG_MAX_SIZE];
	cq_swprintf(routeInfo, L"%08x", nextTbtInfo.routeBackInfo);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, routeInfo);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数5 有无环岛
	cqWCHAR isOnRotary[MSG_MAX_SIZE];
	cq_swprintf(isOnRotary, L"%x", nextTbtInfo.isOnRotary);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, isOnRotary);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数6 环岛内外向
	cqWCHAR isOutside[MSG_MAX_SIZE];
	cq_swprintf(isOutside, L"%x", nextTbtInfo.isOutSide);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, isOutside);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数7 前方方向
	cqWCHAR outLine[MSG_MAX_SIZE];
	cq_swprintf(outLine, L"%08x", nextTbtInfo.actualOutLine);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, outLine);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数8 特殊转向标
	cqWCHAR specialIcon[MSG_MAX_SIZE];
	cq_swprintf(specialIcon, L"%x", nextTbtInfo.specialIcon);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, specialIcon);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数9 到终点的距离
	cqWCHAR remainingDistance[MSG_MAX_SIZE];
	cq_swprintf(remainingDistance, L"%x", nextTbtInfo.remainingDistance);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, remainingDistance);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, L",");

	// 参数10 到终点的时间
	cqWCHAR remainingTime[MSG_MAX_SIZE];
	cq_swprintf(remainingTime, L"%x", nextTbtInfo.remainingTime);
	cq_wcscat_s(tempSignalValue, MSG_MAX_SIZE, remainingTime);

	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)tempSignalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	cq_strcat(convertSignalValue, signalValueBuffer);

	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_NEXT_STREET_DIRECTION convertSignalValue is %s\n", convertSignalValue);
	ToyotaDbusService::getInstance()->emitSignal(convertSignalValue);

	// 释放申请的临时内存
	free(tempSignalValue);
	tempSignalValue = NULL;

	free(convertRoadName);
	convertRoadName = NULL;
}

void Func_Get_Device_Connect(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_DEVICE_CONNECT_INFO_REQ);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_DEVICE_CONNECT_INFO_REQ signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000052"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Entry_Phone_Connect(cqWCHAR* signalValue, void* data){
	printf("FuncID_Send_EntryPhoneConnect");
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_ENTRY_PHONE_CONNECT);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_START_PHONE_CONNECT signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x1000004F"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Exit_Phone_Connect(cqWCHAR* signalValue, void* data)
{
	printf("FuncID_Send_ExitPhoneConnect");
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_EXIT_PHONE_CONNECT);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_START_PHONE_CONNECT signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000050"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Phone_Connect_Status(cqWCHAR* signalValue, void* data)
{
	CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBUS_NAVI_SIG_PHONE_CONNECT_STATUS_NTY.\n", __FUNCTION__, __LINE__);
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_PHONE_CONNECT_STATUS_NTY);
	EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS status = *(EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS*)data;

	if (status == EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_CONNECTING)
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":01");
	if (status == EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_CONNECTED)
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":02");
	if (status == EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_CONNECTERR)
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":03");
	if (status == EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_DISCONNECT)
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":04");

	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_START_PHONE_CONNECT signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000051:"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_SilentMode_Change_Sucess(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SILENTMODE_RES_SUCCESS);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_SILENTMODE_RES_SUCCESS signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000054"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_SilentMode_Change_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_SILENTMODE_RES_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] DBUS_NAVI_SIG_SILENTMODE_RES_FAILURE signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000055"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_UuidError(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_UUID_ERROR);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_UuidError signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000056"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Get_Version_Success(cqWCHAR* signalValue, void* data)
{
	if (NULL == data)
	{
		CQ_LOG_ERROR("[ServiceManager]FuncID_Send_GetVersionSuccess data is null\n");
		return;
	}
	//软件版本
	MapbarVersion* version = (MapbarVersion*)data;
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_VERSION_SUCCESS);
	cq_wcscat_s(signalValue, MSG_MAX_SIZE, L":");

	cqWCHAR* naviVersionTemp = NULL;
	naviVersionTemp = version->naviVersion;
	cqWCHAR naviVersionArray[512];
	cq_wcscpy(naviVersionArray, naviVersionTemp);
	size_t naviVersionLength = cq_encodeUtf8((const cqWCHAR*)naviVersionTemp, cq_wcslen(naviVersionTemp), NULL, 0);

	for (size_t i = 0; i < naviVersionLength; i++)
	{
		// this is to prevent negative number.
		int byteValue = naviVersionArray[i] < 0 ? naviVersionArray[i] + 256 : naviVersionArray[i];
		//int byteValue = convertRoadName[i] + 256;
		cqWCHAR tempnaviVersion[3] = { 0 };
		cq_swprintf(tempnaviVersion, L"%02x", byteValue);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempnaviVersion);
	}

	cq_wcscat_s(signalValue, 256, L",");

	//地图版本
	cqWCHAR* mapVersionTemp = NULL;
	mapVersionTemp = version->mapVersion;
	cqWCHAR mapVersionArray[512];
	cq_wcscpy(mapVersionArray, mapVersionTemp);
	size_t mapVersionLength = cq_encodeUtf8((const cqWCHAR*)mapVersionTemp, cq_wcslen(mapVersionTemp), NULL, 0);

	for (size_t i = 0; i < mapVersionLength; i++)
	{
		// this is to prevent negative number.
		int byteValue = mapVersionArray[i] < 0 ? mapVersionArray[i] + 256 : mapVersionArray[i];
		//int byteValue = convertRoadName[i] + 256;
		cqWCHAR tempnaviVersion[3] = { 0 };
		cq_swprintf(tempnaviVersion, L"%02x", byteValue);
		cq_wcscat_s(signalValue, MSG_MAX_SIZE, tempnaviVersion);
	}

	cqCHAR signalValueBuffer[512];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, 512);
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_InquiryDistanceSuccess signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："10000057:软件版本,地图版本"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);

	if (version->naviVersion != NULL)
	{
		free(version->naviVersion);
		version->naviVersion = NULL;
	}

	if (version->mapVersion != NULL)
	{
		free(version->mapVersion);
		version->mapVersion = NULL;
	}
}

void Func_Get_Version_Failure(cqWCHAR* signalValue, void* data)
{
	cq_swprintf(signalValue, L"%x", DBUS_NAVI_SIG_VERSION_FAILURE);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];
	cq_encodeUtf8((const cqWCHAR*)signalValue, MSG_MAX_SIZE, (cqCHAR*)signalValueBuffer, MSG_MAX_SIZE);
	CQ_LOG_INFO("[ServiceManager] FuncID_Send_UuidError signalValueBuffer is %s\n", signalValueBuffer);
	//最终构造形式是："0x10000058"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Add_Current_To_Fav_Resp(cqWCHAR* signalValue, void* data)
{
	UNUSED_VAR(signalValue);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];

	cq_sprintf(signalValueBuffer, "%x,%s", DBUS_NAVI_SIG_VR_SAVE_CUR_TO_FAV_RSP,(cqCHAR *) data)
	
	//最终构造形式是："0x10000057"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Add_Dest_To_Fav_Resp(cqWCHAR* signalValue, void* data)
{
	UNUSED_VAR(signalValue);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];

	cq_sprintf(signalValueBuffer, "%x,%s", DBUS_NAVI_SIG_VR_SAVE_DEST_TO_FAV_RSP,(cqCHAR *) data)
	
	//最终构造形式是："0x10000058"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Add_Poi_To_Fav_Resp(cqWCHAR* signalValue, void* data)
{
	UNUSED_VAR(signalValue);
	cqCHAR signalValueBuffer[MSG_MAX_SIZE];

	cq_sprintf(signalValueBuffer, "%x,%s", DBUS_NAVI_SIG_VR_SAVE_POI_TO_FAV_RSP,(cqCHAR *) data)
	
	//最终构造形式是："0x10000057"
	ToyotaDbusService::getInstance()->emitSignal(signalValueBuffer);
}

void Func_Is_Navi_Have_Des_Resp(cqWCHAR* signalValue, void* data)
{
	cq_sprintf(((cqCHAR*)signalValue,"%x,%d",DBUS_NAVI_SIG_VR_VR_GET_DEST_INFO_MAIN_RSP,*((int32_t*)data);
	ToyotaDbusService::getInstance()->emitSignal((cqCHAR*)signalValue);
}