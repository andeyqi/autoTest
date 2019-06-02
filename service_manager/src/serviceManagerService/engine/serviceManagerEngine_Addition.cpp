#include "stdafx_serviceManagerEngine.h"
//#include "serviceManagerEngine_Addition.h"
//extern "C"
//{
#include "jansson/jansson.h"
#include "dalr/dalr_file.h"
//}

namespace ServiceManager
{
	// [TEMP]: add by liubo
	// delete by shenghao
	//uint32 flag_just_test = 0;  // bit 0 is uuid; bit 1 is PKB; bit 3 is ILL; bit 4 is wifi
	//BOOL flag_navi_init_state = FALSE;
	//std::mutex lck;
	//std::unique_lock<std::mutex> lck(mtx);
	// ��������
	void AdditionSvcEngine_RunStatusChanged(void* data);
	void AdditionSvcEngine_WifiStatusChanged(void* data);	
	void AdditionServiceEngine_SgnUUIDChanged(void* data);	
	// GPS����ʱ���Ƿ���� Ӧ��
	void AdditionSvcEngine_AnwserIsGPSUpdateTimeEnabled(void* data);
	// Map SD���仯֪ͨ
	void AdditionSvcEngine_AnwserMapCardChanged(void* data);
	// Notify
	void GPSUpdateTimeEnabledNotify(void* data);
	void MapCardChangedNotify(void* data);
	//USB״̬
	void AnswerReqUsbStatus(void* data);
	//WIFI/DCM״̬
	void AdditionSvcEngine_NetworkStatusChange(void* data);
	//����״̬
	//void AdditionSvcEngine_RevStatusChanged(void* data);
	//GBOOK��Ϣ
	void AdditionSvcEngine_gbookInfoChange(void* data);
	//�ֳ���Я�豸���֪ͨ
	void AdditionSvcEngine_DeviceConnectChange(void* data);
	//�ֳ���Я��ʼ����
	void AdditionSvcEngine_StartPhoneConnectResponse(void* data);
	//uuid��Ȩʧ��
	void SendUuidError(void);
	/**
	*	@brief
	*		ʵ����
	*/
	AdditionSvcEngine* AdditionSvcEngine::m_instance = NULL;

	AdditionSvcEngine* AdditionSvcEngine::getInstance()
	{
		if (NULL == m_instance)
		{
			m_instance = new AdditionSvcEngine();
			if (NULL != m_instance)
			{
				m_instance->startService();
			}

			return m_instance;
		}
		return m_instance;
	}
	//delete by shenghao for exit timeout
	//void AdditionSvcEngine::releaseInstance()
	//{
	//	CQ_LOG_INFO("[ServiceManager] AdditionSvcEngine::releaseInstance.\n");
	//	if (NULL != m_instance)
	//	{
	//		m_instance->stopService();
	//		delete m_instance;
	//		m_instance = NULL;
	//	}
	//	return;
	//}

	/**
	*	@brief
	*		���캯��
	*/
	AdditionSvcEngine::AdditionSvcEngine()
		:m_uuidEvent(invalidHandle),
		m_runStatusEvent(invalidHandle),
		m_GPSUpdateTimeEvent(invalidHandle),
		m_addionSvcUsbEvent(invalidHandle),
		m_networkStatusEvent(invalidHandle),
		m_gbookInfoEvent(invalidHandle),
		m_deviceInfoEvent(invalidHandle),
		m_isReceiveMemoryClear(FALSE)
	{
		m_uuid[0] = '\0';
		m_runStatus = EN_ADDITIONSVCENGINE_RUN_STATE_INVALID;
		m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_INVALID;
		m_usbData = NULL;
		m_revStatus = EN_ADDITONSVCENGINE_REV_STATE_INVALID;
		m_networkStatus = EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID;
		m_gbookInfo[0] = '\0';
		m_deviceInfo.connectType = EN_ADDITONSVCENGINE_DEVICE_STATE_INVALID;
		m_deviceInfo.VendorId = 0;
		m_deviceInfo.ProductId = 0;
		m_deviceInfo.deviceType = EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID;
		m_deviceInfo.protocolType = EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID;
	}

	/**
	*	@brief
	*		��������
	*/
	AdditionSvcEngine::~AdditionSvcEngine()
	{
	
	}

	/**
	*	@brief
	*		��ʼ��
	*/
	bool AdditionSvcEngine::InitInstance(void)
	{
		CQ_LOG_INFO("Service Manger addition engine init.\n");
		// ��Receiverע��callback(��ͬ��)
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_RunStatusChanged, AdditionSvcEngine_RunStatusChanged);
		//NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_WifiStatusChanged, AdditionSvcEngine_WifiStatusChanged);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_getUUID, AdditionServiceEngine_SgnUUIDChanged);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_IsGPSUpdateTimeEnabled, AdditionSvcEngine_AnwserIsGPSUpdateTimeEnabled);
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_MapCardChanged, AdditionSvcEngine_AnwserMapCardChanged);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_GetUsbStatus, AnswerReqUsbStatus);
		//NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_RevStatusChanged, AdditionSvcEngine_RevStatusChanged);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_NetworkStatusChanged, AdditionSvcEngine_NetworkStatusChange);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_GbookInfoChanged, AdditionSvcEngine_gbookInfoChange);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_DeviceConnectChanged, AdditionSvcEngine_DeviceConnectChange);
		NaviSysReceiver::getInstance()->setRealTimeCallback(FuncID_Receive_EntryPhoneConnectResponse, AdditionSvcEngine_StartPhoneConnectResponse);

		return true;		
	}

	/**
	*	@brief
	*		cleanup
	*/
	//delete by shenghao for exit timeout
	//bool AdditionSvcEngine::ExitInstance(void)
	//{
	//	CQ_LOG_INFO("Service Manger addition engine cleanup.\n");

	//	return true;
	//}

	/**
	*	@brief
	*		GPS����ʱ���Ƿ����֪ͨ
	*	@param [in] bool
	*		true ����  false��������
	*	@return void
	*/
	void AdditionSvcEngine::GPSUpdateTimeEnabledNotify(bool data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onGPSUpdateTimeEnabledNotify(data);

		return;
	}

	/**
	*	@brief
	*		Map SD ��״̬�仯
	*	@param [in] bool
	*		true attahced�� false��detahced
	*	@return void
	*/
	void AdditionSvcEngine::MapCardChangedNotify(bool data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onMapCardChangedNotify(data);

		return;
	}

	/*========================���й���(����״̬)==========================*/
	EN_ADDITONSVCENGINE_REV_STATE AdditionSvcEngine::getRevStatus()
	{
		CQ_LOG_INFO("[ServiceManager] getRevStatus() Start!\n");
		//�ж��Ƿ��ǵ�һ������
		if (m_revStatus == EN_ADDITONSVCENGINE_REV_STATE_INVALID)
		{
			CQ_LOG_INFO("[ServiceManager]Get Rev status by first.\n");
			getRunStatus();

		}
		CQ_LOG_INFO("[ServiceManager]m_revStatus is %d.\n", m_revStatus);
		return m_revStatus;
	}

	void RevStatusChangedNotify(void* data)
	{
		UNUSED_VAR(data);
		CQ_LOG_INFO("[ServiceManager] m_revStatus is %d\n", AdditionSvcEngine::getInstance()->m_revStatus);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_RevStatusChangedNotify(AdditionSvcEngine::getInstance()->m_revStatus);
	}

	void AdditionSvcEngine::AdditionSvcEngine_RevStatusChangedNotify(EN_ADDITONSVCENGINE_REV_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;
		{
			for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
				((AdditionSvcEngineListener*)*iter)->onRevStatusChangedNotify(data);
		}
	}

	/*========================���й���(����״̬)==========================*/

	/*=============================���й���===============================*/
	/**
	*	@brief
	*		��ȡ���й���״̬
	*	@param [in] void
	*	@return EN_ADDITIONSVCENGINE_RUN_STATE
	*		���й���״̬
	*/
	EN_ADDITIONSVCENGINE_RUN_STATE AdditionSvcEngine::getRunStatus(void)
	{
		CQ_LOG_INFO("[ServiceManager] getRunStatus() Start!\n");
		// ��һ�λ�ȡ��Ҫͨ��dbusȡ��
		if (EN_ADDITIONSVCENGINE_RUN_STATE_INVALID == m_runStatus)
		{
			m_runStatusEvent = Mapbar_createEvent(FALSE, FALSE);

			int retryNum = 3;  // retry 3��
			MapbarWaitState state = MapbarWaitState_failed;
			do
			{
				SendData* paramData = (SendData*)malloc(sizeof(SendData));
				if (paramData == NULL)
				{
					return EN_ADDITIONSVCENGINE_RUN_STATE_RUNSTS;
				}
				paramData->funcId = FuncID_Send_RunStatusRequest;
				paramData->data = NULL;
				NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
				state = Mapbar_waitEventWithTimeout(m_runStatusEvent, 1000); // �ȴ�1��
				if (state == MapbarWaitState_ok)
					break;
				if (state == MapbarWaitState_timeout)
				{
					CQ_LOG_INFO("[ServiceManager]getRunStatus() timeout\n");
					//�˴�����Mapbar_setEventʱ���´�Mapbar_waitEventWithTimeout����Ч��
					//Mapbar_setEvent(m_runStatusEvent);
					continue;
				}
			} while (--retryNum);

			Mapbar_destroyEvent(m_runStatusEvent);
			m_runStatusEvent = invalidHandle;
		}

		// 3��retryʧ�ܷ��أ��������й����д���
		if (EN_ADDITIONSVCENGINE_RUN_STATE_INVALID == m_runStatus)
		{
			return EN_ADDITIONSVCENGINE_RUN_STATE_RUNSTS;
		}

		// �����ȡ�ɹ������жϺ󷵻���Ӧ״̬
		CQ_LOG_INFO("[ServiceManager] m_runStatus is %d.\n", m_runStatus);
		return m_runStatus;
	}

	/**
	*	@brief
	*		���й���״̬�仯֪ͨ
	*	@param [in] AdditionSvcEngineRunStatus
	*		data ״̬��Ϣ
	*	@return void
	*/
	void AdditionSvcEngine::runStatusChangedNotify(EN_ADDITIONSVCENGINE_RUN_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onRunStatusChangedNotify(data);

		return;
	}

	// ֪ͨ����
	void runStatusChangedNotify(void* param)
	{
		UNUSED_VAR(param);
		AdditionSvcEngine::getInstance()->runStatusChangedNotify(AdditionSvcEngine::getInstance()->m_runStatus);
		return;
	}

	/**
	*	@brief
	*		���й���״̬�仯
	*	@param [in] void
	*		data ���й���״̬��Ϣ
	*	@return void
	*/
	void AdditionSvcEngine_RunStatusChanged(void* data)
	{
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		//�Ƿ�ֵ���߲���Ϊ��
		if (NULL == data)
		{
			CQ_LOG_INFO("[ServiceManager] data is NULL.\n");
			if(ins->m_runStatusEvent != invalidHandle)
			Mapbar_setEvent(ins->m_runStatusEvent);
			return;
		}
		int32 runStatus = *(int32*)data;
		if (runStatus == -1)
		{
			CQ_LOG_INFO("[ServiceManager] runStatus is illegal.\n");
			if (ins->m_runStatusEvent != invalidHandle)
			Mapbar_setEvent(ins->m_runStatusEvent);
			return;
		}
		else
		{
			CQ_LOG_INFO("[ServiceManager] run status is %d.\n", *(int32*)data);

			//�ٶ���Ϣ�ж�
			if (0 == (runStatus & (int32)1))
				//δ����
			    ins->m_runStatus = EN_ADDITIONSVCENGINE_RUN_STATE_DISRUNSTS;
			else if (1 == (runStatus & (int32)1))
				//������
				ins->m_runStatus = EN_ADDITIONSVCENGINE_RUN_STATE_RUNSTS;

			//�жϵ�����Ϣ
			if (0 == ((runStatus >> 2) & (int32)1))
				//�����ر�
				ins->m_revStatus = EN_ADDITONSVCENGINE_REV_STATE_UNREVERSE;
			else if (1 == ((runStatus >> 2) & (int32)1))
				//����
				ins->m_revStatus = EN_ADDITONSVCENGINE_REV_STATE_REVERSE;

			if (ins->m_runStatusEvent != invalidHandle)
			{
				CQ_LOG_INFO("[ServiceManager] run state Mapbar_setEvent .\n");
				Mapbar_setEvent(ins->m_runStatusEvent);
			}
			else
			{
				Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)runStatusChangedNotify, NULL);
				Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)RevStatusChangedNotify, NULL);
			}
		}
		return;
	}
	/*=============================���й���===============================*/

	/*==============================3.0 UUID==============================*/
	/**
	*	@brief
	*		�������󳵻��ṩUUID(ͬ��Ҫ��)
	*	@return
	*	 	UUID Value
	*/
	char* AdditionSvcEngine::getUUID(void)
	{
		// ��һ��ͨ��dubs��ȡ
		if ('\0' == m_uuid[0])
		{
			// [TEMMP]: add by liubo
			// delete by shenghao
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, before uuid wait flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
			//flag_just_test |= 1;  // wait ���߳�, |0001
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, after  uuid wait flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);

			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:get uuid by first.\n", __FUNCTION__, __LINE__);
			m_uuidEvent = Mapbar_createEvent(FALSE, FALSE);

			int retryNum = 3;  // retry 3��
			MapbarWaitState state = MapbarWaitState_failed;
			do
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				if (paramData == NULL)
				{
					return NULL;
				}
				paramData->funcId = ServiceManager::FuncID_Send_getUUID;
				paramData->data = (void*)NULL;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				state = Mapbar_waitEventWithTimeout(m_uuidEvent, 1000);//�ȴ�1��
				if (state == MapbarWaitState_ok)
				{
					CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:wait uuidEvent is ok.\n", __FUNCTION__, __LINE__);
					break;
				}
					
				if (state == MapbarWaitState_timeout)
				{
					CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:wait uuidEvent timeout.\n", __FUNCTION__, __LINE__);
					//�˴�����Mapbar_setEventʱ���´�Mapbar_waitEventWithTimeout����Ч��
					//Mapbar_setEvent(m_uuidEvent);
					continue;
				}

			} while (--retryNum);
			Mapbar_destroyEvent(m_uuidEvent);
			m_uuidEvent = invalidHandle;

			// [TEMP]: add by liubo
			// delete by shenghao
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, before uuid release flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
			////flag_just_test &= 14; // �ͷ����߳� &1110
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, after  uuid release flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
		}

		// 3��retryʧ�ܺ󷵻�NULL
		if ('\0' == m_uuid[0])
		{
			return NULL;
		}

		// ��ȷ����uuidֵ
		return m_uuid;
	}

	/**
	*	@brief
	*		����ʧ����Ϣ
	*	@return void
	*/
	void SendUuidError(void)
	{
		//������Ϣ
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return;
		}
		paramData->funcId = FuncID_Send_UuidError;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	}

	/**
	*	@brief
	*		��ȨUUID
	*	@return BOOL
	*	 	true:��Ȩ�ɹ�; false:��Ȩʧ��
	*/
	BOOL AdditionSvcEngine::isNaviActivate(void)
	{
		//��Բ�ͬ����ϵͳUUID�п��ܲ�ͬ���Ĵ���ʹ�����в�ͬUUID���������������е������� 20180904
		//dummy ɾ�������߼�Ȩ���� 20181015
		//return TRUE;
		
		if (NULL == getUUID())
		{
			CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,uuid value is NULL!\n", __FUNCTION__, __LINE__);
			SendUuidError();
			return FALSE;
		}

		char* uuidValue = (char*)malloc(sizeof(char)* (strlen(getUUID()) + 1));
		if (NULL == uuidValue){
			CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d, malloc error!\n", __FUNCTION__, __LINE__);
			SendUuidError();
			return FALSE;			
		}
		
		strcpy(uuidValue, getUUID());

		// ʾ����12000OEM0D0001D26118
		// ���
		//char uuidValueYear[3] = { 0 };
		//uuidValueYear[0] = uuidValue[0];
		//uuidValueYear[1] = uuidValue[1];
		//uuidValueYear[2] = '\0';
		
		// ���ֺ�
		char uuidValueMachine[4] = { 0 };
		uuidValueMachine[0] = uuidValue[2];
		uuidValueMachine[1] = uuidValue[3];
		uuidValueMachine[2] = uuidValue[4];
		uuidValueMachine[3] = '\0';

		// OEM
		char uuidValueOem[4] = { 0 };
		uuidValueOem[0] = uuidValue[5];
		uuidValueOem[1] = uuidValue[6];
		uuidValueOem[2] = uuidValue[7];
		uuidValueOem[3] = '\0';

		// ��ȡ�����ļ�
		char uuidYear[UUID_YEAR_SIZE] = { 0 };
		char uuidMachine[UUID_MACH_SIZE] = { 0 };
		char uuidOem[UUID_OEM_SIZE] = { 0 };
		//char uuidNum[UUID_NUM_SIZE] = { 0 };

		//bool isYearPass = false;
		bool isMachinePass = false;
		bool isOemPass = false;

		json_error_t jError;
		FileObject file;
		File_construct(&file);
		if (!File_openFallible(&file, CONFIG_FILE_JSON, File_OpenForRead))
		{
			//File_close(&file);
			if (NULL != uuidValue)
			{
				free(uuidValue);
				uuidValue = NULL;
			}
			File_destruct(&file);
			SendUuidError();
			return FALSE;
		}

		int32 fileLength = File_getLength(&file);
		cqCHAR *fileBuffer = (cqCHAR *)malloc(sizeof(cqCHAR) * fileLength);
		File_read(&file, fileBuffer, sizeof(cqCHAR) * fileLength);
		json_t* jRoot = json_loadb(fileBuffer, fileLength, JSON_DECODE_ANY, &jError);
		
		// ��ȡkeyΪuuid_year�ļ�ֵ������ͬ��
		//���У��
		//json_t* jValue = json_object_get(jRoot, "uuid_year");
		//if (jValue != NULL)
		//{
		//	size_t yearArraySize = json_array_size(jValue);
		//	for (size_t i = 0; i < yearArraySize; i++)
		//	{
		//		cq_strcpy_s(uuidYear, sizeof(uuidYear) / sizeof(uuidYear[0]), json_string_value(json_array_get(jValue, i)));
		//		 
		//		if (0 == strcmp(uuidValueYear, uuidYear))
		//		{
		//			isYearPass = true;
		//			break;
		//		}
		//	}
		//}

		//
		json_t*jValue = json_object_get(jRoot, "uuid_machine");
		//if (jValue != NULL && isYearPass == true)
		if (jValue != NULL)
		{
			size_t machineArraySize = json_array_size(jValue);
			for (size_t i = 0; i < machineArraySize; i++)
			{
				cq_strcpy_s(uuidMachine, sizeof(uuidMachine) / sizeof(uuidMachine[0]), json_string_value(json_array_get(jValue, i)));

				if (0 == strcmp(uuidValueMachine, uuidMachine))
				{
					isMachinePass = true;
					break;
				}
			}
		}

		jValue = json_object_get(jRoot, "uuid_oem");
		//if (jValue != NULL && isYearPass == true && isMachinePass == true)
		if (jValue != NULL && isMachinePass == true)
		{
			cq_strcpy_s(uuidOem, sizeof(uuidOem) / sizeof(uuidOem[0]), json_string_value(jValue));
			if ((0 == strcmp(uuidValueOem, uuidOem)))
			{
				isOemPass = true;
			}
		}

		/*
		// �����ж�
		jValue = json_object_get(jRoot, "uuid_num");
		if (jValue != NULL)
		{
			cq_strcpy_s(uuidNum, sizeof(uuidNum) / sizeof(uuidNum[0]), json_string_value(jValue));
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, uuidNum %s.\n", __FUNCTION__, __LINE__, uuidNum);
		}
		*/

		File_close(&file);
		File_destruct(&file);
		json_decref(jRoot);

		if (NULL != uuidValue)
		{
			free(uuidValue);
			uuidValue = NULL;
		}

		if (NULL != fileBuffer)
		{
			free(fileBuffer);
			fileBuffer = NULL;
		}

		// ��Ȩuuid
		//if (isYearPass && isMachinePass && isOemPass)
		if (isMachinePass && isOemPass)
		{
			return TRUE;
		}
		else
		{
			SendUuidError();
			return FALSE;
		}
	}

	/**
	*	@brief
	*		��Recieverע���UUIDͬ��Callback����
	*	@param [in] data
	*		UUID�ַ���(���)
	*	@return
	*		void
	*/
	void AdditionServiceEngine_SgnUUIDChanged(void* data)
	{
		// add by liubo
		// uuid�����ǵ�һ����Ϣ�������ﲻ��Ҫ�ж������־λ

		if (data != NULL)
		{
			AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();

			strcpy(ins->m_uuid, (char*)data);
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:uuid %s.\n", __FUNCTION__, __LINE__, ins->m_uuid);

			if (ins->m_uuidEvent != invalidHandle)
				Mapbar_setEvent(ins->m_uuidEvent);

		}
		else
		{
			CQ_LOG_ERROR("[ServiceManager]Func:%s,Line:%d,Message:not get uuid!.\n", __FUNCTION__, __LINE__);
		}
		return;
	}
	/*==============================3.0 UUID==============================*/
	/*==============================3.15 USB==============================*/
	UsbData* AdditionSvcEngine::AdditionSvcEngine_ReqUsbStatus()
	{
		CQ_LOG_INFO("[ServiceManager] AdditionSvcEngine_ReqUsbStatus\n");
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		if (ins->m_usbData != NULL)
		{
			return m_usbData;
		}
		//event
		m_addionSvcUsbEvent = Mapbar_createEvent(FALSE, FALSE);

		//������Ϣ
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return NULL;
		}
		paramData->funcId = FuncID_Send_ReqUsbStatus;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);

		////�ȴ�ͬ�ڷ���
		MapbarWaitState state = MapbarWaitState_failed;
		state = Mapbar_waitEventWithTimeout(m_addionSvcUsbEvent, 1000); // �ȴ�1��
		if (state == MapbarWaitState_ok && ins->m_usbData->usbState == EN_ADDITONSVCENGINE_USB_MOUNT)
		{
			Mapbar_destroyEvent(m_addionSvcUsbEvent);
			m_addionSvcUsbEvent = invalidHandle;
			//CQ_LOG_INFO("[ServiceManager]AdditionSvcEngine_ReqUsbStatus %d %s\n", m_usbData->usbState, m_usbData->usbPath);
			return m_usbData;
		}
		else if (state == MapbarWaitState_ok  && ins->m_usbData->usbState == EN_ADDITONSVCENGINE_USB_UNMOUNT)
		{
			Mapbar_destroyEvent(m_addionSvcUsbEvent);
			m_addionSvcUsbEvent = invalidHandle;
			//CQ_LOG_INFO("[ServiceManager]AdditionSvcEngine_ReqUsbStatus %d %s\n", m_usbData->usbState, m_usbData->usbPath);
			return m_usbData;
		}
		if(state == MapbarWaitState_timeout || state == MapbarWaitState_failed) 
		{
			Mapbar_destroyEvent(m_addionSvcUsbEvent);
			m_addionSvcUsbEvent = invalidHandle;
			CQ_LOG_INFO("[ServiceManager] AdditionSvcEngine_ReqUsbStatus Timeout\n");
			return NULL;
		}
		return NULL;
	}
	

	void UsbStatusChangedNotify(void* data)
	{

		AdditionSvcEngine::getInstance()->AdditionSvcEngine_UsbStatusChangedNotify((UsbData*)data);

	}

	void AnswerReqUsbStatus(void* data)
	{
		CQ_LOG_INFO("[ServiceManager] AnswerReqUsbStatus %d\n", data);
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		if (data != NULL)
		{
			if (ins->m_usbData == NULL)
			{
				ins->m_usbData = (UsbData*)malloc(sizeof(UsbData));
				memset(ins->m_usbData, 0, sizeof(UsbData));
				ins->m_usbData->usbState = EN_ADDITONSVCENGINE_USB_UNMOUNT;
				ins->m_usbData->usbPath = (cqCHAR*)malloc(sizeof(cqCHAR)* (strlen(USBPATH) + 1));
				memset(ins->m_usbData->usbPath, 0, sizeof(cqCHAR)* (strlen(USBPATH) + 1));
			}

			BOOL state = *(BOOL*)data;

			if (state)
			{
				ins->m_usbData->usbState = EN_ADDITONSVCENGINE_USB_MOUNT;
				memcpy(ins->m_usbData->usbPath, USBPATH, sizeof(cqCHAR)* (strlen(USBPATH) + 1));
			}
			else
			{
				ins->m_usbData->usbState = EN_ADDITONSVCENGINE_USB_UNMOUNT;
				memcpy(ins->m_usbData->usbPath, USBPATH, sizeof(cqCHAR)* (strlen(USBPATH) + 1));
			}

			if (ins->m_addionSvcUsbEvent != invalidHandle)
			{
				CQ_LOG_INFO("[ServiceManager].run state Mapbar_setEvent .\n");
				Mapbar_setEvent(ins->m_addionSvcUsbEvent);
			}
			else
			{
				Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)UsbStatusChangedNotify, (void*)ins->m_usbData);
			}

			/*free(ins->m_usbData->usbPath);
			free(ins->m_usbData);*/
		}
		return;

	}


	void AdditionSvcEngine::AdditionSvcEngine_UsbStatusChangedNotify(UsbData* data)
	{
		//CQ_LOG_INFO("[ServiceManager]AdditionSvcEngine_UsbStatusChangedNotify %d %s\n", m_usbData->usbState, m_usbData->usbPath);
		std::set<AdditionSvcEngineListener*>::iterator iter;
		{
			for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
				((AdditionSvcEngineListener*)*iter)->onUsbStatusChangedNotify(data);
		}
	}
	/*==============================3.15 USB==============================*/
	///*===========================3.18 ����״̬============================*/

	//EN_ADDITONSVCENGINE_REV_STATE AdditionSvcEngine::getRevStatus()
	//{
	//	CQ_LOG_INFO("[ServiceManager] getRevStatus\n");
	//	AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
	//	if (m_revStatus != EN_ADDITONSVCENGINE_REV_STATE_INVALID)
	//	{
	//		CQ_LOG_INFO("[ServiceManager] use local data\n");
	//		return m_revStatus;
	//	}
	//	//event
	//	m_addionSvcRevEvent = Mapbar_createEvent(FALSE, FALSE);

	//	int retryNum = 3;  // retry 3��
	//	MapbarWaitState state = MapbarWaitState_failed;
	//	do
	//	{
	//		SendData* paramData = (SendData*)malloc(sizeof(SendData));
	//		paramData->funcId = FuncID_Send_getRevStatus;
	//		paramData->data = NULL;
	//		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	//		state = Mapbar_waitEventWithTimeout(m_addionSvcRevEvent, 1000); // �ȴ�1s
	//		if (state == MapbarWaitState_ok)
	//			break;
	//		if (state == MapbarWaitState_timeout)
	//		{
	//			CQ_LOG_ERROR("[ServiceManager] MapbarWaitState_timeout\n");
	//			Mapbar_setEvent(m_addionSvcRevEvent);
	//			continue;
	//		}
	//	} while (--retryNum);
	//	Mapbar_destroyEvent(m_addionSvcRevEvent);
	//	m_addionSvcRevEvent = invalidHandle;
	//	CQ_LOG_INFO("[ServiceManager] m_revStatus is %d\n", m_revStatus);
	//	return m_revStatus;
	//}

	//void RevStatusChangedNotify(void* data)
	//{
	//	UNUSED_VAR(data);
	//	CQ_LOG_INFO("[ServiceManager] m_revStatus is %d\n", AdditionSvcEngine::getInstance()->m_revStatus);
	//	AdditionSvcEngine::getInstance()->AdditionSvcEngine_RevStatusChangedNotify(AdditionSvcEngine::getInstance()->m_revStatus);
	//}

	//void AdditionSvcEngine_RevStatusChanged(void* data)
	//{
	//	AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
	//	if (data != NULL)
	//	{
	//		ins->m_revStatus = *(EN_ADDITONSVCENGINE_REV_STATE*)data;

	//		if (ins->m_addionSvcRevEvent != invalidHandle)
	//		{
	//			CQ_LOG_INFO("[ServiceManager].run state Mapbar_setEvent .\n");
	//			Mapbar_setEvent(ins->m_addionSvcRevEvent);
	//		}
	//		else
	//		{
	//			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)RevStatusChangedNotify, (void*)ins->m_revStatus);
	//		}
	//	}
	//	return;

	//}


	//void AdditionSvcEngine::AdditionSvcEngine_RevStatusChangedNotify(EN_ADDITONSVCENGINE_REV_STATE data)
	//{
	//	//CQ_LOG_ERROR("[ServiceManager]AdditionSvcEngine_RevStatusChangedNotify");
	//	std::set<AdditionSvcEngineListener*>::iterator iter;
	//	{
	//		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
	//			((AdditionSvcEngineListener*)*iter)->onRevStatusChangedNotify(data);
	//	}
	//}
	///*===========================3.18 ����״̬============================*/

	/*==============================3.8 WIFI==============================*/
	/**
	*	@brief
	*		��ȡwifi״̬
	*	@return
	*	 	void
	*/
	EN_ADDITIONSVCENGINE_WIFI_STATE AdditionSvcEngine::getWifiStatus(void)
	{
		if (EN_ADDITIONSVCENGINE_WIFI_STATE_INVALID == m_wifiStatus)
		{
			getNetworkStatus();

			if (m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_WIFICONNECTED)
			{
				m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_WIFICONNECTED;
			}
			else if (m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED || m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_DCMCONNECTED || m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID)
			{
				m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_WIFIDISCONNECTED;				}
		}
		CQ_LOG_INFO("[ServiceManager]getWifiStatus() m_wifiStatus is %d\n", m_wifiStatus);
		return m_wifiStatus;
	}

	/**
	*	@brief
	*		wifi״̬�仯֪ͨ
	*	@param [in] EN_ADDITIONSVCENGINE_WIFI_STATE
	*		data ״̬��Ϣ
	*	@return void
	*/
	void AdditionSvcEngine::wifiStatusChangedNotify(EN_ADDITIONSVCENGINE_WIFI_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onWifiStatusChangedNotify(data);
		return;
	}

	// ֪ͨ����
	void wifiStatusChangedNotify(void* param)
	{
		UNUSED_VAR(param);
		AdditionSvcEngine::getInstance()->wifiStatusChangedNotify(AdditionSvcEngine::getInstance()->m_wifiStatus);
		return;
	}

	/**
	*	@brief
	*		��Recieverע���WIFI����״̬ͬ��Callback����
	*	@param [in] data
	*		WIFI����״̬
	*	@return
	*		void
	*/
	void AdditionSvcEngine_WifiStatusChanged(void* data)
	{
		UNUSED_VAR(data);
		//��ȡ����
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		//�ж�network״̬
		if (ins->m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_WIFICONNECTED)
		{
			ins->m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_WIFICONNECTED;
		}
		else if (ins->m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED || ins->m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_DCMCONNECTED)
		{
			ins->m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_WIFIDISCONNECTED;
		}

		CQ_LOG_INFO("[ServiceManager]AdditionSvcEngine_WifiStatusChanged() m_wifiStatus is %d\n", ins->m_wifiStatus);
		//�����߳���ִ��֪ͨ����
		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)wifiStatusChangedNotify, NULL);
		
		return;
	}
	/*==============================3.8 WIFI==============================*/
	/*============================3.19 WIFI/DCM===========================*/
	/**
	*	@brief
	*		��ȡWIFI/DCM״̬
	*	@param [in] 
	*		void
	*	@return
	*	 	EN_ADDITIONSVCENGINE_NETWORK_STATE
	*/
	EN_ADDITIONSVCENGINE_NETWORK_STATE AdditionSvcEngine::getNetworkStatus(void)
	{
		//�ж��Ƿ��б������ݣ����б�������ֱ�ӷ��ر�������
		if (EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID == m_networkStatus)
		{
			m_networkStatusEvent = Mapbar_createEvent(FALSE, FALSE);

			int retryNum = 3;  // retry 3��
			MapbarWaitState state = MapbarWaitState_failed;
			do
			{
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				if (paramData == NULL)
				{
					return EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED;
				}
				paramData->funcId = ServiceManager::FuncID_Send_getNetworkStatus;
				paramData->data = NULL;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				state = Mapbar_waitEventWithTimeout(m_networkStatusEvent, 1000); // �ȴ�1s
				if (state == MapbarWaitState_ok)
					break;
				if (state == MapbarWaitState_timeout)
				{
					CQ_LOG_INFO("[ServiceManager]getNetworkStatus() timeout\n");
					//�˴�����Mapbar_setEventʱ���´�Mapbar_waitEventWithTimeout����Ч��
					//Mapbar_setEvent(m_networkStatusEvent);
					continue;
				}
			} while (--retryNum);
			Mapbar_destroyEvent(m_networkStatusEvent);
			m_networkStatusEvent = invalidHandle;
		}
		//����network״̬
		CQ_LOG_INFO("[ServiceManager]getNetworkStatus() NetworkStatus is %d\n", m_networkStatus);
		if (m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID)
		{
			return EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED;
		}
		return m_networkStatus;
	}

	/**
	*	@brief
	*		wifi/dcm״̬�仯֪ͨ
	*	@param [in] EN_ADDITIONSVCENGINE_NETWORK_STATE
	*		data ״̬��Ϣ
	*	@return void
	*/
	void AdditionSvcEngine::AdditionSvcEngine_networkStatusChangedNotify(EN_ADDITIONSVCENGINE_NETWORK_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onNetworkStatusChangedNotify(data);
		return;
	}


	// ֪ͨ����
	void networkStatusChangedNotify(void* data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_networkStatusChangedNotify(AdditionSvcEngine::getInstance()->m_networkStatus);
		return;
	}

	/**
	*	@brief
	*		��Recieverע���WIFI/DCM����״̬ͬ��Callback����
	*	@param [in] data
	*		WIFI/DCM����״̬
	*	@return
	*		void
	*/
	void AdditionSvcEngine_NetworkStatusChange(void* data)
	{
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]AdditionSvcEngine_NetworkStatusChange data is NULL.\n");
			if (ins->m_networkStatusEvent != invalidHandle)
				Mapbar_setEvent(ins->m_networkStatusEvent);
			return;
		}

		ins->m_networkStatus = *(EN_ADDITIONSVCENGINE_NETWORK_STATE*)data;  // ����wifi/dcm״̬������
		if (ins->m_networkStatusEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_networkStatusEvent);
		}
		else
		{
			AdditionSvcEngine_WifiStatusChanged((void*) ins->m_networkStatus);
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)networkStatusChangedNotify, NULL);
		}
		return;
	}
	/*============================3.19 WIFI/DCM===========================*/
	/*===========================3.19 GBOOK��Ϣ===========================*/
	/**
	*	@brief
	*		��ȡG-BOOK��Ϣ
	*	@param [in]
	*		void
	*	@return
	*	 	char*
	*/

	char* AdditionSvcEngine::getGbookInfo(void)
	{
		if (m_gbookInfo[0] == '\0')
		{
			m_gbookInfoEvent = Mapbar_createEvent(FALSE, FALSE);
			int retryNum = 3;  // retry 3��
			MapbarWaitState state = MapbarWaitState_failed;

			do
			{
				//����������Ϣ
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				if (paramData == NULL)
				{
					return NULL;
				}
				paramData->funcId = ServiceManager::FuncID_Send_getGbookInfo;
				paramData->data = NULL;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				state = Mapbar_waitEventWithTimeout(m_gbookInfoEvent, 1000); // �ȴ�1s

				if (state == MapbarWaitState_ok)
					break;
				if (state == MapbarWaitState_timeout || m_gbookInfo[0] == '\0')
				{
					CQ_LOG_INFO("[ServiceManager]getGbookInfo() timeout\n");
					//�˴�����Mapbar_setEventʱ���´�Mapbar_waitEventWithTimeout����Ч��
					//Mapbar_setEvent(m_gbookInfoEvent);
					continue;
				}
			} while (--retryNum);

			Mapbar_destroyEvent(m_gbookInfoEvent);
			m_gbookInfoEvent = invalidHandle;

			if (m_gbookInfo[0] == '\0')
			{
				CQ_LOG_ERROR("[ServiceManager]getGbookInfo failure data is NULL.\n");
				return NULL;
			}
			else
			{
				CQ_LOG_INFO("[ServiceManager]getGbookInfo success data is %s.\n", &m_gbookInfo);
				return m_gbookInfo;
			}
		}
		CQ_LOG_INFO("[ServiceManager]getGbookInfo use local data is %s.\n",&m_gbookInfo);
		return m_gbookInfo;
	}

	/**
	*	@brief
	*		G-BOOK�仯֪ͨ
	*	@param [in] 
	*		data G-BOOK��Ϣ
	*	@return void
	*/
	void AdditionSvcEngine::AdditionSvcEngine_gbookInfoChangedNotify(char* data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onGbookInfoChangedNotify(data);
		return;
	}


	// ֪ͨ����
	void gbookInfoChangedNotify(void *data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_gbookInfoChangedNotify(AdditionSvcEngine::getInstance()->m_gbookInfo);
		return;
	}

	/**
	*	@brief
	*		��Recieverע���G-BOOK��Ϣͬ��Callback����
	*	@param [in] data
	*		G-BOOK��Ϣ
	*	@return
	*		void
	*/
	void AdditionSvcEngine_gbookInfoChange(void* data)
	{
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]AdditionSvcEngine_gbookInfoChange data is NULL.\n");
			if (ins->m_gbookInfoEvent != invalidHandle)
				Mapbar_setEvent(ins->m_gbookInfoEvent);
			return;
		}
		strcpy(ins->m_gbookInfo, (char*)data);
		if (ins->m_gbookInfoEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_gbookInfoEvent);
		}
		else
		{
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)gbookInfoChangedNotify, NULL);
		}
		return;
	}
	/*============================3.19 G-BOOK��Ϣ==========================*/

/*=============================4.0.2 �豸���֪ͨ===============================*/
	DeviceInfo* AdditionSvcEngine::getDeviceInfo()
	{
		CQ_LOG_INFO("[ServiceManager] AdditionSvcEngine_ReqUsbStatus\n");
		if (m_deviceInfo.connectType != EN_ADDITONSVCENGINE_DEVICE_STATE_INVALID)
		{
			return &m_deviceInfo;
		}
		//event
		m_deviceInfoEvent = Mapbar_createEvent(FALSE, FALSE);

		//������Ϣ
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return NULL;
		}
		paramData->funcId = FuncID_Send_getDeviceConnect;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);

		//�ȴ�ͬ�ڷ���
		//MapbarWaitState state = MapbarWaitState_failed;
		//state = Mapbar_waitEventWithTimeout(m_deviceInfoEvent, 1000); // �ȴ�1��

		Mapbar_waitEventWithTimeout(m_deviceInfoEvent, 1000); // �ȴ�1��

		Mapbar_destroyEvent(m_deviceInfoEvent);
		m_deviceInfoEvent = invalidHandle;
		return &m_deviceInfo;


	}

	void AdditionSvcEngine::AdditionSvcEngine_deviceConnectChangeNotify(DeviceInfo* data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onDeviceConnectChangedNotify(data);
		return;
	}

	void deviceConnectChangeNotify(void *data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_deviceConnectChangeNotify(&AdditionSvcEngine::getInstance()->m_deviceInfo);
		return;
	}

	void AdditionSvcEngine_DeviceConnectChange(void* data)
	{
		CQ_LOG_INFO("[service_managerfortest]AdditionSvcEngine_DeviceConnectChange\n");
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]AdditionSvcEngine_DeviceConnectChange data is NULL.\n");
			if (ins->m_deviceInfoEvent != invalidHandle)
				Mapbar_setEvent(ins->m_deviceInfoEvent);
			return;
		}
		memcpy(&(ins->m_deviceInfo), data, sizeof(DeviceInfo));

		if (ins->m_deviceInfoEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_deviceInfoEvent);
		}
		else
		{
			Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)deviceConnectChangeNotify, NULL);
		}
		return;
	}
/*=============================4.0.2 �豸���֪ͨ===============================*/

/*=============================4.0.1 ��ʼ�ֳ���Я===============================*/
	void AdditionSvcEngine::startPhoneConnectNotify(bool data)
	{
		//������Ϣ
		printf("phoneConnectNotify() start\n");
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return;
		}
		if (data == true)
		{
			paramData->funcId = FuncID_Send_EntryPhoneConnect;
		}
		else
		{
			paramData->funcId = FuncID_Send_ExitPhoneConnect;
		}
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	}
/*=============================4.0.1 ��ʼ�ֳ���Я===============================*/
/*===========================4.0.1 ��ʼ�ֳ���Я����===============================*/
	void AdditionSvcEngine::AdditionSvcEngine_startPhoneConnectResponse(EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		
		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onDeviceConnectChangedNotify(data);
		return;
	}

	void startPhoneConnectResponse(void *data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_startPhoneConnectResponse(AdditionSvcEngine::getInstance()->m_entryPhoneStatus);
		return;
	}

	void AdditionSvcEngine_StartPhoneConnectResponse(void* data)
	{
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager]AdditionSvcEngine_NetworkStatusChange data is NULL.\n");
			return;
		}

		ins->m_entryPhoneStatus = *(EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE*)data;  

		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)startPhoneConnectResponse, NULL);

	}
/*===========================4.0.1 ��ʼ�ֳ���Я����===============================*/
/*===========================4.0.3 �ֳ���Я״̬֪ͨ=============================*/
	void AdditionSvcEngine::phoneConnectStatusNotify(EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS data)
	{
		EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS* status = (EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS*)malloc(sizeof(EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS));
		if (status == NULL)
		{
			return;
		}
		*status = data;
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			free(status);
			status = NULL;
			return;
		}
		paramData->funcId = FuncID_Send_PhoneConnectStatus;
		paramData->data = status;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	}
/*===========================4.0.3 �ֳ���Я״̬֪ͨ=============================*/
	/*===============================����=================================*/
	/*========================Desai GPSUpdateTime============================*/
	/**
	*	@brief
	*		Check if GPS Update system time is enabled.
	*		this fucntion should be call when navi APP is set to foreground.
	*	@param
	*/
	BOOL AdditionSvcEngine::isGPSUpdateTimeEnabled()
	{
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		if (paramData == NULL)
		{
			return FALSE;
		}
		paramData->funcId = ServiceManager::FuncID_Send_IsGPSUpdateTimeEnabled;
		paramData->data = (void*)NULL;

		m_GPSUpdateTimeEvent = Mapbar_createEvent(FALSE, FALSE);
		NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_GPSUpdateTimeEvent, 1000);//�ȴ�1��
		if (state == MapbarWaitState_ok)
		{
			Mapbar_destroyEvent(m_GPSUpdateTimeEvent);
			m_GPSUpdateTimeEvent = invalidHandle;
			return m_isGPSEnabled;
		}

		Mapbar_destroyEvent(m_GPSUpdateTimeEvent);
		m_GPSUpdateTimeEvent = invalidHandle;
		return FALSE;
	}

	/**
	*	@brief
	*		�����������״̬
	*	@param [in] BOOL
	*	@return void
	*/
	void AdditionSvcEngine::setMemoryClearFlag(BOOL isRevMemoryClear)
	{
		m_isReceiveMemoryClear = isRevMemoryClear;
	}
	
	/**
	*	@brief
	*		��ȡ�������״̬
	*	@param [in] void
	*	@return BOOL
	*/
	BOOL AdditionSvcEngine::getMemoryClearFlag()
	{
		return m_isReceiveMemoryClear;
	}

	/**
	*	@brief
	*		��Recieverע��GPS����ʱ���Ƿ���õ�Callback����
	*	@param [in] data
	*		�Ƿ����
	*	@return
	*		void
	*/
	void AdditionSvcEngine_AnwserIsGPSUpdateTimeEnabled(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AdditionSvcEngine_AnwserIsGPSUpdateTimeEnabled is NULL!\n");
			return;
		}

		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		ins->m_isGPSEnabled = *(bool*)data;
		CQ_LOG_INFO("[ServiceManager] AnswerVolume = %d\n", ins->m_isGPSEnabled);
		if (ins->m_GPSUpdateTimeEvent != invalidHandle)
		{
			Mapbar_setEvent(ins->m_GPSUpdateTimeEvent);
		}
		//else
		//{
		//	bool* pdata = (bool*)malloc(sizeof(bool));
		//	*pdata = ins->m_isgpsenabled;
		//	mapbar_runfunctioninmainthread((mapbar_mainthreadfunc)gpsupdatetimeenablednotify, pdata);
		//}
		return;
	}

	void GPSUpdateTimeEnabledNotify(void* data)
	{
		AdditionSvcEngine::getInstance()->GPSUpdateTimeEnabledNotify(*(bool*)data);
	}

	/**
	*	@brief
	*		��Recieverע��Callback����,��sd card״̬�仯ʱ�ص�
	*	@param [in] data
	*		SD card״̬
	*	@return
	*		void
	*/
	void AdditionSvcEngine_AnwserMapCardChanged(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] AdditionSvcEngine_AnwserMapCardChanged data is NULL!\n");
			return;
		}

		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)MapCardChangedNotify, data);
		return;
	}

	void MapCardChangedNotify(void* data)
	{
		if (NULL == data)
		{
			CQ_LOG_ERROR("[ServiceManager] MapCardChangedNotify data is NULL!\n");
			return;
		}
		AdditionSvcEngine::getInstance()->MapCardChangedNotify(*(bool *)data);
		// del by liubo
		// Receiver�н����ͷ�
		/*
		if (NULL != data)
		{
			free(data);
			data = NULL;
		}
		*/
	}
	/*===============================����=================================*/

}
