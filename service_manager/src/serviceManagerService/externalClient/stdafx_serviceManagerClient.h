#ifndef __STDFAX_SERVICE_MANAGER_CLIENT_H__
#define __STDFAX_SERVICE_MANAGER_CLIENT_H__

/// MbApi

#define USE_NAVICORE_HEADER

//#ifdef MAPBAR_TOYOTA_262AVN_LINUX
//#include "serviceManagerLog/LitesTraceDrc.h"
//#endif

#include "mbapi/mbapi.h"

// 建议删掉下面函数的调用内容
#include "mbapi/mbapi_deleted.h"


// project
#include "communication/serviceManagerCommonPublic.h"
#include "communication/serviceManagerCommunicationSender.h"
#include "communication/serviceManagerCommunicationReceiver.h"

#include "serviceManagerService/externalClient/serviceManagerExternalClient_Display.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Guidance.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Addition.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_NaviSystemStateNotify.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_Voice.h"
#include "serviceManagerService/externalClient/serviceManagerExternalClient_VR.h"

#endif //__STDFAX_SERVICE_MANAGER_CLIENT_H__
