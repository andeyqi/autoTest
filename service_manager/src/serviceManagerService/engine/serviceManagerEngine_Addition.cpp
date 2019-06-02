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
	// 函数声明
	void AdditionSvcEngine_RunStatusChanged(void* data);
	void AdditionSvcEngine_WifiStatusChanged(void* data);	
	void AdditionServiceEngine_SgnUUIDChanged(void* data);	
	// GPS更新时间是否可用 应答
	void AdditionSvcEngine_AnwserIsGPSUpdateTimeEnabled(void* data);
	// Map SD卡变化通知
	void AdditionSvcEngine_AnwserMapCardChanged(void* data);
	// Notify
	void GPSUpdateTimeEnabledNotify(void* data);
	void MapCardChangedNotify(void* data);
	//USB状态
	void AnswerReqUsbStatus(void* data);
	//WIFI/DCM状态
	void AdditionSvcEngine_NetworkStatusChange(void* data);
	//倒车状态
	//void AdditionSvcEngine_RevStatusChanged(void* data);
	//GBOOK信息
	void AdditionSvcEngine_gbookInfoChange(void* data);
	//手车连携设备插拔通知
	void AdditionSvcEngine_DeviceConnectChange(void* data);
	//手车连携开始反馈
	void AdditionSvcEngine_StartPhoneConnectResponse(void* data);
	//uuid鉴权失败
	void SendUuidError(void);
	/**
	*	@brief
	*		实例化
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
	*		构造函数
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
	*		析构函数
	*/
	AdditionSvcEngine::~AdditionSvcEngine()
	{
	
	}

	/**
	*	@brief
	*		初始化
	*/
	bool AdditionSvcEngine::InitInstance(void)
	{
		CQ_LOG_INFO("Service Manger addition engine init.\n");
		// 向Receiver注册callback(非同期)
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
	*		GPS更新时间是否可用通知
	*	@param [in] bool
	*		true 可用  false：不可用
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
	*		Map SD 卡状态变化
	*	@param [in] bool
	*		true attahced， false：detahced
	*	@return void
	*/
	void AdditionSvcEngine::MapCardChangedNotify(bool data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onMapCardChangedNotify(data);

		return;
	}

	/*========================走行规制(倒车状态)==========================*/
	EN_ADDITONSVCENGINE_REV_STATE AdditionSvcEngine::getRevStatus()
	{
		CQ_LOG_INFO("[ServiceManager] getRevStatus() Start!\n");
		//判断是否是第一次请求
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

	/*========================走行规制(倒车状态)==========================*/

	/*=============================走行规制===============================*/
	/**
	*	@brief
	*		获取走行规制状态
	*	@param [in] void
	*	@return EN_ADDITIONSVCENGINE_RUN_STATE
	*		走行规制状态
	*/
	EN_ADDITIONSVCENGINE_RUN_STATE AdditionSvcEngine::getRunStatus(void)
	{
		CQ_LOG_INFO("[ServiceManager] getRunStatus() Start!\n");
		// 第一次获取需要通过dbus取得
		if (EN_ADDITIONSVCENGINE_RUN_STATE_INVALID == m_runStatus)
		{
			m_runStatusEvent = Mapbar_createEvent(FALSE, FALSE);

			int retryNum = 3;  // retry 3次
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
				state = Mapbar_waitEventWithTimeout(m_runStatusEvent, 1000); // 等待1秒
				if (state == MapbarWaitState_ok)
					break;
				if (state == MapbarWaitState_timeout)
				{
					CQ_LOG_INFO("[ServiceManager]getRunStatus() timeout\n");
					//此处调用Mapbar_setEvent时，下次Mapbar_waitEventWithTimeout不生效。
					//Mapbar_setEvent(m_runStatusEvent);
					continue;
				}
			} while (--retryNum);

			Mapbar_destroyEvent(m_runStatusEvent);
			m_runStatusEvent = invalidHandle;
		}

		// 3次retry失败返回，按照走行规制中处理
		if (EN_ADDITIONSVCENGINE_RUN_STATE_INVALID == m_runStatus)
		{
			return EN_ADDITIONSVCENGINE_RUN_STATE_RUNSTS;
		}

		// 如果获取成功，则判断后返回响应状态
		CQ_LOG_INFO("[ServiceManager] m_runStatus is %d.\n", m_runStatus);
		return m_runStatus;
	}

	/**
	*	@brief
	*		走行规制状态变化通知
	*	@param [in] AdditionSvcEngineRunStatus
	*		data 状态信息
	*	@return void
	*/
	void AdditionSvcEngine::runStatusChangedNotify(EN_ADDITIONSVCENGINE_RUN_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onRunStatusChangedNotify(data);

		return;
	}

	// 通知函数
	void runStatusChangedNotify(void* param)
	{
		UNUSED_VAR(param);
		AdditionSvcEngine::getInstance()->runStatusChangedNotify(AdditionSvcEngine::getInstance()->m_runStatus);
		return;
	}

	/**
	*	@brief
	*		走行规制状态变化
	*	@param [in] void
	*		data 走行规制状态信息
	*	@return void
	*/
	void AdditionSvcEngine_RunStatusChanged(void* data)
	{
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		//非法值或者参数为空
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

			//速度信息判断
			if (0 == (runStatus & (int32)1))
				//未走行
			    ins->m_runStatus = EN_ADDITIONSVCENGINE_RUN_STATE_DISRUNSTS;
			else if (1 == (runStatus & (int32)1))
				//走行中
				ins->m_runStatus = EN_ADDITIONSVCENGINE_RUN_STATE_RUNSTS;

			//判断倒车信息
			if (0 == ((runStatus >> 2) & (int32)1))
				//倒车关闭
				ins->m_revStatus = EN_ADDITONSVCENGINE_REV_STATE_UNREVERSE;
			else if (1 == ((runStatus >> 2) & (int32)1))
				//倒车
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
	/*=============================走行规制===============================*/

	/*==============================3.0 UUID==============================*/
	/**
	*	@brief
	*		导航请求车机提供UUID(同期要求)
	*	@return
	*	 	UUID Value
	*/
	char* AdditionSvcEngine::getUUID(void)
	{
		// 第一次通过dubs获取
		if ('\0' == m_uuid[0])
		{
			// [TEMMP]: add by liubo
			// delete by shenghao
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, before uuid wait flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
			//flag_just_test |= 1;  // wait 主线程, |0001
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, after  uuid wait flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);

			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:get uuid by first.\n", __FUNCTION__, __LINE__);
			m_uuidEvent = Mapbar_createEvent(FALSE, FALSE);

			int retryNum = 3;  // retry 3次
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
				state = Mapbar_waitEventWithTimeout(m_uuidEvent, 1000);//等待1秒
				if (state == MapbarWaitState_ok)
				{
					CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:wait uuidEvent is ok.\n", __FUNCTION__, __LINE__);
					break;
				}
					
				if (state == MapbarWaitState_timeout)
				{
					CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Message:wait uuidEvent timeout.\n", __FUNCTION__, __LINE__);
					//此处调用Mapbar_setEvent时，下次Mapbar_waitEventWithTimeout不生效。
					//Mapbar_setEvent(m_uuidEvent);
					continue;
				}

			} while (--retryNum);
			Mapbar_destroyEvent(m_uuidEvent);
			m_uuidEvent = invalidHandle;

			// [TEMP]: add by liubo
			// delete by shenghao
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, before uuid release flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
			////flag_just_test &= 14; // 释放主线程 &1110
			//CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d, after  uuid release flag:%d.\n", __FUNCTION__, __LINE__, flag_just_test);
		}

		// 3次retry失败后返回NULL
		if ('\0' == m_uuid[0])
		{
			return NULL;
		}

		// 正确返回uuid值
		return m_uuid;
	}

	/**
	*	@brief
	*		发送失败消息
	*	@return void
	*/
	void SendUuidError(void)
	{
		//发送消息
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
	*		鉴权UUID
	*	@return BOOL
	*	 	true:鉴权成功; false:鉴权失败
	*/
	BOOL AdditionSvcEngine::isNaviActivate(void)
	{
		//针对不同车机系统UUID有可能不同做的处理，使得所有不同UUID车机可以正常运行导航程序 20180904
		//dummy 删除正常走鉴权流程 20181015
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

		// 示例：12000OEM0D0001D26118
		// 年份
		//char uuidValueYear[3] = { 0 };
		//uuidValueYear[0] = uuidValue[0];
		//uuidValueYear[1] = uuidValue[1];
		//uuidValueYear[2] = '\0';
		
		// 机种号
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

		// 读取配置文件
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
		
		// 获取key为uuid_year的键值，以下同理
		//年份校验
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
		// 产量判断
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

		// 鉴权uuid
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
	*		向Reciever注册的UUID同期Callback函数
	*	@param [in] data
	*		UUID字符串(半角)
	*	@return
	*		void
	*/
	void AdditionServiceEngine_SgnUUIDChanged(void* data)
	{
		// add by liubo
		// uuid必须是第一条消息，故这里不需要判断其与标志位

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

		//发送消息
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return NULL;
		}
		paramData->funcId = FuncID_Send_ReqUsbStatus;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);

		////等待同期返回
		MapbarWaitState state = MapbarWaitState_failed;
		state = Mapbar_waitEventWithTimeout(m_addionSvcUsbEvent, 1000); // 等待1秒
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
	///*===========================3.18 倒车状态============================*/

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

	//	int retryNum = 3;  // retry 3次
	//	MapbarWaitState state = MapbarWaitState_failed;
	//	do
	//	{
	//		SendData* paramData = (SendData*)malloc(sizeof(SendData));
	//		paramData->funcId = FuncID_Send_getRevStatus;
	//		paramData->data = NULL;
	//		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	//		state = Mapbar_waitEventWithTimeout(m_addionSvcRevEvent, 1000); // 等待1s
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
	///*===========================3.18 倒车状态============================*/

	/*==============================3.8 WIFI==============================*/
	/**
	*	@brief
	*		获取wifi状态
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
	*		wifi状态变化通知
	*	@param [in] EN_ADDITIONSVCENGINE_WIFI_STATE
	*		data 状态信息
	*	@return void
	*/
	void AdditionSvcEngine::wifiStatusChangedNotify(EN_ADDITIONSVCENGINE_WIFI_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onWifiStatusChangedNotify(data);
		return;
	}

	// 通知函数
	void wifiStatusChangedNotify(void* param)
	{
		UNUSED_VAR(param);
		AdditionSvcEngine::getInstance()->wifiStatusChangedNotify(AdditionSvcEngine::getInstance()->m_wifiStatus);
		return;
	}

	/**
	*	@brief
	*		向Reciever注册的WIFI连接状态同期Callback函数
	*	@param [in] data
	*		WIFI连接状态
	*	@return
	*		void
	*/
	void AdditionSvcEngine_WifiStatusChanged(void* data)
	{
		UNUSED_VAR(data);
		//获取单例
		AdditionSvcEngine* ins = AdditionSvcEngine::getInstance();
		//判断network状态
		if (ins->m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_WIFICONNECTED)
		{
			ins->m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_WIFICONNECTED;
		}
		else if (ins->m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED || ins->m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_DCMCONNECTED)
		{
			ins->m_wifiStatus = EN_ADDITIONSVCENGINE_WIFI_STATE_WIFIDISCONNECTED;
		}

		CQ_LOG_INFO("[ServiceManager]AdditionSvcEngine_WifiStatusChanged() m_wifiStatus is %d\n", ins->m_wifiStatus);
		//在主线程中执行通知函数
		Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)wifiStatusChangedNotify, NULL);
		
		return;
	}
	/*==============================3.8 WIFI==============================*/
	/*============================3.19 WIFI/DCM===========================*/
	/**
	*	@brief
	*		获取WIFI/DCM状态
	*	@param [in] 
	*		void
	*	@return
	*	 	EN_ADDITIONSVCENGINE_NETWORK_STATE
	*/
	EN_ADDITIONSVCENGINE_NETWORK_STATE AdditionSvcEngine::getNetworkStatus(void)
	{
		//判断是否有本地数据，若有本地数据直接返回本地数据
		if (EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID == m_networkStatus)
		{
			m_networkStatusEvent = Mapbar_createEvent(FALSE, FALSE);

			int retryNum = 3;  // retry 3次
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
				state = Mapbar_waitEventWithTimeout(m_networkStatusEvent, 1000); // 等待1s
				if (state == MapbarWaitState_ok)
					break;
				if (state == MapbarWaitState_timeout)
				{
					CQ_LOG_INFO("[ServiceManager]getNetworkStatus() timeout\n");
					//此处调用Mapbar_setEvent时，下次Mapbar_waitEventWithTimeout不生效。
					//Mapbar_setEvent(m_networkStatusEvent);
					continue;
				}
			} while (--retryNum);
			Mapbar_destroyEvent(m_networkStatusEvent);
			m_networkStatusEvent = invalidHandle;
		}
		//返回network状态
		CQ_LOG_INFO("[ServiceManager]getNetworkStatus() NetworkStatus is %d\n", m_networkStatus);
		if (m_networkStatus == EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID)
		{
			return EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED;
		}
		return m_networkStatus;
	}

	/**
	*	@brief
	*		wifi/dcm状态变化通知
	*	@param [in] EN_ADDITIONSVCENGINE_NETWORK_STATE
	*		data 状态信息
	*	@return void
	*/
	void AdditionSvcEngine::AdditionSvcEngine_networkStatusChangedNotify(EN_ADDITIONSVCENGINE_NETWORK_STATE data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onNetworkStatusChangedNotify(data);
		return;
	}


	// 通知函数
	void networkStatusChangedNotify(void* data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_networkStatusChangedNotify(AdditionSvcEngine::getInstance()->m_networkStatus);
		return;
	}

	/**
	*	@brief
	*		向Reciever注册的WIFI/DCM连接状态同期Callback函数
	*	@param [in] data
	*		WIFI/DCM连接状态
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

		ins->m_networkStatus = *(EN_ADDITIONSVCENGINE_NETWORK_STATE*)data;  // 保存wifi/dcm状态到本地
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
	/*===========================3.19 GBOOK信息===========================*/
	/**
	*	@brief
	*		获取G-BOOK信息
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
			int retryNum = 3;  // retry 3次
			MapbarWaitState state = MapbarWaitState_failed;

			do
			{
				//发送请求消息
				ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
				if (paramData == NULL)
				{
					return NULL;
				}
				paramData->funcId = ServiceManager::FuncID_Send_getGbookInfo;
				paramData->data = NULL;
				NaviSysSender::getInstance()->sendMsg(paramData, LOW_PRIO);
				state = Mapbar_waitEventWithTimeout(m_gbookInfoEvent, 1000); // 等待1s

				if (state == MapbarWaitState_ok)
					break;
				if (state == MapbarWaitState_timeout || m_gbookInfo[0] == '\0')
				{
					CQ_LOG_INFO("[ServiceManager]getGbookInfo() timeout\n");
					//此处调用Mapbar_setEvent时，下次Mapbar_waitEventWithTimeout不生效。
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
	*		G-BOOK变化通知
	*	@param [in] 
	*		data G-BOOK信息
	*	@return void
	*/
	void AdditionSvcEngine::AdditionSvcEngine_gbookInfoChangedNotify(char* data)
	{
		std::set<AdditionSvcEngineListener*>::iterator iter;

		for (iter = getListerners()->begin(); iter != getListerners()->end(); ++iter)
			((AdditionSvcEngineListener*)*iter)->onGbookInfoChangedNotify(data);
		return;
	}


	// 通知函数
	void gbookInfoChangedNotify(void *data)
	{
		UNUSED_VAR(data);
		AdditionSvcEngine::getInstance()->AdditionSvcEngine_gbookInfoChangedNotify(AdditionSvcEngine::getInstance()->m_gbookInfo);
		return;
	}

	/**
	*	@brief
	*		向Reciever注册的G-BOOK信息同期Callback函数
	*	@param [in] data
	*		G-BOOK信息
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
	/*============================3.19 G-BOOK信息==========================*/

/*=============================4.0.2 设备插拔通知===============================*/
	DeviceInfo* AdditionSvcEngine::getDeviceInfo()
	{
		CQ_LOG_INFO("[ServiceManager] AdditionSvcEngine_ReqUsbStatus\n");
		if (m_deviceInfo.connectType != EN_ADDITONSVCENGINE_DEVICE_STATE_INVALID)
		{
			return &m_deviceInfo;
		}
		//event
		m_deviceInfoEvent = Mapbar_createEvent(FALSE, FALSE);

		//发送消息
		SendData* paramData = (SendData*)malloc(sizeof(SendData));
		if (paramData == NULL)
		{
			return NULL;
		}
		paramData->funcId = FuncID_Send_getDeviceConnect;
		paramData->data = NULL;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);

		//等待同期返回
		//MapbarWaitState state = MapbarWaitState_failed;
		//state = Mapbar_waitEventWithTimeout(m_deviceInfoEvent, 1000); // 等待1秒

		Mapbar_waitEventWithTimeout(m_deviceInfoEvent, 1000); // 等待1秒

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
/*=============================4.0.2 设备插拔通知===============================*/

/*=============================4.0.1 开始手车连携===============================*/
	void AdditionSvcEngine::startPhoneConnectNotify(bool data)
	{
		//发送消息
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
/*=============================4.0.1 开始手车连携===============================*/
/*===========================4.0.1 开始手车连携反馈===============================*/
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
/*===========================4.0.1 开始手车连携反馈===============================*/
/*===========================4.0.3 手车连携状态通知=============================*/
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
/*===========================4.0.3 手车连携状态通知=============================*/
	/*===============================德赛=================================*/
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
		MapbarWaitState state = Mapbar_waitEventWithTimeout(m_GPSUpdateTimeEvent, 1000);//等待1秒
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
	*		设置清除数据状态
	*	@param [in] BOOL
	*	@return void
	*/
	void AdditionSvcEngine::setMemoryClearFlag(BOOL isRevMemoryClear)
	{
		m_isReceiveMemoryClear = isRevMemoryClear;
	}
	
	/**
	*	@brief
	*		获取清除数据状态
	*	@param [in] void
	*	@return BOOL
	*/
	BOOL AdditionSvcEngine::getMemoryClearFlag()
	{
		return m_isReceiveMemoryClear;
	}

	/**
	*	@brief
	*		向Reciever注册GPS更新时间是否可用的Callback函数
	*	@param [in] data
	*		是否可用
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
	*		向Reciever注册Callback函数,当sd card状态变化时回调
	*	@param [in] data
	*		SD card状态
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
		// Receiver中进行释放
		/*
		if (NULL != data)
		{
			free(data);
			data = NULL;
		}
		*/
	}
	/*===============================德赛=================================*/

}
