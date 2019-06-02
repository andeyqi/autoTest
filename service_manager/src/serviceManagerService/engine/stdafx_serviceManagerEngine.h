#ifndef __STDAFX_SERVICE_MANAGER_ENGINE_H__
#define __STDAFX_SERVICE_MANAGER_ENGINE_H__

// Basic
#include "cq_stdlib.h"
#include "cq_memory_dbg.h"
#include "cq_math.h"
#include "cqstl/vector.h"
#include "dalr/dalr_file.h"
#include "dalr/dalr_file_sys.h"

//#ifdef MAPBAR_TOYOTA_262AVN_LINUX
//#include "serviceManagerLog/LitesTraceDrc.h"
//#endif

#include "communication/serviceManagerCommonPublic.h"
#include "communication/serviceManagerCommunicationSender.h"
#include "communication/serviceManagerCommunicationReceiver.h"
#include "service_manager/serviceManagerEngine_Display.h"
#include "service_manager/serviceManagerEngine_Addition.h"
#include "service_manager/serviceManagerEngine_Voice.h"
#include "service_manager/serviceManagerEngine_NaviSystemStateNotify.h"
#include "service_manager/serviceManagerEngine_Guidance.h"

#include "logic/navi_session.h"

// mapdal
//extern "C"
//{
#include "mapdal/mapbar_dal_pub.h"
//}
#endif //__STDAFX_SERVICE_MANAGER_ENGINE_H__