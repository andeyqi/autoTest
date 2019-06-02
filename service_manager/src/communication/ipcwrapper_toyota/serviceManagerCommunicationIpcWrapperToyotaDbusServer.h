#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_DBUS_SERVICE_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_DBUS_SERVICE_H__

#include "serviceManagerCommunicationIpcWrapperToyotaDbusCommon.h"

namespace ServiceManager
{
	class ToyotaDbusServiceListener
	{
	public:
		virtual void notifyMsg(void* data) = 0;
	};

	class ToyotaDbusService 
	{
	public:
		// ʵ����
		static ToyotaDbusService* getInstance();
		static void releaseInstance();	
		
		// initial dbus connection
		void initDbusConnect();

		void addListener(ToyotaDbusServiceListener* listener);
		void removeListener(ToyotaDbusServiceListener* listener);
		
		// ����signal
		bool emitSignal(const cqCHAR* signalValue);

	private:
		ToyotaDbusService();
		~ToyotaDbusService();
		ToyotaDbusService(ToyotaDbusService& sevice);
		ToyotaDbusService& operator=(ToyotaDbusService& service);
		
		static DBusConnection* getDBusConnection(const cqCHAR* connectionName, const cqCHAR* matchName);
		static void disConnectConnection(DBusConnection* connection, const cqCHAR* connectionName, const cqCHAR* matchName);
		
		// ����signal
		friend void* monitorSignal(void* pdata);
	private:
		DBusConnection* mpConnection;	// ����
		pthread_t mTthread;
		cqstd::vector<ToyotaDbusServiceListener*> mpListener;
		int mFlag;						// �����ӿ�monitorSignalʹ�õı�־

		//class object add by shenghao
		static ToyotaDbusService* m_instatce;
	};
}

#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_DBUS_SERVICE_H__ */