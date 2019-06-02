#include "stdafx_serviceManager.h"

//=====================================================//
NaviSysSender* NaviSysSender::mpInstance = NULL;
static Handle m_mutex = invalidHandle;
static BOOL m_shouldExit = FALSE;
static Handle m_threadHandle = invalidHandle;
#define SenderThread	"SM_SenderThread"
using namespace ServiceManager;

/**
*	@brief
*		NaviSysSender����
*	@return
*		void
*/
NaviSysSender::NaviSysSender()
{
	CQ_LOG_INFO("[ServiceManager] NaviSysSender::NaviSysSender.\n");
	m_mutex = Mapbar_createMutex();
	m_requestList.clear();
	NaviSysSender::startSenderMessageProcess();
	m_ipcWrapper = NaviSysReceiver::getInstance()->getIpcWrapper();
}

/**
*	@brief
*		NaviSysSender����
*	@return
*		void
*/
NaviSysSender::~NaviSysSender()
{
	CQ_LOG_INFO("[ServiceManager] ~NaviSysIPCWrapperToyota.\n");
	naviServiceClearRequestList();
	m_ipcWrapper = NULL;
	
	if (m_threadHandle != invalidHandle)
	{
		m_shouldExit = true;
		Mapbar_waitThread(m_threadHandle);
		Mapbar_closeThread(m_threadHandle);
		m_threadHandle = invalidHandle;
	}

	Mapbar_destroyMutex(m_mutex);
	return;
}

/**
*	@brief
*		Engine��Sender������Ϣ�ӿ�
*	@return
*		bool true���ɹ� false��ʧ��
*	@param
*		data�����͵�msg��priority�����ȼ�
*/
bool NaviSysSender::sendMsg(void *data, int priority)
{
	UNUSED_VAR(priority);
	Mapbar_lockMutex(m_mutex);

	m_requestList.push_back((SendData*)data);

	Mapbar_unlockMutex(m_mutex);
	return true;
}

/**
*	@brief
*		����Ϣ�����л�ȡmsg
*	@return
*		Data msg
*/
SendData* NaviSysSender::naviServiceGetRequest()
{
	Mapbar_lockMutex(m_mutex);

	if (m_requestList.size() <= 0)
	{
		Mapbar_unlockMutex(m_mutex);
		return NULL;
	}

	SendData* msg = m_requestList.front();
	m_requestList.pop_front();

	Mapbar_unlockMutex(m_mutex);

	return msg;
}

/**
*	@brief
*		�ж���Ϣ�����Ƿ���msg
*	@return
*		bool true���� false����
*/
bool NaviSysSender::naviServiceHasRequest()
{
	Mapbar_lockMutex(m_mutex);

	int size = m_requestList.size();

	Mapbar_unlockMutex(m_mutex);

	return (size != 0);
}

/**
*	@brief
*		�����Ϣ����
*	@return
*		bool
*/
bool NaviSysSender::naviServiceClearRequestList()
{
	Mapbar_lockMutex(m_mutex);

	if (!naviServiceHasRequest())
	{
		Mapbar_unlockMutex(m_mutex);
		return true;
	}

	std::list<SendData*>::iterator itor = m_requestList.begin();

	while (itor != m_requestList.end())
	{
		free(*itor);
		itor++;
	}

	m_requestList.clear();

	Mapbar_unlockMutex(m_mutex);

	return true;
}

/**
*	@brief
*		��IPCWrapper������Ϣ
*/
int senderMessageProcess(void* para)
{
	NaviSysSender* m_sender = (NaviSysSender*)para;
	
	if (m_threadHandle != invalidHandle)
		Mapbar_setThreadName(m_threadHandle, SenderThread);

	while (!m_shouldExit)
	{
		if (!m_sender->naviServiceHasRequest())
		{
			Mapbar_sleep(100);
			//printf("Sender print 0 \n");
			continue;
		}

		SendData* request = m_sender->naviServiceGetRequest();

		//call ServiceManager engine/client callback function
		if (request != NULL)
			m_sender->m_ipcWrapper->sendMsg(request, LOW_PRIO);
	};
	return D_NAVISYS_OK;
}

/**
*	@brief
*		����Sender��Ϣ�����߳�
*	@return
*		void
*/
void NaviSysSender::startSenderMessageProcess()
{
	m_threadHandle = Mapbar_createThread(senderMessageProcess, this);
	//Mapbar_setThreadName(m_threadHandle, SenderThread);
	return;
}

/**
*	@brief
*		��ȡNaviSysSender��������
*/
NaviSysSender *NaviSysSender::getInstance()
{
	/*!< check if mpInstance is initialized or not  */
	CQ_LOG_INFO("[ServiceManager] NaviSysSender::getInstance.\n");
	if (NULL == mpInstance)
	{
		mpInstance = new NaviSysSender();
	}
	return mpInstance;
}

/**
*	@brief
*		�ͷ�NaviSysSender����
*/
//delete by shenghao for exit timeout
//void NaviSysSender::releaseInstance()
//{
//	/*!< check if mpInstance is initialized or not  */
//	CQ_LOG_INFO("[ServiceManager] NaviSysSender::releaseInstance.\n");
//	if (NULL != mpInstance)
//	{
//		delete mpInstance;
//		mpInstance = NULL;
//	}
//	return;
//}
