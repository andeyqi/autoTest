#ifndef __SERVICE_MANAGER_ENGINE_DISPLAY_H__
#define __SERVICE_MANAGER_ENGINE_DISPLAY_H__
#include <vector>
#include "service_manager/serviceManagerService.h"

namespace ServiceManager
{
	enum EN_DISPLAYSVCENGINE_ILL_STATE
	{
		EN_DISPLAYSVCENGINE_ILL_STATE_ON,
		EN_DISPLAYSVCENGINE_ILL_STATE_OFF,
		EN_DISPLAYSVCENGINE_ILL_STATE_INVALID,
	};
	
	enum DeSai_BackLightMode
	{
		SM_BLM_MANUAL = 0,
		SM_BLM_AUTO	//1
	};
	enum DeSai_BackLightType
	{
		SM_BLT_DAY = 0,
		SM_BLT_NIGHT	//1
	};

	enum EN_DISPLAYSVCENGINE_WIDGET_STATE
	{
		EN_DISPLAYSVCENGINE_WIDGET_SHOW = 0,
		EN_DISPLAYSVCENGINE_WIDGET_HIDE,
		EN_DISPLAYSVCENGINE_WIDGET_INVALID,
	};

	typedef struct DeSai_BackLight
	{
		DeSai_BackLightMode mode;
		DeSai_BackLightType type;
		int level;
	}DeSai_BackLight;
	
	typedef enum Linkage_Ctrl_ID{
		GET_DAYNIGHT_MODE_LINKAGE_STATUS = 0,
		NOTIFY_SYS_LINKAGE_YES,
		NOTIFY_SYS_LINKAGE_NO,
	}Linkage_Ctrl_ID;
	
	typedef struct DayNight_Mode_Status
	{
		int status;
		int mode;
		union
		{
			uint32_t bitmaps;
			struct
			{
				uint32_t isAlreadyGeted	 : 1;
				uint32_t invalued		 : 31;
			}bits;
		}linkageevent;
	}DayNight_Mode_Status;
	
	//HMI时间格式
	enum EN_DISPLAYSVCENGINE_TIME_FORMAT
	{
		EN_DISPLAYSVCENGINE_TIME_FORMAT_12HOUR = 0,
		EN_DISPLAYSVCENGINE_TIME_FORMAT_24HOUR,
		EN_DISPLAYSVCENGINE_TIME_FORMAT_UNKNOWN,
	};

	typedef struct TimeInfo
	{
		uint64 timeValue;
		EN_DISPLAYSVCENGINE_TIME_FORMAT timeFormat;
	}TimeInfo;

	typedef struct TouchInfo
	{
		bool status;
		uint32 tickCount;
	}TouchInfo;

	class DisplaySvcEngineListener
	{
		public:
			//ILL状态状态变化通知
			virtual void onILLStateChangedNotify(EN_DISPLAYSVCENGINE_ILL_STATE data) { UNUSED_VAR(data); }
			//背光状态变化通知
			virtual void onBacklightModeChangNotify(DeSai_BackLight data) { UNUSED_VAR(data); }
			virtual void onLanguageIdChangNotify(int32 data) { UNUSED_VAR(data); }
			virtual void onLanguageSwitchNotify(uint32* data) { UNUSED_VAR(data); }
			// 请求MusicWidget显示回调通知
			virtual void onMusicWidgetShowNotify(bool data) { UNUSED_VAR(data); }
			// 请求MusicWidget隐藏回调通知
			virtual void onMusicWidgetHideNotify(bool data) { UNUSED_VAR(data); }
			// 请求MusicWidget隐藏回调通知
			virtual void onMusicWidgetIsShowNotify(bool data) { UNUSED_VAR(data); }
			// 车机系统时间变化通知
			virtual void onSysHmiTimeChangeNotify(uint64 data) { UNUSED_VAR(data); }
			virtual void onSysDayNightModeSwitchChangeNotify(int32 mode, int32 param) { UNUSED_VAR(mode); UNUSED_VAR(param); }
			// 触摸事件信号使能与关闭通知
			virtual void onTouchStatusChangeNotify(bool data){ UNUSED_VAR(data); }
			// 时间格式状态通知
			virtual void onTimeFormatNotify(EN_DISPLAYSVCENGINE_TIME_FORMAT data){ UNUSED_VAR(data); }
	};
	
	class DisplaySvcEngine : public ServiceManagerService < DisplaySvcEngineListener >
	{
		public:
			static DisplaySvcEngine* getInstance();

			//delete by shenghao for exit timeout
			//static void releaseInstance();
			//bool ExitInstance();

			bool InitInstance();

			EN_DISPLAYSVCENGINE_ILL_STATE ReqCheckILLState();

			BOOL DisplaySvcEngine_ReqShowOtherForm();
			BOOL DisplaySvcEngine_ReqHideOtherForm();
//			void DisplaySvcEngine_RetLanguageSwitch(bool data);

			//前后台切换
			void ReqBackgroundSwitch(void);

			DeSai_BackLight GetBacklightMode();
			int32 GetLanguageID(void);

			void DisplaySvcEngine_BackToHost(void);
			void DisplaySvcEngine_ShowHostMenu(void);

			// 请求显示MusicWidget
			void DisplaySvcEngine_ReqShowMusicWidget();
			// 请求隐藏MusicWidget
			void DisplaySvcEngine_ReqHideMusicWidget();
			// 获取MusicWidget显示状态
			EN_DISPLAYSVCENGINE_WIDGET_STATE DisplaySvcEngine_GetMusicWidgetStatus();
			// 请求系统HMI时间
			uint64 DisplaySvcEngine_ReqSysHmiTime();
			// 获取导航前后台状态
			bool GetNaviRunStatus(void);
			// 设定导航前后台状态
			void SetNaviRunStatus(bool);
			// 通知车机可以进行前后台切换
			void DisplaySvcEngine_ReadyForForground(void);
			int32_t DisplaySvcEngine_ReqSysLinkAgeInfo(Linkage_Ctrl_ID cmdid,void *data);
			// 获取时间格式
			EN_DISPLAYSVCENGINE_TIME_FORMAT getTimeFormat();
		private:
			DisplaySvcEngine();
			~DisplaySvcEngine();
			DisplaySvcEngine(DisplaySvcEngine& display);
			DisplaySvcEngine& operator=(DisplaySvcEngine& display);


			void illStateChangedNotify(EN_DISPLAYSVCENGINE_ILL_STATE data);
			void backlightModeChangNotify(DeSai_BackLight data);
			void languageIdChangNotify(int32 data);
//			void DisplaySvcEngine_LanguageSwitchNotify(void *data);

			friend void illStateChangedNotify(void* param);
//			friend void LanguageSwitchNotify(void *data);
			friend void DisplaySvcEngine_SgnShowOtherFormSuc(void* data);
			friend void DisplaySvcEngine_SgnHideOtherFormSuc(void* data);
			friend void DisplaySvcEngine_SgnILLState(void* data);
			friend void DisplaySvcEngine_AnswerBacklightMode(void* data);
			friend void DisplaySvcEngine_BacklightModeChangeNotify(void* pData);
			friend void DisplaySvcEngine_AnswerLanguageID(void* data);
			friend void DisplaySvcEngine_LanguageIDChangeNotify(void* data);
			friend void DisplaySvcEngine_LanguageSwitchChanged(void* data);
			// MusicWidget show
			friend void AnswerShowMusicWidget(void* data);
			friend void ShowMusicWidgetNotify(void* data);
			void DisplaySvcEngine_ShowMusicWidgetNotify(bool data);
			// MusicWidget hide
			friend void AnswerHideMusicWidget(void* data);
			friend void HideMusicWidgetNotify(void* data);
			void DisplaySvcEngine_HideMusicWidgetNotify(bool data);
			// 通知mapdal
			void DisplaySvcEngine_MusicWidgetChangeNotify(bool data);
			// SysHmiTime
			friend void AnswerSysHmiTime(void* data);
			friend void SysHmiTimeChangedNotify(void* data);
			void DisplaySvcEngine_SysHmiTimeNotify(uint64 data);

			friend void NotifyDayNightLinkAge(void* data); 
			friend void SysDayNightLinkAgNotify(void* data);
			friend void NotifyTouchStatus(void* data);
			friend int TouchStatusChangedNotify(void *data);
			void DisplaySvcEngine_TouchStatusChangedNotify(bool data);

			friend void TimeFormatChangeNotify(void *data);
			void TimeFormatNotify(EN_DISPLAYSVCENGINE_TIME_FORMAT data);	// 时间格式状态通知

			void DisplaySvcEngine_SysDayNightLinkAgNotify(int32_t status,int32_t mode);
		private:
			static DisplaySvcEngine* m_instance;

//			void* m_languageShowStatus;
			bool isBack;
			int32 m_languageId;
			DeSai_BackLight* m_backLight;
			EN_DISPLAYSVCENGINE_ILL_STATE m_illStatus;
			uint64 m_timeDValue;
			EN_DISPLAYSVCENGINE_WIDGET_STATE m_widgetStatus;
			//BOOL m_isTouchEnabled;

			Handle m_displaySvcShowOtherEvent;
			Handle m_displaySvcHideOtherEvent;
			Handle m_displaySvcEventILL;
			Handle m_displaySvcBackLightEvent;
			Handle m_displaySvcLanguageIdEvent;
			Handle m_displaySvcSysHmiTimeEvent;
			Handle m_displaySvcDayNightLinkAgeEvent;
			DayNight_Mode_Status m_dayNightLinkAgeData;
			TouchInfo m_touchInfo;
			uint32 m_tickCount;

			// TimeFormat
			EN_DISPLAYSVCENGINE_TIME_FORMAT m_timeFormat;
			TimeInfo* m_timeInfo;

	};
}

#endif /* __SERVICE_MANAGER_ENGINE_DISPLAY_H__ */