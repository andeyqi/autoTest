#ifndef __EXTERNAL_MBAPI_ADDITION_H__
#define __EXTERNAL_MBAPI_ADDITION_H__

#include "serviceManagerService.h"

namespace ServiceManager
{
#define CONFIG_FILE_JSON	L"other/uuid.json"

	// wifi status 
	enum EN_ADDITIONSVCENGINE_WIFI_STATE
	{
		EN_ADDITIONSVCENGINE_WIFI_STATE_WIFICONNECTED = 0,
		EN_ADDITIONSVCENGINE_WIFI_STATE_WIFIDISCONNECTED,
		EN_ADDITIONSVCENGINE_WIFI_STATE_WIFICONNECTING,
		EN_ADDITIONSVCENGINE_WIFI_STATE_INVALID      // 仅ServiceManager模块内部使用，外部无需处理这个状态
	};

	// run status 
	enum EN_ADDITIONSVCENGINE_RUN_STATE
	{
		EN_ADDITIONSVCENGINE_RUN_STATE_RUNSTS = 0,  // 走行规制中
		EN_ADDITIONSVCENGINE_RUN_STATE_DISRUNSTS,   // 未走行规制中
		EN_ADDITIONSVCENGINE_RUN_STATE_INVALID      // 仅ServiceManager模块内部使用，外部无需处理这个状态
	};
	
	//usb status
	enum EN_ADDITONSVCENGINE_USB_STATE
	{
		EN_ADDITONSVCENGINE_USB_UNMOUNT = 0,
		EN_ADDITONSVCENGINE_USB_MOUNT,
	};

	// wifi/dcm status 
	enum EN_ADDITIONSVCENGINE_NETWORK_STATE
	{
		EN_ADDITIONSVCENGINE_NETWORK_STATE_DISCONNECTED = 0,   //未连接
		EN_ADDITIONSVCENGINE_NETWORK_STATE_WIFICONNECTED,   //wifi连接
		EN_ADDITIONSVCENGINE_NETWORK_STATE_DCMCONNECTED,    //dcm连接
		EN_ADDITIONSVCENGINE_NETWORK_STATE_INVALID,			// 仅ServiceManager模块内部使用，外部无需处理这个状态
	}; 

	//倒车状态
	enum EN_ADDITONSVCENGINE_REV_STATE
	{
		EN_ADDITONSVCENGINE_REV_STATE_REVERSE = 0,
		EN_ADDITONSVCENGINE_REV_STATE_UNREVERSE,
		EN_ADDITONSVCENGINE_REV_STATE_INVALID,
	};
	//开始手车连携
	enum EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE
	{
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_SUCCESS = 0,
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_FALSE,
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_INVALID,
	};

	typedef struct UsbData
	{
		EN_ADDITONSVCENGINE_USB_STATE usbState;
		char* usbPath;
	}UsbData;

	class AdditionSvcEngineListener
	{
	public:
		// 走行规制状态通知
		virtual void onRunStatusChangedNotify(EN_ADDITIONSVCENGINE_RUN_STATE data){ UNUSED_VAR(data); }
		// wifi连接状态通知
		virtual void onWifiStatusChangedNotify(EN_ADDITIONSVCENGINE_WIFI_STATE data){ UNUSED_VAR(data); }
		// GPS更新时间是否可用通知
		virtual void onGPSUpdateTimeEnabledNotify(bool data){ UNUSED_VAR(data); }
		// Map SD card通知
		virtual void onMapCardChangedNotify(bool data){ UNUSED_VAR(data); }
		// USB状态通知
		virtual void onUsbStatusChangedNotify(UsbData* data){ UNUSED_VAR(data); }
		//wifi/dcm状态
		virtual void onNetworkStatusChangedNotify(EN_ADDITIONSVCENGINE_NETWORK_STATE data){ UNUSED_VAR(data); }
		//倒车状态通知
		virtual void onRevStatusChangedNotify(EN_ADDITONSVCENGINE_REV_STATE data){ UNUSED_VAR(data); }
		//G-BOOK信息
		virtual void onGbookInfoChangedNotify(char* data){ UNUSED_VAR(data); }
		//手车连携设备插拔通知
		virtual void onDeviceConnectChangedNotify(DeviceInfo* data){ UNUSED_VAR(data); }
		//开始手车连携反馈
		virtual void onDeviceConnectChangedNotify(EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE data){ UNUSED_VAR(data); }

	};

	class AdditionSvcEngine : public ServiceManagerService < AdditionSvcEngineListener >
	{
	public:

		static AdditionSvcEngine* getInstance();
		static void releaseInstance();

		// initialize
		bool InitInstance(void);

		// cleanup
		bool ExitInstance(void);
	public:

		// 导航请求走行规制状态
		EN_ADDITIONSVCENGINE_RUN_STATE getRunStatus(void);

		// 获取uuid(同期要求)
		char* getUUID(void);

		// 鉴权uuid
		BOOL isNaviActivate(void);

		// 获取wifi状态(同期要求)
		EN_ADDITIONSVCENGINE_WIFI_STATE getWifiStatus(void);

		// GPS更新时间是否可用
		void isGPSUpdateTimeEnabled();

		//USB状态请求
		UsbData* AdditionSvcEngine_ReqUsbStatus();

		// 获取wifi/dcm状态(同期要求)
		EN_ADDITIONSVCENGINE_NETWORK_STATE getNetworkStatus(void);

		//倒车状态
		EN_ADDITONSVCENGINE_REV_STATE getRevStatus();

		//G-BOOK信息
		char* getGbookInfo(void);
	private:
		AdditionSvcEngine();
		~AdditionSvcEngine();
		AdditionSvcEngine(AdditionSvcEngine& additon);
		AdditionSvcEngine& operator=(AdditionSvcEngine& addition);

		void runStatusChangedNotify(EN_ADDITIONSVCENGINE_RUN_STATE data);		// 走行规制通知
		void wifiStatusChangedNotify(EN_ADDITIONSVCENGINE_WIFI_STATE data);	//wifi通知
		void GPSUpdateTimeEnabledNotify(bool data);	// GPS更新时间是否可用通知
		void MapCardChangedNotify(bool data);	// Map SD card通知

		friend void GPSUpdateTimeEnabledNotify(void* data);
		friend void MapCardChangedNotify(void* data);
		friend void runStatusChangedNotify(void* param);
		friend void AdditionSvcEngine_RunStatusChanged(void* data);
		friend void wifiStatusChangedNotify(void* param);
		friend void AdditionSvcEngine_WifiStatusChanged(void* data);
		friend void AdditionServiceEngine_SgnUUIDChanged(void* data);

	private:
		static AdditionSvcEngine* m_instance;

		// uuid
		char m_uuid[128];
		Handle m_uuidEvent;

		// 走行规制
		EN_ADDITIONSVCENGINE_RUN_STATE m_runStatus;
		Handle m_runStatusEvent;

		// wifi status
		EN_ADDITIONSVCENGINE_WIFI_STATE m_wifiStatus;
		Handle m_wifiStatusEvent;
	};
}
#endif /* __EXTERNAL_MBAPI_ADDITION_H__ */