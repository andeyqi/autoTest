#ifndef __EXTERNAL_MBAPI_NAVI_SYSTEM_STATE_NOTIFY_H__
#define __EXTERNAL_MBAPI_NAVI_SYSTEM_STATE_NOTIFY_H__

namespace ServiceManager
{
	class SystemNotify
	{
	public:
		static SystemNotify* getInstance();
		static void releaseInstance();

		//BOOL naviExitComplete();


		void NaviSystemStateNotifySvcEngine_Initialize();
		void NaviSystemStateNotifySvcEngine_CleanUp(void);
		BOOL NaviSystemStateNotifySvcEngine_NaviExitCompleteMsg();

	private:
		SystemNotify();
		~SystemNotify();
		SystemNotify(SystemNotify& display);
		SystemNotify& operator=(SystemNotify& display);

		friend void AnswerNaviExitSucess(void * para);


		static SystemNotify* m_instance;
		Handle m_exitCompleteEvent;
	};
}
#endif /* __SERVICE_MANAGER_ENGINE_NAVI_SYSTEM_STATE_NOTIFY_H__ */