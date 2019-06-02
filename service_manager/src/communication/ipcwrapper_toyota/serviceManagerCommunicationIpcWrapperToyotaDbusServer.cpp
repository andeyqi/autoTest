#include "stdafx_serviceManager.h"
#include "serviceManagerCommunicationIpcWrapperToyotaDbusServer.h"
#include "communication/serviceManagerOutput.h"
//add by shenghao
ToyotaDbusService* ToyotaDbusService::m_instatce = NULL;


namespace ServiceManager
{
	
	void* monitorSignal(void* pdata);
	
	/**
	*	@brief
	*		创建对象
	*	@return
	*		ToyotaDbusService指针 
	*	@param pdata
	*		NULL
	*/
	ToyotaDbusService* ToyotaDbusService::getInstance()
	{
		try
		{
			if (NULL == m_instatce)
			{
				m_instatce = new ToyotaDbusService();
			}
			return m_instatce;
			//delete by shenghao
			//static ToyotaDbusService m_instatce;
			//CQ_LOG_INFO("[ServiceManager]Construct dbus service instance.\n");;
			//return &m_instatce;
		}
		catch (const char* e)
		{
			CQ_LOG_ERROR("[ServiceManager]Construct dbus service instance failed! %s.\n", e);
			Mapbar_drclog_printf("sm-dbus getInstance failed! %s.\n", e);
			return NULL;
		}
	}

	void ToyotaDbusService::releaseInstance()
	{
		if (NULL != m_instatce)
		{
			delete m_instatce;
			m_instatce = NULL;
		}
		return;
	}
	
	/**
	*	@brief
	*		建立Debus通信
	*	@param 
	*		NULL
	*/
	void ToyotaDbusService::initDbusConnect()
	{
		ServiceManager::NaviData naviData;	
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "ToyotaDbusService::initDbusConnect start!");
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
		// 追加DRC LOG
		Mapbar_drclog_printf("sm-dbus initDbusConnect start!\n");

		//reInitDbusConnect();
		mpConnection = getDBusConnection((const cqCHAR*)TOYOTA_DBUS_BUS_NAME_NAVIGATION, NULL);
		
		//步骤4:新线程监听收到的signal
		int retryNum = 3;  // retry 3次
		bool isConnected = false;
		do
		{
			CQ_LOG_INFO("[ServiceManager]DBus Step4 try number = %d!\n", retryNum);
			// 这里给出的是连接中获取任何消息的方式，所以获取后去检查一下这个消息是否我们期望的信号，并获取内容。
			// 我们也可以通过这个方式来获取method call消息。
			if (0 != pthread_create(&mTthread, NULL, monitorSignal, (void*)this))
			{
				CQ_LOG_ERROR("[ServiceManager]DBus create thread failed!\n");
				Mapbar_drclog_printf("sm-dbus initDbusConnect create thread failed!\n");
			}
			else
			{
				isConnected = true;
				break;
			}

		} while (--retryNum);
		
		CQ_ASSERT(isConnected);
		if (!isConnected)
		{
			cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "ToyotaDbusService::initDbusConnect pthread_create error!");
			ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
			Mapbar_drclog_printf("sm-dbus initDbusConnect pthread_create error!\n");
			return;
		}
	}
	
	/**
	*	@brief
	*		添加listener
	*/
	void ToyotaDbusService::addListener(ToyotaDbusServiceListener* listener)
	{
		CQ_LOG_INFO("[ServiceManager] ToyotaDbusService::addListener.\n");
		if (NULL != listener){
			mpListener.push_back(listener);
		}
		return;
	}
		
	/**
	*	@brief
	*		移除listener
	*/
	void ToyotaDbusService::removeListener(ToyotaDbusServiceListener* listener)
	{
		CQ_LOG_INFO("[ServiceManager] ToyotaDbusService::removeListener.\n");
		if (NULL != listener){
			cqstd::vector<ToyotaDbusServiceListener*>::iterator iter;
			for (iter = mpListener.begin(); iter != mpListener.end(); ++iter)
			{
				if ((ToyotaDbusServiceListener*)*iter == listener)
				{
					mpListener.erase(iter);
					break;
				}
			}
		}
		return;
	}
	
	/**
	*	@brief
	*		发送signal
	*	@return
	*		true 发送成功； false 发送失败
	*	@param signalValue
	*		[in] signal值
	*/
	bool ToyotaDbusService::emitSignal(const cqCHAR* signalValue)
	{
		// file log
		ServiceManager::NaviData naviData;
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, signalValue);
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
		uint32 msg_id = 0;
		cqCHAR buff_temp[256] = { 0 };
		sscanf(signalValue, "%x:%s", &msg_id, buff_temp);

		switch (msg_id)
		{
		case 0x10000041:
		case 0x10000044:
		case 0x10000008:
			break;
		default:
			Mapbar_drclog_printf("sm-send msg: %s", signalValue);
			break;
		}

		// 系统LOG输出
		// fs_drc_printf(SYS_ERR_LVL_LIGHT, 0, "Send:%s", signalValue);
		CQ_LOG_INFO("[ServiceManaer] Send:%s .\n", signalValue);

			// 获取连接当前是否打开   返回false和true
			if (false == dbus_connection_get_is_connected(mpConnection)) {
				
				ServiceManager::NaviData naviData;
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "dbus_connection_get_is_connected error! maybe mpconnection disconnected!");
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
				Mapbar_drclog_printf("sm-dbus emitSignal DBus connected error! maybe disconnected!\n");
				//重连
				ToyotaDbusService::disConnectConnection(mpConnection, (const cqCHAR*)TOYOTA_DBUS_BUS_NAME_NAVIGATION, NULL);
				mpConnection = ToyotaDbusService::getDBusConnection((const cqCHAR*)TOYOTA_DBUS_BUS_NAME_NAVIGATION, NULL);

				CQ_LOG_ERROR("[ServiceManaer]Function:%s, Line:%d, DBus Connection is error.\n", __FUNCTION__, __LINE__);
				Mapbar_drclog_printf("sm-dbus emitSignal DBus connected error! connect agine done !\n");
			}
		
			//步骤3:发送一个信号
			//给出这个信号的路径（即可以指向对象），接口，以及信号名，创建一个Message
			DBusMessage* message;
			if ((message = dbus_message_new_signal(TOYOTA_DBUS_OBJECT_PATH_NAVIGATION, TOYOTA_DBUS_INTERFACE_NAVIGATION, TOYOTA_DBUS_SIGNAL_NAME_NAVIGATION)) == NULL){
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "[dbus_message_new_signal]Lack of Memory!");
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);

				CQ_LOG_ERROR("[ServiceManaer]Message construct failed!\n");
				Mapbar_drclog_printf("sm-dbus emitSignal DBus Message construct failed!\n");
				return false;
			}
			//给这个信号（messge）具体的内容
			//迭代子用来在信号（message）最后追加参数值用
			DBusMessageIter mArg;			// 迭代子
			dbus_message_iter_init_append(message, &mArg);
			// 追加参数值到消息（message）中
			if (!dbus_message_iter_append_basic(&mArg, DBUS_TYPE_STRING, &signalValue)){
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "[dbus_message_iter_append_basic] Lack of Memory!");
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);

				CQ_LOG_ERROR("[ServiceManaer]Dbus Message Out Of Memory!\n");
				Mapbar_drclog_printf("sm-dbus emitSignal DBus Message Out Of Memory!\n");
				return false;
			}

			//步骤4: 将信号从连接中发送
			if (!dbus_connection_send(mpConnection, message, NULL)){
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "[dbus_connection_send] Lack of Memory!");
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);

				CQ_LOG_ERROR("[ServiceManager]Function:%s, Line:%d, DBus Send Message Failed.\n", __FUNCTION__, __LINE__);
				Mapbar_drclog_printf("sm-dbus emitSignal DBus Send Message Failed.\n");
				return false;
			}
			dbus_connection_flush(mpConnection);
			CQ_LOG_INFO("[ServiceManager]Func:%s,Line:%d,Msg:DBus Signal Send:%s.\n", __FUNCTION__, __LINE__, signalValue);

			//步骤5: 释放相关的分配的内存。
			dbus_message_unref(message);

		return true;
	}

	/**
	*	@brief
	*		初始化dbus连接
	*	@return
	*		NULL
	*	@param pdata
	*		NULL
	*/
	ToyotaDbusService::ToyotaDbusService() :mpConnection(NULL), mFlag(1)
	{		
		
	}

	/**
	*	@brief
	*		析构
	*/
	ToyotaDbusService::~ToyotaDbusService()
	{
		ServiceManager::NaviData naviData;
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "ToyotaDbusService ::~ToyotaDbusService called!");
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);

		disConnectConnection(mpConnection, (const cqCHAR*)TOYOTA_DBUS_BUS_NAME_NAVIGATION, NULL);
		mpConnection = NULL;
	
		// 监听线程退出
		mFlag = 0;
		pthread_join(mTthread, NULL);
		CQ_LOG_INFO("[ServiceManager] ~ToyotaDbusService.\n");
	}
	
	/**
	*	@brief
	*		获取一个dbus链接
	*	@return
	*		dbus 链接
	*	@param pdata
	*		NULL
	*/
	DBusConnection* ToyotaDbusService::getDBusConnection(const cqCHAR* connectionName, const cqCHAR* matchName)
	{
		ServiceManager::NaviData naviData;	
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "ToyotaDbusService::getDBusConnection start!");
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
		
		cqCHAR failLog[DBUS_MAX_MSG_SIZE] = {0};
		DBusError mError;
		
		DBusConnection* connection;	// 连接	
		//步骤1:建立与D-Bus后台的连接
		bool isConnected = false;
		int retryNum = 3;  // retry 3次
		do
		{
			CQ_LOG_INFO("[ServiceManager] getDBusConnection[%s] DBus Step1 try number = %d!\n", connectionName==NULL? "NULL" : connectionName, retryNum);
			dbus_error_init(&mError);
			connection = dbus_bus_get(TOYOTA_DBUS_SESSION, &mError);
			if (dbus_error_is_set(&mError)){
				CQ_LOG_ERROR("[ServiceManager] getDBusConnection[%s] DBus connection failed! %s\n", connectionName==NULL? "NULL" : connectionName, mError.message);
				Mapbar_drclog_printf("sm-dbus getDBusConnection DBus connection failed!.\n");
				cq_sprintf(failLog, "[ServiceManager] getDBusConnection[%s] DBus connection failed! %s", connectionName==NULL? "NULL" : connectionName, mError.message);
				
				dbus_error_free(&mError);
			}
			if (NULL == connection){
				//mFlag = 0;
				//throw "DBus connection failed!\n";
			}
			else
			{
				//mFlag = 1;
				//add by sh  设置当连接收到断开信号时是否应该调用_exit() 
				dbus_connection_set_exit_on_disconnect(connection, FALSE);
				isConnected = true;
				break;
			}
		} while (--retryNum);
		
		CQ_ASSERT(isConnected);
		if (!isConnected)
		{
			cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, failLog);
			ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
			return NULL;
		}

		if (connectionName != NULL)
		{
			//步骤2:给连接名分配一个可记忆名字com.fujitsuten.navigation作为Bus name，这个步骤不是必须的,但推荐这样处理
			retryNum = 3;  // retry 3次
			isConnected = false;
			do
			{
				int mRet;
				CQ_LOG_INFO("[ServiceManager]DBus[%s] Step2 try number = %d!\n", connectionName==NULL? "NULL" : connectionName, retryNum);
				mRet = dbus_bus_request_name(connection, connectionName, TOYOTA_DBUS_NAME_FLAG_REPLACE_EXISTING, &mError);
				if (dbus_error_is_set(&mError)){
					CQ_LOG_ERROR("[ServiceManager]getDBusConnection[%s] Request DBus Bus Name failed! %s\n", connectionName==NULL? "NULL" : connectionName, mError.message);
					Mapbar_drclog_printf("sm-dbus getDBusConnection DBus Name failed!.\n");
					cq_sprintf(failLog, "[ServiceManager]getDBusConnection[%s] Request DBus Bus Name failed! %s", connectionName==NULL? "NULL" : connectionName, mError.message);
					dbus_error_free(&mError);
				}
				if (mRet == TOYOTA_DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
				{
					isConnected = true;
					break;
				}
					//throw "DBus bus name error.\n";
			} while (--retryNum);
			CQ_ASSERT(isConnected);
			if (!isConnected)
			{
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, failLog);
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
				
				dbus_connection_unref(connection);
				return NULL;
			}
		}
		
		if (matchName != NULL)
		{
			//步骤3:通知D-Bus daemon，希望监听来行接口fujitsuten.signal.wrapper的信号
			retryNum = 3;  // retry 3次
			isConnected = false;
			do
			{
				CQ_LOG_INFO("[ServiceManager] getDBusConnection[%s] DBus Step3 try number = %d!\n", connectionName==NULL? "NULL" : connectionName, retryNum);
				dbus_bus_add_match(connection, matchName, &mError);
				//实际需要发送东西给daemon来通知希望监听的内容，所以需要flush
				dbus_connection_flush(connection);
				if (dbus_error_is_set(&mError)){
					CQ_LOG_ERROR("[ServiceManager]getDBusConnection[%s] DBus match error. %s\n", connectionName==NULL? "NULL" : connectionName, mError.message);
					Mapbar_drclog_printf("sm-dbus getDBusConnection DBus match error.\n");
					cq_sprintf(failLog, "[ServiceManager]getDBusConnection[%s] DBus match error. %s", connectionName==NULL? "NULL" : connectionName, mError.message);
					dbus_error_free(&mError);
				}
				else
				{
					isConnected = true;
					break;
				}
			} while (--retryNum);
			
			CQ_ASSERT(isConnected);
			if (!isConnected)
			{
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, failLog);
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
				
				dbus_bus_release_name(connection, connectionName, &mError);
				dbus_connection_unref(connection);
				return NULL;
			}
		}
		
		return connection;
	}
	
	void ToyotaDbusService::disConnectConnection(DBusConnection* connection, const cqCHAR* connectionName, const cqCHAR* matchName)
	{
		if (NULL != connection)
		{
			ServiceManager::NaviData naviData;
			cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "ToyotaDbusService ::disConnectConnection start!");
			ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
		
			if (matchName != NULL)
			{
				// D-Bus 清除Match
				dbus_bus_remove_match(connection, matchName, NULL);
			}
			if (connectionName != NULL)
			{
				// D-Bus 释放Name
				dbus_bus_release_name(connection, connectionName, NULL);
			}
			// D-Bus 关闭连接
			//dbus_connection_close(connection);
			// this is a shared connection. use dbus_connection_unref instead of dbus_connection_close

			dbus_connection_unref(connection);

		}
	}
	
	/**
	*	@brief
	*		监听车机侧的signal
	*	@return
	*		NULL
	*	@param [in]pdata
	*		NULL
	*/
	void* monitorSignal(void* pdata)
	{
		if (NULL == pdata)
		{
			CQ_LOG_ERROR("[ServiceManager]DBus connection is NULL!\n");
			Mapbar_drclog_printf("sm-dbus monitorSignal pdata == NULL.\n");
			return NULL;
		}
		//DBusConnection* pConnection = ((ToyotaDbusService*)pdata)->mpConnection;
		DBusConnection* pConnection = ToyotaDbusService::getDBusConnection(NULL, "type='signal',interface='fujitsuten.signal.wrapper'");
		ToyotaDbusService::getInstance()->mFlag = pConnection != NULL ? 1 : 0;
		
		DBusMessage* pMessage = NULL;
		DBusMessageIter arg;
		char* signalValue = NULL;
	
		while (ToyotaDbusService::getInstance()->mFlag){
			// 只要“连接”没有断，这个函数会阻塞到“连接”可以读写的时候，这时候返回true。如果“连接”断开，返回false
			if (false == dbus_connection_read_write(pConnection, 0)) {
				
				ServiceManager::NaviData naviData;
				cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "dbus_connection_read_write error! maybe connection disconnected!");
				ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);
				Mapbar_drclog_printf("sm-dbus monitorSignal connection error! maybe disconnected.\n");

				ToyotaDbusService::disConnectConnection(pConnection, NULL, "type='signal',interface='fujitsuten.signal.wrapper'");
				pConnection = ToyotaDbusService::getDBusConnection(NULL, "type='signal',interface='fujitsuten.signal.wrapper'");
				
				ToyotaDbusService::getInstance()->mFlag = pConnection != NULL ? 1 : 0;
				Mapbar_drclog_printf("sm-dbus monitorSignal DBus connect again.\n");
				continue;
				CQ_LOG_ERROR("[ServiceManaer]Function:%s, Line:%d, DBus Connection is error.\n", __FUNCTION__, __LINE__);
				//return NULL;
			}

			// 返回消息队列第一条消息。如果消息队列空，返回NULL
			pMessage = dbus_connection_pop_message(pConnection);
			if(pMessage == NULL){
				usleep(100000);
				continue;
			}

			// 判断是否是我们监听的信号
			if(dbus_message_is_signal(pMessage, TOYOTA_DBUS_INTERFACE_WRAPPER, TOYOTA_DBUS_SIGNAL_NAME_WRAPPER)){
				if (!dbus_message_iter_init(pMessage, &arg))
				{
					Mapbar_drclog_printf("sm-dbus monitorSignal Message Has no Param.\n");
					CQ_LOG_ERROR("[ServiceManaer]Function:%s, Line:%d, Message Has no Param.\n", __FUNCTION__, __LINE__);
					continue;
				}	
				else if (dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING)
				{
					Mapbar_drclog_printf("sm-dbus monitorSignal Param is not string.\n");
					CQ_LOG_ERROR("[ServiceManaer]Function:%s, Line:%d, Param is not string.\n", __FUNCTION__, __LINE__);
					continue;
				}			
				else
					dbus_message_iter_get_basic(&arg, &signalValue);  // 从迭代子中获取参数值

				CQ_LOG_INFO("[ServiceManager]Dbus got signal %s.\n", signalValue);

				// 系统LOG输出
				// fs_drc_printf(SYS_ERR_LVL_LIGHT, 1, "Recv:%s", signalValue);
				CQ_LOG_INFO("[ServiceManaer] Recv:%s .\n", signalValue);

				// 消息通知给导航
				cqstd::vector<ToyotaDbusServiceListener*>::iterator iter;
				for (iter = ToyotaDbusService::getInstance()->mpListener.begin(); iter != ToyotaDbusService::getInstance()->mpListener.end(); ++iter)
					(*iter)->notifyMsg(signalValue);

			}
			// 减少message的引用
			dbus_message_unref(pMessage);
		}//End of while
		
		ToyotaDbusService::disConnectConnection(pConnection, NULL, "type='signal',interface='fujitsuten.signal.wrapper'");
		
		return NULL;
	}
	


	

	


}