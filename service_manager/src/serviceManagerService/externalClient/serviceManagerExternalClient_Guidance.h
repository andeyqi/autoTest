#ifndef __SERVICE_MANAGER_EXTERNAL_CLIENT_GUIDANCE_H__
#define __SERVICE_MANAGER_EXTERNAL_CLIENT_GUIDANCE_H__

//#include "cq_stdlib/cq_stdlib.h"
#include "mbapi/mbapi_enum.h"
#include "mbapi/mbapi_guidance.h"
#include "mbapi/mbapi_struct.h"
#include "poiservice/poi_service_IF.h"
#include "mbapi/mbapi_value.h"
#include "mbapi/mbapi_data.h"
#include "mbapi/mbapi_message_distribution.h"

#define MAX_SIZE 80
namespace ServiceManager
{
	#define TIMER_TBT_1_SECOND	1000

	// initialize
	void GuidanceServiceClient_Initialize(void);

	//delete by shenghao for exit timeout
	//cleanup
	//void GuidanceServiceClient_CleanUp(void);

	enum IDX_INFO
	{
		IDX_HOME = 0,   ///< 表示“家”的下标
		IDX_OFFICE,		///< 表示“公司”的下标
		IDX_ADDR_1,		///< 表示“常用地址1”的下标
		IDX_ADDR_2,		///< 表示“常用地址2”的下标
		IDX_ADDR_3,		///< 表示“常用地址3”的下标
		IDX_ADDR_INVALID,
	};
	enum ADDR_STATUS
	{
		ADDR_DEL = 0,  // 未设置
		ADDR_ADD = 1,  // 设置
	};

	enum BIT_Position
	{
		BIT_Position_1 = 1,
		BIT_Position_2,
		BIT_Position_3,
		BIT_Position_4,
		BIT_Position_5,
		BIT_Position_6,
		BIT_Position_7,
		BIT_Position_8,
		BIT_Position_9,
		BIT_Position_10,
		BIT_Position_11,
		BIT_Position_12,
		BIT_Position_13,
		BIT_Position_14,
		BIT_Position_15,
		BIT_Position_16,
		BIT_Position_17,
		BIT_Position_18,
		BIT_Position_19,
		BIT_Position_20,
		BIT_Position_21,
		BIT_Position_22,
		BIT_Position_23,
		BIT_Position_24,
		BIT_Position_25,
		BIT_Position_26,
		BIT_Position_27,
		BIT_Position_28,
		BIT_Position_29,
		BIT_Position_30,
		BIT_Position_31,
		BIT_Position_invalid,
	};

	// 特殊转向标定义
	enum ENUM_SPECIAL_ICON
	{
		ENUM_SPECIAL_ICON_NONE = 0x0000,		//无特殊图标
		ENUM_SPECIAL_ICON_FRONT_RIGHT = 0x0001,	//前方靠右行驶
		ENUM_SPECIAL_ICON_FRONT_LEFT = 0x0010,	//前方靠左行驶
		ENUM_SPECIAL_ICON_UTURN_RIGHT = 0x0011,	//右掉头
		ENUM_SPECIAL_ICON_UTURN_LEFT = 0x0100,	//左掉头
		ENUM_SPECIAL_ICON_ROTARY_UTURN_RIGHT_OUT_SIDE = 0x0101,	//靠右行驶环岛外路掉头
		ENUM_SPECIAL_ICON_ROTARY_UTURN_LEFT_OUT_SIDE = 0x0110,	//靠左行驶环岛外路掉头
		ENUM_SPECIAL_ICON_TURN_RIGHT_UTURN = 0x0111,	//先右转再左掉头
		ENUM_SPECIAL_ICON_TURN_LEFT_UTURN = 0x1000,		//先左转再右掉头
		ENUM_SPECIAL_ICON_DEST_POINT = 0x1001,		//目的地（点）
		ENUM_SPECIAL_ICON_VIA_POINT = 0x1010,		//途径点（点）
		ENUM_SPECIAL_ICON_DEST_FLAG = 0x1011,		//目的地（旗）
		ENUM_SPECIAL_ICON_VIA_FLAG = 0x1100,		//途径点（旗）
		ENUM_SPECIAL_ICON_ROTARY_UTURN_RIGHT_IN_SIDE = 0x1101,	//靠右行驶环岛内路掉头
		ENUM_SPECIAL_ICON_ROTARY_UTURN_LEFT_IN_SIDE = 0x1110,	//靠左行驶环岛内路掉头
		ENUM_SPECIAL_ICON_TOLL_STATION = 0x10000,				// 收费站
		ENUM_SPECIAL_ICON_TUNNEL = 0x10001,						// 隧道
		ENUM_SPECIAL_ICON_DEST_ON_ROTARY_RIGHT_HAND = 0x10010,		// 目的地在环岛（靠右行驶）
		ENUM_SPECIAL_ICON_START_ON_ROTARY_RIGHT_HAND = 0x10011,		// 起始点在环岛（靠右行驶）
		ENUM_SPECIAL_ICON_DEST_ON_ROTARY_LEFT_HAND = 0x10100,		// 目的地在环岛（靠左行驶）
		ENUM_SPECIAL_ICON_START_ON_ROTARY_LEFT_HAND = 0x10101,		// 起始点在环岛（靠左行驶）
		ENUM_SPECIAL_ICON_WAYPOINTS1 = 0x10110,						//途经点1
		ENUM_SPECIAL_ICON_WAYPOINTS2 = 0x10111,						//途经点2
		ENUM_SPECIAL_ICON_WAYPOINTS3 = 0x11000,						//途经点3
		ENUM_SPECIAL_ICON_WAYPOINTS4 = 0x11001,						//途经点4
		ENUM_SPECIAL_ICON_WAYPOINTS5 = 0x11010,						//途经点5

	};

	typedef struct TBTInfo
	{
		BOOL isOffLineTurnIconsOnRotary;		//是否是环岛离线转向标
		BOOL isOffLineTurnIcons; 				//是否是离线转向标
		TNaviTurnIconID turnIcon; 				//当前需要显示的转向标图标ID
		TurnIconsOnRotary turnIconsOnRotary;  	//环岛转向标图标信息	
		int32 turnIconRemainDistance;   		//显示在转向标上的距离值，单位：米。
		int32 routeLength;		  				//路线的总长度，单位：米。如果没有路线，则为：0
		int32 travelledDistance;  				//自车在路线上已经行驶过的距离，单位：米。如果没有路线，则为：0
		uint32 remainingTime;
	} TBTInfo;

	// 地图朝向、自车行进方向、目的地方向
	typedef struct DirectionInfo
	{
		uint32 mapDirection;					//0未确定，1北向上，2车头向上，3未确定     
		uint32 carDirection; 					//自车行进方向
		int32 destDirection; 					//目的地方向
	} DirectionInfo;
	
	// 路口路线信息
	typedef struct NextTBTInfo
	{
		cqWCHAR nextRoadName[64];					//下一条路名称
		uint32 distance; 						//距路口距离
		//cqWCHAR unit; 							//距离单位 m
		uint32 routeBackInfo;  					//前方道路32方位信息，参数1是32bit的int型整数
		uint32 isOnRotary;   					//是否有环岛 0:无,1：有
		uint32 isOutSide;		  				//环岛内外向 默认外向 0
		uint32 actualOutLine;  					//实际出边
		uint32 specialIcon;						//特殊转向标
		uint32 remainingDistance;				//距终点距离 单位m
		uint32 remainingTime;					//距终点剩余时间 单位s
	} NextTBTInfo;

	class GuidanceServiceClient : public MbApi::MessageDistribution::GeneralEventListener,
		public MbApi::MessageDistribution::NaviSessionEventListener, MbApi::MessageDistribution::NavigationStateListener, MbApi::MessageDistribution::MapViewEventListener
	{
		public:
			GuidanceServiceClient();
			virtual ~GuidanceServiceClient();
			void initialize();
		
			BOOL onGeneralEventNotify(MbApi::MessageDistribution::GeneralEvent event, void* data);
			BOOL onNaviSessionEventNotify(MbApi::MessageDistribution::NaviSessionEvent event, void* data);
			BOOL onNavigationStateNotify(RealNavigationState state);
			BOOL onMapViewEventNotify(MbApi::MessageDistribution::MapViewEvent event, void* data);

			// 获取当前路名
			//cqWCHAR* getCurrentRoadName(void);
			// 设置当前路名
			//void setCurrentRoadName(const cqWCHAR* curRoadName, size_t curRoadNameSize);
		private:
			Handle m_NaviDataTimer;//导航状态数据通知周期
			cqWCHAR* m_nextRoadName; // 下一条路名
			cqWCHAR* m_currentRoadName; // 当前路名
			Handle m_NaviCarMapInfoTimer;//导航地图模式、车头方向、目的地方向通知Timer
	};
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_GUIDANCE_H__ */
