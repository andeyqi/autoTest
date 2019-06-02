#ifndef __STDAFX_SERVICE_MANAGER_H__
#define __STDAFX_SERVICE_MANAGER_H__

	/// Basic
#include "cq_stdlib.h"
#include "cq_memory_dbg.h"
#include "cq_math.h"
#include "cqstl/vector.h"

#include "mapdal/mapbar_dal_pub.h"

#include "communication/serviceManagerCommonPublic.h"
#include "communication/serviceManagerCommunicationSender.h"
#include "communication/serviceManagerCommunicationReceiver.h"
#include "communication/serviceManagerCommunicationIpcWrapper.h"
//#include "communication/serviceManagerOutput.h"

#include "service_manager/serviceManagerEngine_Addition.h"
#include "service_manager/serviceManagerEngine_Display.h"
#include "service_manager/serviceManagerEngine_Guidance.h"
#include "service_manager/serviceManagerEngine_NaviSystemStateNotify.h"
#include "service_manager/serviceManagerEngine_Voice.h"

//#include "serviceManagerService/externalClient/serviceManagerExternalClient_Addition.h"
//#include "serviceManagerService/externalClient/serviceManagerExternalClient_Display.h"
//#include "serviceManagerService/externalClient/serviceManagerExternalClient_Guidance.h"
//#include "serviceManagerService/externalClient/serviceManagerExternalClient_NaviSystemStateNotify.h"
//#include "serviceManagerService/externalClient/serviceManagerExternalClient_Voice.h"

#ifdef MAPBAR_WIN32
#include "communication/ipcwrapper_win32/serviceManagerCommunicationIpcWrapperWin32.h"
#elif MAPBAR_DESAY
#include "communication/ipcwrapper_desay/serviceManagerCommunicationIpcWrapperDeSay.h"
#elif MAPBAR_TOYOTA
#include "communication/ipcwrapper_toyota/serviceManagerCommunicationIpcWrapperToyota.h"
#else
#endif

#include "communication/threadManager/CJob.h"
#include "communication/threadManager/CWorkerThread.h"
#include "communication/threadManager/CThreadPool.h"

#endif //__STDAFX_SERVICE_MANAGER_H__