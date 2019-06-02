#include "stdafx_serviceManager.h"
#include "serviceManagerCommunicationIpcWrapperDeSay.h"

//#include "appfw/ISVPAppFw.h"
//#include "appfw/fw/ISVPAppFw_EventID.h"
//#include "appfw/gui/ISVPAppFw_GUI_WS.h"
//
//#include "SVPSingleton.h"
//#include "SVPApp.h"
//#include "device/input/SVPInputType.h"
//
//#include "SVPAudio.h"
//
//#include "SVPLog.h"
//#include "SVPLocation.h"
//#include "SVPExtNavi.h"
//#include "SVPLocation.h"
//
//#include "source/SVPSourceType.h"
//#include "mapdal/mapbar_dal_pub.h"

//=====================================================//
using namespace ServiceManager;
//using namespace SVPPower;

NaviSysIPCWrapperDeSay* NaviSysIPCWrapperDeSay::mpInstance = NULL;
void DataReceiveCallBack(uint32 evt_id, int argc, void *argv[], int *args, int fromID);
/**
*	@brief
*		NaviSysIPCWrapper����
*/
NaviSysIPCWrapperDeSay::NaviSysIPCWrapperDeSay()
{
	m_mutex = Mapbar_createMutex();
	m_shouldExit = FALSE;
	m_threadHandle = invalidHandle;
	m_pExtNavi = GetExtNavi();
	//m_naviGuidance = GetExtNaviGuidance();

	if (NULL != m_pExtNavi)
	{
		//m_pExtNavi->AddListener(this);
		m_pExtNavi->SetCallback(DataReceiveCallBack);
	}
	mpInstance = this;
	startIpcWrappereProcess();
}

/**
*	@brief
*		NaviSysIPCWrapper����
*/
NaviSysIPCWrapperDeSay::~NaviSysIPCWrapperDeSay()
{
	//DestroyExtNavi();
	//DestroyExtNaviGuidance();
	Mapbar_destroyMutex(m_mutex);

	if (NULL != m_listerner)
	{
		free(m_listerner);
		m_listerner = NULL;
	}

	if (m_threadHandle != invalidHandle)
	{
		m_shouldExit = TRUE;
		Mapbar_waitThread(m_threadHandle);
		Mapbar_closeThread(m_threadHandle);
		m_threadHandle = invalidHandle;
	}
	mpInstance = NULL;
	delete(mpInstance);
}

void DataReceiveCallBack(uint32 evt_id, int argc, void *argv[], int *args, int fromID)
{
	NaviSysIPCWrapperDeSay* ins = NaviSysIPCWrapperDeSay::getInstance();
	if (NULL == ins)
	{
		return;
	}

	switch (evt_id)
	{
	case SVExt::MSG_HARDKEY_RELEASE:
		break;
	case SVExt::MSG_NAVVOL_CHANGE:
		// VOLUME����
		ins->updataVolume();
		break;
	case SVExt::MSG_BACKLIGHT_CHANGE:
		// �������
		ins->updataBacklightMode();
		break;
	case SVExt::MSG_WIFICONNECT_CHANGE:
		// WIFI����״̬���
		ins->updateWifiStatus();
		break;
	default:
		break;
	}
	return;
}

NaviSysIPCWrapperDeSay* NaviSysIPCWrapperDeSay::getInstance()
{
	if (NULL != mpInstance)
	{
		return mpInstance;
	}
	return NULL;
}

/**
*	@brief
*		�ṩ��Receiver������Ϣ�ӿ�
*	@return
*		bool true���ɹ� false��ʧ��
*	@param pdata
*		data�����͵�msg priority�����ȼ�
*/
bool NaviSysIPCWrapperDeSay::sendMsg(void *data, int priority)
{
	UNUSED_VAR(priority);

	Mapbar_lockMutex(m_mutex);

	m_SendList.push_back((SendData*)data);

	Mapbar_unlockMutex(m_mutex);
}

/**
*	@brief
*		��ȡ��Ϣ����msg
*/
SendData* NaviSysIPCWrapperDeSay::naviServiceGetSend()
{
	Mapbar_lockMutex(m_mutex);

	SendData* msg = m_SendList.front();
	m_SendList.pop_front();

	Mapbar_unlockMutex(m_mutex);

	return msg;
}

/**
*	@brief
*		�򳵻�ϵͳ�����߳�
*/
int IpcWrapperMessageProcess(void* para)
{
	NaviSysIPCWrapperDeSay* ipcWrapper = (NaviSysIPCWrapperDeSay*)para;
	while (!ipcWrapper->m_shouldExit)
	{
		if (!ipcWrapper->ipcWrapperHasRequest())
		{
			Mapbar_sleep(100);
			continue;
		}

		SendData* request = ipcWrapper->naviServiceGetSend();

		//call IPC Function
		ipcWrapper->ipcWrapperProcess(request);
	};
	return D_NAVISYS_OK;
}

/**
*	@brief
*		�ж���Ϣ�����Ƿ���msg
*	@return
*		bool true���� false����
*/
bool NaviSysIPCWrapperDeSay::ipcWrapperHasRequest()
{
	Mapbar_lockMutex(m_mutex);

	int size = m_SendList.size();

	Mapbar_unlockMutex(m_mutex);

	return (size != 0);
}

/**
*	@brief
*		����IpcWrapper��Ϣ�����߳�
*	@return
*		void
*/
void NaviSysIPCWrapperDeSay::startIpcWrappereProcess()
{
	m_threadHandle = Mapbar_createThread(IpcWrapperMessageProcess, this);
	return;
}

/**
*	@brief
*		IpcWrapper��Ϣ������
*	@return
*		void
*/
void NaviSysIPCWrapperDeSay::ipcWrapperProcess(SendData *param)
{
	if (param == NULL)
	{
		return;
	}

	SendData* paramData = (SendData *)param;
	switch (paramData->funcId)
	{
		case ServiceManager::FuncID_Send_getUUID:
		{
			getMachineID();
		}
		break;
		case FuncID_Send_GetVolume:
		{
			getVolume();
		}
		break;

		case FuncID_Send_SetVolume:
		{
			setVolume(paramData->data);
		}
		break;
		
		case FuncID_Send_GetBackLight:
		{
			getBacklightMode();
		}
		break;
		/*case FuncID_Send_BackToHost:
		{
			backToHost();
		}
			break;
		case FuncID_Send_ShowHostMenu:
		{
			showHostMenu();
		}
			break;
		case FuncID_Send_RequestAudioGuidancePlay:
		{
			requestAudioGuidancePlay();
		}
			break;
		case FuncID_Send_ReleaseAudioGuidancePlay:
		{
			releaseAudioGuidancePlay();
		}
			break;*/
		case FuncID_Send_GetLanguageID:
		{
			getLanguageID();
		}
			break;
		//case FuncID_Send_IsGPSUpdateTimeEnabled:
		//{
		//	isGPSUpdateTimeEnabled();
		//}
		//	break;
		case FuncID_Send_SetRouteStatus:
		{
			if (NULL != paramData->data)
				setRouteStatus((bool)paramData->data);
		}
			break;
		/*case FuncID_Send_IsCarPlayNaviActive:
		{
			isCarPlayNaviActive();
		}
			break;

		case FuncID_Send_SetGuidance:
		{
			if (NULL != paramData->data)
			{
				Guidance_DeSai* guidanceData = (Guidance_DeSai*)paramData->data;
				m_naviGuidance->SetGuidance(guidanceData->uiID, guidanceData->uiDistance);
			}								 
		}
			break;
		case FuncID_Send_UpdateNextRoadName:
		{
			if (NULL != paramData->data)
			{
				RoadName_DeSai* roadName = (RoadName_DeSai*)paramData->data;
				m_naviGuidance->NextRoadName(roadName->pUTF8RoadName, roadName->uiLen);
			}
		}
			break;
		case FuncID_Send_StartGuidance:
		{
			m_naviGuidance->StartGuidance();
		}
			break;
		case FuncID_Send_StopGuidance:
		{
			m_naviGuidance->StopGuidance();
		}
			break;*/
		case FuncID_Send_ReqPlayBeepSound:
		{
			if (NULL != paramData->data)
			{
				BEEP_TYPE beepType = *(BEEP_TYPE*)paramData->data;
				reqBeep(beepType);
			}
			
		}
			break;
		case FuncID_Send_SetVersion:
		{
			if (NULL != paramData->data)
			{
				setVersion(paramData->data);
			}
		}
			break;
		case FuncID_Send_getWifiStatus:
		{
			getWifiStatus();
		}

	default:
		break;
	}

	// free data
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
	return;
}

/**
*	@brief
*		Receiver��IPCע�����Listener
*/
void NaviSysIPCWrapperDeSay::addListener(NaviSysIPCWrapperListener *listerner)
{
	if (listerner != NULL)
		m_listerner = listerner;
	return;
}

/**
*	@brief
*		Receiver�����IPC��ע���Listener
*/
void NaviSysIPCWrapperDeSay::removeListener()
{
	if (m_listerner != NULL)
		m_listerner = NULL;
}

/**
*	@brief
*		Receiver��IPCע��callback
*	@return
*		pfun ReceiverCallback�ص�ָ��
void NaviSysIPCWrapperDeSai::setCallback(void *pfun)
{
	if (NULL != pfun){
		m_ReceiverCallback = (ReceiverCallback)pfun;
	}
	return;
}
*/

/**
*	@brief
*		��ȡ������
*/
void NaviSysIPCWrapperDeSay::getMachineID()
{
	string machineID = m_pExtNavi->GetMachineID();
	updataMachineID(machineID);
	return;
}

void NaviSysIPCWrapperDeSay::updataMachineID(string machineID)
{
	// �������
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	char *mp_machineID;
	int len = machineID.length();
	mp_machineID = (char *)malloc((len + 1)*sizeof(char));
	machineID.copy(mp_machineID, len, 0);
	*(mp_machineID + len) = '\0';
	paramData->data = mp_machineID;
	paramData->funcId = ServiceManager::FuncID_Receive_getUUID;
	// ִ�лص�
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mp_machineID);
	}
	return;
}
/************************************1.1 Volum start********************************************/
/**
*	@brief
*		��ȡ����
*/
void NaviSysIPCWrapperDeSay::getVolume()
{
	uint32 volume = m_pExtNavi->GetNavVol();
	updataVolume(volume);
	return;
}


/**
*	@brief
*		��������
*/
void NaviSysIPCWrapperDeSay::setVolume(void* data)
{
	uint32 volume = *(uint32*)data;
	m_pExtNavi->SetNavVol(volume);
	return;
}

/**
*	@brief
*		֪ͨNavi����Volume��Ϣ
*	@return
*		int vol ����ֵ
*/
void NaviSysIPCWrapperDeSay::updataVolume(int vol)
{
	// �������
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	uint32* mVolume = (uint32*)malloc(sizeof(uint32));
	*mVolume = vol;
	paramData->data = mVolume;
	paramData->funcId = ServiceManager::FuncID_Receive_AnswerVolume;
	// ִ�лص�
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mVolume);
	}
	return;
}

/**
*	@brief
*		���������仯֪ͨNavi����Volume��Ϣ
*	@return
*		int vol ����ֵ
*/
//SVPVoid NaviSysIPCWrapperDeSay::OnVolumeChanged(SVPUint8 uiVolume)
//{
//	updataVolume((int)uiVolume);
//	return;
//}

/************************************1.1 Volum end********************************************/

/************************************1.2 Backlight start**************************************/
/**
*	@brief
*		�򳵻���ȡ������Ϣ
*/
void NaviSysIPCWrapperDeSay::getBacklightMode()
{
	//SVPPower::BkLight::BkLightMode mode;
	//SVPPower::BkLight::BkLightType type;
	uint8 mode;
	uint8 type;
	uint8  level;

	m_pExtNavi->GetBkLightMode(&mode, &type, &level);
	updataBacklightMode(mode, type, level);
	return;
}

/**
*	@brief
*		֪ͨNavi���±�����Ϣ
*/
void NaviSysIPCWrapperDeSay::updataBacklightMode(
	uint8 mode,
	uint8 type,
	uint8 level)
{
	BL_DeSai *data = (BL_DeSai*)malloc(sizeof(BL_DeSai));
	data->mode = (BackLightMode)mode;
	data->type = (BackLightType)type;
	data->level = 0;

	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	paramData->data = data;
	paramData->funcId = ServiceManager::FuncID_Receive_BackLightChanged;
	// ִ�лص�
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(data);
	}
	return;
}

/**
*	@brief
*		��������仯������֪ͨNavi������Ϣ����
*	@return
*		int vol ����ֵ
*/
void NaviSysIPCWrapperDeSay::OnBacklightChanged(
	uint8 mode,
	uint8 type,
	uint8 level)
{
	BL_DeSai *data = (BL_DeSai*)malloc(sizeof(BL_DeSai));
	data->mode = (BackLightMode)mode;
	data->type = (BackLightType)type;
	data->level = level;

	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	paramData->data = data;
	paramData->funcId = ServiceManager::FuncID_Receive_BackLightChanged;
	// ִ�лص�
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(data);
	}
	return;
}
/************************************1.2 Backlight end****************************************/

/**
*   @brief
*       ���õ�������汾�ż���ͼ�汾�� (�汾���ַ�����<30�ֽ�)
*
*/
void NaviSysIPCWrapperDeSay::setVersion(void* data)
{
	if (NULL != data)
	{
		MapbarVersion* ver = (MapbarVersion*)(data);
		if (NULL != ver->naviVersion || NULL != ver->mapVersion)
		{
			char * naviVer = NULL;
			char * mapVer = NULL;
			int naviSize = wcstombs(naviVer, ver->naviVersion, 0) + 1;
			int mapSize = wcstombs(naviVer, ver->mapVersion, 0) + 1;
			naviVer = new char[naviSize];
			mapVer = new char[mapSize];
			memset(naviVer, 0, naviSize);
			memset(mapVer, 0, mapSize);

			int nRet = wcstombs(naviVer, ver->naviVersion, naviSize);
			int mRet = wcstombs(mapVer, ver->mapVersion, mapSize);
			if (nRet > 0 || mRet > 0)
			{
				printf("ת���ɹ�%d�ַ�\n", nRet);
				printf("%s\n", naviVer);
				m_pExtNavi->SetVersion(naviVer, mapVer);
			}
		}
	}

}

/************************************1.4 backToHost start*************************************/

/**
*	@brief
*		Resume to the menu befor entering navigation APP
*/
//SVPVoid NaviSysIPCWrapperDeSay::backToHost()
//{
//	m_pExtNavi->BackToHost();
//	return;
//}

/************************************1.4 backToHost end***************************************/

/************************************1.5 showHostMenu start***********************************/
/**
*	@brief
*		Show host menu.
*/
//SVPVoid NaviSysIPCWrapperDeSay::showHostMenu()
//{
//	m_pExtNavi->ShowHostMenu();
//	return;
//}
/************************************1.5 showHostMenu end*************************************/

/************************************1.6 AudioGuidance start**********************************/

/**
*	@brief
*		����Guidance�����Ƿ���Բ���
*/
//void NaviSysIPCWrapperDeSay::requestAudioGuidancePlay()
//{
//	SVPBool svpBool = m_pExtNavi->RequestAudioGuidancePlay();
//	responseAudioGuidancePlay(svpBool);
//	return;
//}

/**
*	@brief
*	֪ͨnavi Guidance�Ƿ���Բ���
*/
//SVPVoid NaviSysIPCWrapperDeSay::responseAudioGuidancePlay(SVPBool svpBool)
//{
//	// �������
//	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
//	SVPBool* mBool = (SVPBool*)malloc(sizeof(SVPBool));
//	*mBool = svpBool;
//	paramData->data = mBool;
//	paramData->funcId = ServiceManager::FuncID_Receive_ResponseAudioGuidancePlay;
//	// ִ�лص�
//	if (NULL != m_listerner)
//	{
//		m_listerner->messageListener(paramData);
//	}
//	else
//	{
//		free(paramData);
//		free(mBool);
//	}
//	return;
//}


/**
*	@brief
*	�ͷ�Guidance��������Ȩ��
*/
//void NaviSysIPCWrapperDeSay::releaseAudioGuidancePlay()
//{
//	m_pExtNavi->ReleaseAudioGuidancePlay();
//}

/*
* Description:
* This function is called when audio guidance is request to be stopped.
* The current audio guidance shoud be stopped immediately.
* This function only stops the current audio guidance.
* The later audio guidance can also be played if RequestAudioGuidancePlay returns SVP_TRUE.

* Parameters:
* bPause:
* this parameter is not used.
*/
//SVPVoid NaviSysIPCWrapperDeSay::OnStopAudioGuidancePlayRequested(SVPBool bPause)
//{
//	responseAudioGuidancePlay(bPause);
//	return;
//}
/************************************1.6 AudioGuidance end************************************/

/************************************1.7 getLanguageID start**********************************/

/**
*	@brief
*	��ȡ��ǰ����ID
*/
void NaviSysIPCWrapperDeSay::getLanguageID()
{
	uint8 languageId = m_pExtNavi->GetLanguageID();
	updateLanguageID(languageId);
	return;
}

/**
*	@brief
*	֪ͨnavi��������ID
*/
void NaviSysIPCWrapperDeSay::updateLanguageID(uint8 languageId)
{
	// �������
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	uint8* mLanguageId = (uint8*)malloc(sizeof(uint8));
	*mLanguageId = languageId;
	paramData->data = mLanguageId;
	paramData->funcId = ServiceManager::FuncID_Receive_GetLanguageID;
	// ִ�лص�
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mLanguageId);
	}
	return;
}
/************************************1.7 getLanguageID end************************************/

/************************************1.7 GPS start********************************************/
/**
*	@brief
*	����gpsʱ������Ƿ����
*/
//void NaviSysIPCWrapperDeSay::isGPSUpdateTimeEnabled()
//{
//	SVPBool sBool = m_pExtNavi->IsGPSUpdateTimeEnabled();
//	responseGPSUpdateTimeEnabled(sBool);
//	return;
//}

/**
*	@brief
*	֪ͨNavigpsʱ������Ƿ����
*/
//SVPVoid NaviSysIPCWrapperDeSay::responseGPSUpdateTimeEnabled(SVPBool svpBool)
//{
//	// �������
//	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
//	SVPBool* mBool = (SVPBool*)malloc(sizeof(SVPBool));
//	*mBool = svpBool;
//	paramData->data = mBool;
//	paramData->funcId = ServiceManager::FuncID_Receive_IsGPSUpdateTimeEnabled;
//	// ִ�лص�
//	if (NULL != m_listerner)
//	{
//		m_listerner->messageListener(paramData);
//	}
//	else
//	{
//		free(paramData);
//		free(mBool);
//	}
//	return;
//}
/************************************1.7 GPS end**********************************************/


/************************************1.8 Route start******************************************/
/**
*	@brief
*	����·��״̬
*/
void NaviSysIPCWrapperDeSay::setRouteStatus(bool bStart)
{
	m_pExtNavi->SetRouteStatus(bStart);
}
/************************************1.8 Route end********************************************/

/************************************1.8 CarPlay start****************************************/
/**
*	@brief
*	carPlay Navi�Ƿ�Active
*/
//void NaviSysIPCWrapperDeSay::isCarPlayNaviActive()
//{
//	SVPBool sBool = m_pExtNavi->IsCarPlayNaviActive();
//	responseisCarPlayNaviActive(sBool);
//	return;
//}

/**
*	@brief
*	֪ͨNavi carPlay Navi�Ƿ�Active
*/
//SVPVoid NaviSysIPCWrapperDeSay::responseisCarPlayNaviActive(SVPBool svpBool)
//{
//	// �������
//	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
//	SVPBool* mBool = (SVPBool*)malloc(sizeof(SVPBool));
//	*mBool = svpBool;
//	paramData->data = mBool;
//	paramData->funcId = ServiceManager::FuncID_Receive_IsCarPlayNaviActive;
//	// ִ�лص�
//	if (NULL != m_listerner)
//	{
//		m_listerner->messageListener(paramData);
//	}
//	else
//	{
//		free(paramData);
//		free(mBool);
//	}
//	return;
//}

/**
*	@brief
*	֪ͨNavi carPlay Navi״̬�仯
*/
//SVPVoid NaviSysIPCWrapperDeSay::OnCarPlayNaviStatusChanged(SVPBool bStart)
//{
//	responseisCarPlayNaviActive(bStart);
//}
/************************************1.8 CarPlay end******************************************/

/************************************1.9 MapCard start****************************************/
/**
*	@brief
*	֪ͨNavi SD card״̬�仯
*/
//SVPVoid NaviSysIPCWrapperDeSay::OnMapCardChanged(SVPBool uiAttached)
//{
//	// �������
//	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
//	SVPBool* mBool = (SVPBool*)malloc(sizeof(SVPBool));
//	*mBool = uiAttached;
//	paramData->data = mBool;
//	paramData->funcId = ServiceManager::FuncID_Receive_MapCardChanged;
//	// ִ�лص�
//	if (NULL != m_listerner)
//	{
//		m_listerner->messageListener(paramData);
//	}
//	else
//	{
//		free(paramData);
//		free(mBool);
//	}
//	return;
//}
/************************************1.9 MapCard end******************************************/

/************************************2.0 Beep start*******************************************/
/**
*	@brief
*		Navi����Beep��
*/
void NaviSysIPCWrapperDeSay::reqBeep(BEEP_TYPE beepType)
{
	m_pExtNavi->PlayBeep(beepType);
	return;
}

/**
*	@brief
*		Navi�����ض�Beep��
*/
void NaviSysIPCWrapperDeSay::reqBeep(const std::string& player_id)
{
	if (!player_id.empty())
	{
		m_pExtNavi->PlayBeep(player_id);
	}
	return;
}

/**
*	@brief
*		Navi����wifi״̬
*/
void NaviSysIPCWrapperDeSay::getWifiStatus()
{
	int32 wifiStatus;
	m_pExtNavi->GetWifiSignal(&wifiStatus);
}

/**
*	@brief
*	�ص�����wifi״̬
*/
void NaviSysIPCWrapperDeSay::updateWifiStatus(int32 wifistatus)
{
	// �������
	ServiceManager::ReceiveData* paramData = (ServiceManager::ReceiveData*)malloc(sizeof(ServiceManager::ReceiveData));
	int32* mWifiStatus = (int32*)malloc(sizeof(int32));
	*mWifiStatus = wifistatus;
	paramData->data = mWifiStatus;
	paramData->funcId = ServiceManager::FuncID_Receive_WifiStatusChanged;
	// ִ�лص�
	if (NULL != m_listerner)
	{
		m_listerner->messageListener(paramData);
	}
	else
	{
		free(paramData);
		free(mWifiStatus);
	}
	return;
}

/************************************2.0 Beep end*********************************************/