#ifndef __EXTERNAL_MBAPI_VOICE_H__
#define __EXTERNAL_MBAPI_VOICE_H__

#include "cq_stdlib/cq_stdlib.h"
#include "serviceManagerService.h"

namespace ServiceManager
{
	enum BEEP_TYPE
	{
		Beep_Type_Button = 0
	};

	class VoiceSvcEngineListener
	{
	public:
		// Volume 
		virtual void onVolumeChangeNotify(uint32* data) { UNUSED_VAR(data); };
		// Mute 
		virtual void onMuteChangedNotify(uint32* data) { UNUSED_VAR(data); };
	};

	class VoiceSvcEngine : public ServiceManagerService <VoiceSvcEngineListener>
	{
	public:
		static VoiceSvcEngine* getInstance();
		static void releaseInstance();
	public:
		// 发声开始
		BOOL ReqVoiceStart(void);
		// 发声结束
		BOOL ReqVoiceStop(void);
		// 请求播放beep音
		void ReqPlayBeepSound(BEEP_TYPE beepType);
		// 获取系统音量
		uint32 ReqSysVolume();
		// 设置Navi音量给车机系统
		void SetNaviVolume(uint32 vol);
		// 如果允许播放音频指令，则必须在播放音频指导之前调用此功能
		void RequestAudioGuidancePlay(void);
		// 释放已经获得音频播放权限
		void ReleaseAudioGuidancePlay(void);
	protected:
		// init
		virtual bool InitInstance(void);
		// cleanup
		virtual bool ExitInstance(void);
	private:
		VoiceSvcEngine();
		~VoiceSvcEngine();
		VoiceSvcEngine(VoiceSvcEngine& voice);
		VoiceSvcEngine& operator=(VoiceSvcEngine& addition);

		void VolumeChangedNotify(uint32* para);
		void MuteChangedNotify(uint32* para);

		friend void AnswerVoiceStart(void* data);
		friend void VolumeChangedNotify(void* para);
		friend void AnswerVolume(void* data);
		friend void MuteChangedNotify(void* para);
		friend void AnswerMute(void* data);

	private:
		// 发声开始
		Handle m_reqVoiceStartEvent;

		// 获取系统音量
		uint32 m_volume;
		Handle m_volumeEvent;
		static VoiceSvcEngine* m_instance;
	};

}

#endif /* __EXTERNAL_MBAPI_VOICE_H__ */
