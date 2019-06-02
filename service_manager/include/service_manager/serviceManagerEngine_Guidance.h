#ifndef __SERVICE_MANAGER_ENGINE_GUIDANCE_H__
#define __SERVICE_MANAGER_ENGINE_GUIDANCE_H__

#include "service_manager/serviceManagerService.h"

namespace ServiceManager
{
	class GuidanceSvcEngineListener
	{
	public:
		virtual void onCarPlayActiveChangeNotify(BOOL data) { UNUSED_VAR(data); };
	};

	class GuidanceSvcEngine : public ServiceManagerService <GuidanceSvcEngineListener>
	{
	public:
		static GuidanceSvcEngine* getInstance();
		//delete by shenghao for exit timeout
		//static void releaseInstance();
		// cleanup
		//virtual bool ExitInstance(void);

		// init
		virtual bool InitInstance(void);

	public:
		
		BOOL IsCarPlayNaviActive(void);
		
	private:
		GuidanceSvcEngine();
		~GuidanceSvcEngine();
		GuidanceSvcEngine(GuidanceSvcEngine& guidance);
		GuidanceSvcEngine& operator=(GuidanceSvcEngine& guidance);
		void GuidanceSvcEngine_CarPlayActiveChangedNotify(BOOL para);
	private:
		friend void CarPlayActiveChangedNotify(void* para);
		friend void AnswerCarPlayNaviStatus(void* para);
		static GuidanceSvcEngine* m_instance;
		BOOL m_isCarPlayNaviActive;
		Handle m_carPlayNaviActiveEvent;
	};
}

#endif /* __SERVICE_MANAGER_ENGINE_GUIDANCE_H__ */