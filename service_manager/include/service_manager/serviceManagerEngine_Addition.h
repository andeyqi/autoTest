#ifndef __SERVICE_MANAGER_ENGINE_ADDITION_H__
#define __SERVICE_MANAGER_ENGINE_ADDITION_H__

#include "service_manager/serviceManagerService.h"

namespace ServiceManager
{
#define CONFIG_FILE_JSON	L"other/uuid.json"
#define UUID_YEAR_SIZE      3
#define UUID_MACH_SIZE      4
#define UUID_OEM_SIZE       4
#define USBPATH             "/mnt/usb1"
//#define UUID_NUM_SIZE       8
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

	//usb status

	//倒车状态
	enum EN_ADDITONSVCENGINE_REV_STATE
	{
		EN_ADDITONSVCENGINE_REV_STATE_REVERSE = 0,
		EN_ADDITONSVCENGINE_REV_STATE_UNREVERSE,
		EN_ADDITONSVCENGINE_REV_STATE_INVALID,
	};

	typedef struct UsbData
	{
		EN_ADDITONSVCENGINE_USB_STATE usbState;
		char* usbPath;
	}UsbData;
	//开始手车连携
	enum EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE
	{
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_SUCCESS = 0,
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_FALSE,
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE_INVALID,
	};
	//手车连携设备插拔通知
	enum EN_ADDITONSVCENGINE_DEVICE_STATE
	{
		EN_ADDITONSVCENGINE_DEVICE_STATE_CONNECT = 0,
		EN_ADDITONSVCENGINE_DEVICE_STATE_DISCONNECT,
		EN_ADDITONSVCENGINE_DEVICE_STATE_INVALID,
	};

	enum EN_ADDITONSVCENGINE_DEVICE_TYPE
	{
		EN_ADDITONSVCENGINE_DEVICE_TYPE_ANDROID = 0,
		EN_ADDITONSVCENGINE_DEVICE_TYPE_IOS,
		EN_ADDITONSVCENGINE_DEVICE_TYPE_INVALID,
	};

	enum EN_ADDITONSVCENGINE_DEVICE_PROTOCOL
	{
		EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_AOA1 = 0,
		EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_AOA2,
		EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_iAP1,
		EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_iAP2,
		EN_ADDITONSVCENGINE_DEVICE_PROTOCOL_INVALID,
	};
	typedef struct DeviceInfo
	{
		EN_ADDITONSVCENGINE_DEVICE_STATE connectType;
		uint32 VendorId;
		uint32 ProductId;
		EN_ADDITONSVCENGINE_DEVICE_TYPE deviceType;
		EN_ADDITONSVCENGINE_DEVICE_PROTOCOL protocolType;
	}DeviceInfo;

	enum EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS
	{
		EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_CONNECTING,
		EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_CONNECTED,
		EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_CONNECTERR,
		EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS_DISCONNECT,
	};
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

		//delete by shenghao for exit timeout
		//static void releaseInstance();
		// cleanup
		//bool ExitInstance(void);

		// initialize
		bool InitInstance(void);

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
		BOOL isGPSUpdateTimeEnabled();

		//USB状态请求
		UsbData* AdditionSvcEngine_ReqUsbStatus();

		// 获取wifi/dcm状态(同期要求)
		EN_ADDITIONSVCENGINE_NETWORK_STATE getNetworkStatus(void);

		//倒车状态
		EN_ADDITONSVCENGINE_REV_STATE getRevStatus();

		//G-BOOK信息
		char* getGbookInfo(void);

		//手车连携设备插拔信息
		DeviceInfo* getDeviceInfo(void);

		//开始手车连携
		void startPhoneConnectNotify(bool data);

		//手车连携状态通知
		void phoneConnectStatusNotify(EN_ADDITONSVCENGINE_PHONE_CONNECT_STATUS data);

		//清除数据状态
		void setMemoryClearFlag(BOOL isRevMemoryClear);
		BOOL getMemoryClearFlag();
	private:
		AdditionSvcEngine();
		~AdditionSvcEngine();
		AdditionSvcEngine(AdditionSvcEngine& additon);
		AdditionSvcEngine& operator=(AdditionSvcEngine& addition);

		void runStatusChangedNotify(EN_ADDITIONSVCENGINE_RUN_STATE data);		// 走行规制通知
		void wifiStatusChangedNotify(EN_ADDITIONSVCENGINE_WIFI_STATE data);	//wifi通知
		void GPSUpdateTimeEnabledNotify(bool data);	// GPS更新时间是否可用通知
		void MapCardChangedNotify(bool data);	// Map SD card通知
		//void UsbStatusChangedNotify(void* data);
		void AdditionSvcEngine_UsbStatusChangedNotify(UsbData* data);
		void AdditionSvcEngine_networkStatusChangedNotify(EN_ADDITIONSVCENGINE_NETWORK_STATE data);
		void AdditionSvcEngine_RevStatusChangedNotify(EN_ADDITONSVCENGINE_REV_STATE data);
		void AdditionSvcEngine_gbookInfoChangedNotify(char* data);
		void AdditionSvcEngine_deviceConnectChangeNotify(DeviceInfo* data);
		void AdditionSvcEngine_startPhoneConnectResponse(EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE data);

		friend void GPSUpdateTimeEnabledNotify(void* data);
		friend void MapCardChangedNotify(void* data);
		friend void runStatusChangedNotify(void* param);
		friend void AdditionSvcEngine_RunStatusChanged(void* data);
		friend void wifiStatusChangedNotify(void* param);
		friend void AdditionSvcEngine_WifiStatusChanged(void* data);
		friend void AdditionServiceEngine_SgnUUIDChanged(void* data);
		friend void AdditionSvcEngine_AnwserIsGPSUpdateTimeEnabled(void* data);
		friend void AnswerReqUsbStatus(void* data);
		friend void UsbStatusChangedNotify(void* data);
		friend void RevStatusChangedNotify(void* data);
		friend void networkStatusChangedNotify(void* data);
		friend void AdditionSvcEngine_NetworkStatusChange(void* data);
		//friend void AdditionSvcEngine_RevStatusChanged(void* data);
		friend void gbookInfoChangedNotify(void* data);
		friend void AdditionSvcEngine_gbookInfoChange(void* data);
		friend void deviceConnectChangeNotify(void *data);
		friend void startPhoneConnectResponse(void *data);
		friend void AdditionSvcEngine_DeviceConnectChange(void* data);
		friend void AdditionSvcEngine_StartPhoneConnectResponse(void* data);
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

		// isGpsEnable
		BOOL m_isGPSEnabled;
		Handle m_GPSUpdateTimeEvent;
		
		//usb
		Handle m_addionSvcUsbEvent;
		UsbData* m_usbData;

		//wifi/dcm
		Handle m_networkStatusEvent;
		EN_ADDITIONSVCENGINE_NETWORK_STATE m_networkStatus;

		//倒车通知
		EN_ADDITONSVCENGINE_REV_STATE m_revStatus; 

		//G-book信息
		Handle m_gbookInfoEvent;
		char m_gbookInfo[256];

		//设备插拔通知
		Handle m_deviceInfoEvent;
		DeviceInfo m_deviceInfo;

		//清除数据
		BOOL m_isReceiveMemoryClear;

		//开始瞅车连携
		EN_ADDITONSVCENGINE_ENTRY_PHONE_STATE m_entryPhoneStatus;
	};
}

#endif /* __SERVICE_MANAGER_ENGINE_ADDITION_H__ */