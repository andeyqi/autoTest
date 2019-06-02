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
*		���캯��
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
*		��������
*/
NaviSysReceiver::~NaviSysReceiver()
{
	CQ_LOG_INFO("[ServiceManager] ~NaviSysReceiver.\n");
	// �ͷ�IPCWrapper
	m_ipcWrapper->removeListener();
	delete(m_ipcWrapper);
	m_ipcWrapper = NULL;
	// �����Ϣ����
	naviServiceClearReceiveList();
	// �ر�Receiver�߳�
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
*		����Ϣ������Ϣ����
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
*		��ȡNaviSysReceiver��������
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
*		�ͷ�NaviSysReceiver��������
*	@return
*		void
*/

/**
*	@brief
*		ע���첽�ص�����
*	@return
*		void
*	@param
*		funcId ����ID, *pfun �ص�����ָ��
*/
//		delete mpInstance;
//		mpInstance = NULL;
//	}
//	return;
//}

/**
*	@brief
*		ע���첽�ص�����
*	@return
*		void
*	@param
*		funcId ����ID, *pfun �ص�����ָ��
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
*		ע��ͬ���ص�����
*	@return
*		void
*	@param
*		funcId ����ID, *pfun �ص�����ָ��
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

	// ���FuncID����Max
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
		// 1�����ڶ����е���Ϣ�ڴ˴������ͷš�
		// 2�������е���ϢipcWrapperProcess�н����ͷš�
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
		// �����е���ϢipcWrapperProcess�н����ͷ�
		sendMsgQuene(data, LOW_PRIO);
	}
	return;
}

/**
*	@brief
*		�ж�ͬ���첽����ִ�лص�����
*	@return
*		void
*	@param
*		data �ص�Я������
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
*		ִ�лص�����
*	@return
*		void
*	@param
*		data �ص�����
*/
void NaviSysReceiver::doAsynCallback(ReceiveData *paramData)
{
	// add by renxs 2017.10.13 ��ӷǿ��ж�
	if (m_CallBackArray[paramData->funcId].callback)
	{
		m_CallBackArray[paramData->funcId].callback(paramData->data);
	}
	return;
}

/**
*	@brief
*		����Ϣ�����л�ȡmsg
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
*		�ж���Ϣ�����Ƿ���msg
*	@return
*		bool true���� false����
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
*		�����Ϣ����
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
*		ִ�лص�
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
*		����Recever��Ϣ�����߳�
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
*		��ȡIpcWrapper
*	@return
*		void
*/
NaviSysIPCWrapper* NaviSysReceiver::getIpcWrapper(){
	return m_ipcWrapper;
}
