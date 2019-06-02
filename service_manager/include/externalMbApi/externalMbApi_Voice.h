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
		// ������ʼ
		BOOL ReqVoiceStart(void);
		// ��������
		BOOL ReqVoiceStop(void);
		// ���󲥷�beep��
		void ReqPlayBeepSound(BEEP_TYPE beepType);
		// ��ȡϵͳ����
		uint32 ReqSysVolume();
		// ����Navi����������ϵͳ
		void SetNaviVolume(uint32 vol);
		// �����������Ƶָ�������ڲ�����Ƶָ��֮ǰ���ô˹���
		void RequestAudioGuidancePlay(void);
		// �ͷ��Ѿ������Ƶ����Ȩ��
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
		// ������ʼ
		Handle m_reqVoiceStartEvent;

		// ��ȡϵͳ����
		uint32 m_volume;
		Handle m_volumeEvent;
		static VoiceSvcEngine* m_instance;
	};

}

#endif /* __EXTERNAL_MBAPI_VOICE_H__ */
