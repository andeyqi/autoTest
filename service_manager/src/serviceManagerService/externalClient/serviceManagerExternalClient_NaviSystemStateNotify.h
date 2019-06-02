#ifndef __SERVICE_MANAGER_EXTERNAL_CLIENT_NAVI_SYSTEM_STATE_NOTIFY_H__
#define __SERVICE_MANAGER_EXTERNAL_CLIENT_NAVI_SYSTEM_STATE_NOTIFY_H__



namespace ServiceManager
{
	#define TIMER_TBT_1_SECOND	1000
	#define TIMER_NAVI_IS_LIVE  45000   // 每45秒发送live状态

	class NaviSystemStateNotifySvcClient : public MbApi::MessageDistribution::GpsEventListener,
		public MbApi::MessageDistribution::GeneralEventListener
	{
		public:
			NaviSystemStateNotifySvcClient();
			virtual ~NaviSystemStateNotifySvcClient();
			void initialize();
		
			BOOL onGpsEventNotify(MbApi::MessageDistribution::GpsEvent event, void* data);
			BOOL onGeneralEventNotify(MbApi::MessageDistribution::GeneralEvent event, void* data);

		private:
			// delete by liubo
			//Handle NaviDataTimer;//导航状态数据通知周期			
	};


	void NaviSystemStateNotifySvcClient_Initialize();
	//delete by shenghao for exit timeout 20180711
	//void NaviSystemStateNotifySvcClient_CleanUp(void);
	void NaviSystemStateNotifySvcClient_ReqACCOFF(void *data);
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_NAVI_SYSTEM_STATE_NOTIFY_H__ */
