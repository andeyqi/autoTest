#ifndef __SERVICE_MANAGER_COMMON_H__
#define __SERVICE_MANAGER_COMMON_H__
#include <mutex> 
namespace ServiceManager
{
	/**
	@brief ServiceManager Clinet init
	@param [in] 	void
	@return 		void
	@details
	*/
	void initServiceManagerClient();

	/**
	@brief ServiceManager Engine init
	@param [in] 	void
	@return 		void
	*/
	void initServiceManagerEngine();

	/**
	@brief ServiceManager Client deInit
	@param [in] 	void
	@return 		void
	*/
	//delete by shenghao for exit timeout 20180711
	//void deInitServiceManagerClient();

	/**
	@brief ServiceManager Engine deInit
	@param [in] 	void
	@return 		void
	*/
	void deInitServiceManagerEngine();

	/**
	@brief navi Exit Completed
	@param [in] 	void
	@return 		void
	*/
	//void naviExitCompleted();

	//logÊä³ö
	void ServiceManager_log_start();
	void ServiceManager_log_stop();

	/**
	@brief ServiceManager Notice Ready Forground
	@param [in] 	void
	@return 		void
	*/
	void ServiceManager_ReadyForground();
}
#endif /* __SERVICEMANAGER_COMMON_H__ */
