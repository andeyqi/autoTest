#include "stdafx_serviceManager.h"
#include "serviceManagerOutput.h"
#include <typeinfo>
//=====================================================//
NaviSysReceiver* NaviSysReceiver::mpInstance = NULL;
static Handle m_mutex = invalidHandle;
static bool m_shouldExit = false;
static Handle m_threadHandle = invalidHandle;
#define ReceiveThread	"SM_ReceiveThread"

using namespace ServiceManager;

/**
*	@brief
*		构造函数
*/
NaviSysReceiver::NaviSysReceiver()
{
	CQ_LOG_INFO("[ServiceManager] NaviSysReceiver.\n");
	m_mutex = Mapbar_createMutex();
	NaviSysReceiver::naviServiceClearReceiveList();
	NaviSysReceiver::startReceiverMessageProcess();
	// init m_CallBackArray
	for (size_t index = 0; index < FuncID_Receive_Max; index++)
	{
		m_CallBackArray[index].callback = NULL;
		m_CallBackArray[index].isRealTime = false;
	}
	// add by renxs
	#ifdef MAPBAR_WIN32
	m_ipcWrapper = new NaviSysIPCWrapperWin32();	//NaviSysIPCWrapperWin32();
	#elif MAPBAR_DESAY
	m_ipcWrapper = new NaviSysIPCWrapperDeSay();
	#elif MAPBAR_TOYOTA
	m_ipcWrapper = new NaviSysIPCWrapperToyota();
	#else
	#endif
	m_ipcWrapper->addListener(this);
	return;
}

/**
*	@brief
*		析构函数
*/
NaviSysReceiver::~NaviSysReceiver()
{
	CQ_LOG_INFO("[ServiceManager] ~NaviSysReceiver.\n");
	// 释放IPCWrapper
	m_ipcWrapper->removeListener();
	delete(m_ipcWrapper);
	m_ipcWrapper = NULL;
	// 清空消息队列
	naviServiceClearReceiveList();
	// 关闭Receiver线程
	if (m_threadHandle != invalidHandle)
	{
		m_shouldExit = TRUE;
		Mapbar_waitThread(m_threadHandle);
		Mapbar_closeThread(m_threadHandle);
		m_threadHandle = invalidHandle;
	}
	Mapbar_destroyMutex(m_mutex);
	return;
}

/**
*	@brief
*		将消息放入消息队列
*	@return
*		void
*	@param
*		false ,failed, true success
*/
bool NaviSysReceiver::sendMsgQuene(void *data, int priority)
{
	UNUSED_VAR(priority);

	Mapbar_lockMutex(m_mutex);

	m_receiveList.push_back((ReceiveData*)data);

	Mapbar_unlockMutex(m_mutex);
	
	return true;
}

/**
*	@brief
*		获取NaviSysReceiver单例对象
*	@return
*		NaviSysReceiver
*/
NaviSysReceiver *NaviSysReceiver::getInstance()
{
	/*!< check if mpInstance is initialized or not  */
	if (NULL == mpInstance)
	{
		mpInstance = new NaviSysReceiver();
	}
	return mpInstance;
}

/**
*	@brief
*		释放NaviSysReceiver单例对象
*	@return
*		void
*/

/**
*	@brief
*		注册异步回调函数
*	@return
*		void
*	@param
*		funcId 函数ID, *pfun 回调函数指针
*/
//		delete mpInstance;
//		mpInstance = NULL;
//	}
//	return;
//}

/**
*	@brief
*		注册异步回调函数
*	@return
*		void
*	@param
*		funcId 函数ID, *pfun 回调函数指针
*/
void NaviSysReceiver::setCallback(int funcId, ReceiverCallback pfun){	//void* pfun void(*pfun)(void *data)
	CQ_LOG_INFO("[ServiceManager] NaviSysReceiver setCallback.\n");
	if (NULL != pfun && funcId < FuncID_Receive_Max)
	{
		// Test print funcId and func point
		ServiceManager::NaviData naviData;
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "setCallback");
		naviData.ReceInfo.infoId = NaviDataType_rev;
		naviData.ReceInfo.receFuncId = funcId;
		naviData.ReceInfo.callBackAdd = pfun;
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_rev, &naviData);

		m_CallBackArray[funcId].callback = (ReceiverCallback)pfun;
		m_CallBackArray[funcId].isRealTime = false;
	}
	return;
}

/**
*	@brief
*		注册同步回调函数
*	@return
*		void
*	@param
*		funcId 函数ID, *pfun 回调函数指针
*/
void NaviSysReceiver::setRealTimeCallback(int funcId, ReceiverCallback pfun){
	CQ_LOG_INFO("[ServiceManager] NaviSysReceiver setRealTimeCallback.\n");
	if (NULL != pfun && funcId < FuncID_Receive_Max)
	{
		// Test print funcId and func point
		ServiceManager::NaviData naviData;
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "setRealTimeCallback");
		naviData.ReceInfo.infoId = NaviDataType_rev;
		naviData.ReceInfo.receFuncId = funcId;
		naviData.ReceInfo.callBackAdd = pfun;
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_rev, &naviData);

		m_CallBackArray[funcId].callback = (ReceiverCallback)pfun;
		m_CallBackArray[funcId].isRealTime = true;
	}
	return;
}

void NaviSysReceiver::messageListener(void *data)
{
	if (NULL == data)
		return;

	ReceiveData* paramData = (ReceiveData*)data;

	// 如果FuncID大于Max
	if (!(paramData->funcId < FuncID_Receive_Max))
	{
		ServiceManager::NaviData naviData;
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "ERROR paramData funcId > FuncID_Receive_Max");
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_debus, &naviData);

		if (NULL != paramData)
		{
			free(paramData);
			paramData = NULL;
		}			
		return;
	}

	//printf("m_CallBackArray[%d].callback = %p \n", paramData->funcId, m_CallBackArray[paramData->funcId].callback);

	// Test print funcId and func point
	ServiceManager::NaviData naviData;
	cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "messageListener");
	naviData.ReceInfo.infoId = NaviDataType_rev;
	naviData.ReceInfo.receFuncId = paramData->funcId;
	naviData.ReceInfo.callBackAdd = m_CallBackArray[paramData->funcId].callback;
	ServiceManagerOutput::getInstance()->outputLog(NaviDataType_rev, &naviData);

	if (m_CallBackArray[paramData->funcId].isRealTime)
	{
		if (m_CallBackArray[paramData->funcId].callback)
		{
			if (typeid(*paramData) == typeid(ReceiveData) && typeid(m_CallBackArray[paramData->funcId].callback) == typeid(ReceiverCallback))
				m_CallBackArray[paramData->funcId].callback(paramData->data);
		}

		// add by liubo
		// 1、不在队列中的消息在此处进行释放。
		// 2、队列中的消息ipcWrapperProcess中进行释放。
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
	}
	else
	{
		// add by liubo
		// 队列中的消息ipcWrapperProcess中进行释放
		sendMsgQuene(data, LOW_PRIO);
	}
	return;
}

/**
*	@brief
*		判断同、异步属性执行回调函数
*	@return
*		void
*	@param
*		data 回调携带参数
void NaviSysReceiver::callbackProc(void *data)
{
	ReceiveData* paramData = (ReceiveData*)data;
	if (m_CallBackArray[paramData->funcId].isRealTime)
	{
		m_CallBackArray[paramData->funcId].callback(paramData->data);
	}
	else
	{
		sendMsgQuene(data, LOW_PRIO);
	}
	return;
}
*/

/**
*	@brief
*		执行回调函数
*	@return
*		void
*	@param
*		data 回调参数
*/
void NaviSysReceiver::doAsynCallback(ReceiveData *paramData)
{
	// add by renxs 2017.10.13 添加非空判断
	if (m_CallBackArray[paramData->funcId].callback)
	{
		m_CallBackArray[paramData->funcId].callback(paramData->data);
	}
	return;
}

/**
*	@brief
*		从消息队列中获取msg
*	@return
*		Data msg
*/
ReceiveData* NaviSysReceiver::naviServiceGetReceive()	//static
{
	Mapbar_lockMutex(m_mutex);
	ReceiveData* msg = m_receiveList.front();
	m_receiveList.pop_front();
	Mapbar_unlockMutex(m_mutex);
	return msg;
}

/**
*	@brief
*		判断消息队列是否有msg
*	@return
*		bool true：有 false：无
*/
bool NaviSysReceiver::naviServiceHasReceive()	//static
{
	Mapbar_lockMutex(m_mutex);

	int size = m_receiveList.size();

	Mapbar_unlockMutex(m_mutex);

	return (size != 0);
}

/**
*	@brief
*		清除消息队列
*	@return
*		bool
*/
bool NaviSysReceiver::naviServiceClearReceiveList()	//static 
{
	Mapbar_lockMutex(m_mutex);

	if (!naviServiceHasReceive())
	{
		Mapbar_unlockMutex(m_mutex);
		return true;
	}

	std::list<ReceiveData*>::iterator itor = m_receiveList.begin();

	while (itor != m_receiveList.end())
	{
		free(*itor);
		itor++;
	}

	m_receiveList.clear();

	Mapbar_unlockMutex(m_mutex);

	return true;
}

/**
*	@brief
*		执行回调
*/
int receiverMessageProcess(void* para)
{
	NaviSysReceiver* receiver = (NaviSysReceiver*)para;
	
	if (m_threadHandle != invalidHandle)
		Mapbar_setThreadName(m_threadHandle, ReceiveThread);

	while (!m_shouldExit)
	{
		if (!receiver->naviServiceHasReceive())
		{
			Mapbar_sleep(100);
			//printf("Receive print 1 \n");
			continue;
		}
		ReceiveData* request = receiver->naviServiceGetReceive();				
		//call ServiceManager engine/client callback Function
		//Test print funcID and func pointer by renxs
		ServiceManager::NaviData naviData;
		cq_strcpy_s(naviData.DebusInfo.debusMessage, DBUS_MAX_MSG_SIZE, "receiverMessageProcess");
		naviData.ReceInfo.infoId = NaviDataType_rev;
		naviData.ReceInfo.receFuncId = request->funcId;
		naviData.ReceInfo.callBackAdd = receiver->m_CallBackArray[request->funcId].callback;
		ServiceManagerOutput::getInstance()->outputLog(NaviDataType_rev, &naviData);

		// TYTAVNNEW-281 Modify by Renxs 20180521
		if (NULL != request && NULL != receiver && typeid(*receiver) == typeid(NaviSysReceiver) && typeid(*request) == typeid(ReceiveData))
		{
			receiver->doAsynCallback(request);
		}
		else
		{
			CQ_LOG_ERROR("[ServiceManager]:Receiver illegal data request or receiver unavailable .\n");
		}
				
		// free data is need ?
		if (request != NULL)
		{
			if (request->data != NULL)
			{
				free(request->data);
				request->data = NULL;
			}
			free(request);
			request = NULL;
		}
	};
	return D_NAVISYS_OK;
}

/**
*	@brief
*		启动Recever消息处理线程
*	@return
*		void
*/
void  NaviSysReceiver::startReceiverMessageProcess()
{
	m_threadHandle = Mapbar_createThread(receiverMessageProcess, this);
	//Mapbar_setThreadName(m_threadHandle, ReceiveThread);
	return;
}
/**
*	@brief
*		获取IpcWrapper
*	@return
*		void
*/
NaviSysIPCWrapper* NaviSysReceiver::getIpcWrapper(){
	return m_ipcWrapper;
}
