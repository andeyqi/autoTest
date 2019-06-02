#ifndef __SERVICE_MANAGER_ENGINE_NAVI_SYSTEM_STATE_NOTIFY_H__
#define __SERVICE_MANAGER_ENGINE_NAVI_SYSTEM_STATE_NOTIFY_H__

namespace ServiceManager
{
	class SystemNotify
	{
	public:
		static SystemNotify* getInstance();
		static void releaseInstance();
		//BOOL naviExitComplete();
		//delete by shenghao for exit timeout 20180711
		//void setSendStopMsgFlag(bool flg);
		void sendACCOFFSuc(void);
		// delete by liubo
		//void NaviSystemStateNotifySvcEngine_Initialize();
		//void NaviSystemStateNotifySvcEngine_CleanUp(void);
		//BOOL NaviSystemStateNotifySvcEngine_NaviExitCompleteMsg();	
	private:
		SystemNotify();
		~SystemNotify();
		SystemNotify(SystemNotify& display);
		SystemNotify& operator=(SystemNotify& display);
		friend void sendNaviStopSuc(void);
		friend void AnswerNaviExitSucess(void * para);
		//delete by shenghao for exit timeout
		//friend void AnswerNaviStopSucess(void * para);

		static SystemNotify* m_instance;
		Handle m_exitCompleteEvent;
		//Handle m_stopSucessEvent;
		//delete by shenghao for exit timeout 20180711
		//bool isNeedSendMsg;
	};
}
#endif /* __SERVICE_MANAGER_ENGINE_NAVI_SYSTEM_STATE_NOTIFY_H__ */