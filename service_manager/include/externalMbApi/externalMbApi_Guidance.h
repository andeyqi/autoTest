#ifndef __EXTERNAL_MBAPI_GUIDANCE_H__
#define __EXTERNAL_MBAPI_GUIDANCE_H__

#include "cq_stdlib/cq_stdlib.h"
#include "serviceManagerService.h"

namespace ServiceManager
{
	class GuidanceSvcEngineListener
	{
	public:
		virtual void onCarPlayActiveChangeNotify(bool* data) { UNUSED_VAR(data); };
	};

	class GuidanceSvcEngine : public ServiceManagerService <GuidanceSvcEngineListener>
	{
	public:
		static GuidanceSvcEngine* getInstance();
		static void releaseInstance();

		// init
		virtual bool InitInstance(void);
		// cleanup
		virtual bool ExitInstance(void);
	public:
		// 检查CarPlay是否已经制定了路线。
		void IsCarPlayNaviActive(void);

	private:
		GuidanceSvcEngine();
		~GuidanceSvcEngine();
		GuidanceSvcEngine(GuidanceSvcEngine& guidance);
		GuidanceSvcEngine& operator=(GuidanceSvcEngine& guidance);
	private:
		void CarPlayActiveChangedNotify(bool* para);  // CarPlay制定路线通知

		friend void CarPlayActiveChangedNotify(void* para);
		static GuidanceSvcEngine* m_instance;
	};
}
#endif /* __EXTERNAL_MBAPI_GUIDANCE_H__ */