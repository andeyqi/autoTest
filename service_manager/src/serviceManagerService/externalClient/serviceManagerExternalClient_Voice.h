#ifndef __SERVICE_MANAGER_EXTERNAL_CLIENT_VOICE_H__
#define __SERVICE_MANAGER_EXTERNAL_CLIENT_VOICE_H__


namespace ServiceManager
{
	enum EN_VOICESVCCLIENT_FUNCTIONINMAINTHREAD
	{
		EN_VOICESVCCLIENT_FUNCTIONINMAINTHREAD_OK,
		EN_VOICESVCCLIENT_FUNCTIONINMAINTHREAD_NG,
	};

	void VoiceSvcClient_Initialize();
	//delete by shenghao for exit timeout
	//void VoiceSvcClient_CleanUp();
	// delete by liubo
	// 不需要在头文件中暴露此接口
	// void VoiceSvcClient_ReqStopEMG(void *data);
	// int VoiceSvcClient_SoundArbiter_ForceStop(void* data);
	// void VoiceSvcClient_ReqVoiceStateCHK(void* data);
	// int VoiceSvcClient_SoundArbiter_IsPlaying(void* data);
	// BOOL Judge_SoundArbiter_IsPlaying();
	// void Send_VoiceStateCHKMsg(void* data);
	void VoiceSvcClient_SetNaviVolume(void* data);
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_VOICE_H__ */