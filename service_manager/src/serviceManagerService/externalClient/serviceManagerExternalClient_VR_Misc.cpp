#include "stdafx_serviceManagerClient.h"
#include "mapdal/message_loop.h"
 
using namespace MbApi;
namespace ServiceManager
{
	static void VRGetIsNaviHaveDestPoint(void * data)
	{
		UNUSED_VAR(data);
		ServiceManager::SendData* paramData = (ServiceManager::SendData*)malloc(sizeof(ServiceManager::SendData));
		/* 根据当前的plan的点的数量确认是否已经设定目的地 */
		MbApi::RouteService::Plan plan = MbApi::RouteApi_getPlan();
	
		paramData->funcId = FuncID_Send_GetDestPointInfoResp;
		(int32_t *)(paramData->data) = (int32_t *)malloc(sizeof(int32_t));
		*((int32_t *)(paramData->data)) = (plan && (plan.getPointNum() > 1)) ? 0 : 1 ;
		NaviSysSender::getInstance()->sendMsg((void*)paramData, LOW_PRIO);
	}
	

	static void VRGetIsNaviHaveDestPointSig(void* data)
	{
		MbApi::Mapbar_runFunctionInMainThread((Mapbar_mainThreadFunc)VRGetIsNaviHaveDestPoint, data);
	}
	
	void VRMiscInit(void)
	{
		NaviSysReceiver::getInstance()->setCallback(FuncID_Receive_NaviIsHaveDest, VRGetIsNaviHaveDestPoint);
	}
}
