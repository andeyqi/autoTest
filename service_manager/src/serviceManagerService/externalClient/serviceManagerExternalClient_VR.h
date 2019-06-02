#ifndef __SERVICE_MANAGER_EXTERNAL_CLIENT_VR_H__
#define __SERVICE_MANAGER_EXTERNAL_CLIENT_VR_H__

#define SM_JSON_FILE_CUSTOM_KEYWORD											"CustomValue"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE							"NaviOption_hybridRouteType"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_JAM				"NaviOption_hybridRouteType_avoidJam"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_TOLL				"NaviOption_hybridRouteType_avoidToll"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_SHORTEST					"NaviOption_hybridRouteType_shortest"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_HIGHWAY_PRIORITY			"NaviOption_hybridRouteType_highwayPriority"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_HIGHWAY			"NaviOption_hybridRouteType_avoidHighway"
#define SM_JSON_FILE_NAVI_OPTION_HYBRID_ROUTE_TYPE_AVOID_FERRY				"NaviOption_hybridRouteType_avoidFerry"

#define	SM_JSON_FILE_NAVI_OPTION_MAP_SHOW_STATE_NOT_NAVIGATION_KEYWORD		"NaviOption_mapShowStateNotNavigation"
#define	SM_JSON_FILE_NAVI_OPTION_MAP_SHOW_STATE_NAVIGATION_KEYWORD			"NaviOption_mapShowStateNavigation"

namespace ServiceManager
{
	enum VR_FAV_TYPE
	{
		VR_FAV_TYPE_CURRENT_POINT = 0,
		VR_FAV_TYPE_DEST_POINT,
		VR_FAV_TYPE_POI_POINT
	};

	enum VR_ERR_TYPE
	{
		VR_ERROR_NONE = 0,
		VR_ERROR_PARAMETER,
		VR_ERROR_NO_PLAN,
		VR_ERROR_FAV_AREADY_ADD,
		VR_ERROR_FAV_ADD_FAILED,
		VR_ERROR_FAV_FULL,
	};
	

	typedef struct DBUS_CMD_DATA
	{
		int argc;
		cqCHAR** argv; 
	}DBUS_CMD_DATA;
	
	void VRSvcClient_Initialize();
	void VRSvcClient_CleanUp();

	class VRServiceClient : public MbApi::MessageDistribution::NaviSessionEventListener
	{
	public:
		VRServiceClient();
		virtual ~VRServiceClient();
		void initialize();

		BOOL onNaviSessionEventNotify(MbApi::MessageDistribution::NaviSessionEvent event, void* data);
	private:

	};
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_VR_H__ */