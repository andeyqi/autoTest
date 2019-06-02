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
		IDX_HOME = 0,   ///< ��ʾ���ҡ����±�
		IDX_OFFICE,		///< ��ʾ����˾�����±�
		IDX_ADDR_1,		///< ��ʾ�����õ�ַ1�����±�
		IDX_ADDR_2,		///< ��ʾ�����õ�ַ2�����±�
		IDX_ADDR_3,		///< ��ʾ�����õ�ַ3�����±�
		IDX_ADDR_INVALID,
	};
	enum ADDR_STATUS
	{
		ADDR_DEL = 0,  // δ����
		ADDR_ADD = 1,  // ����
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

	// ����ת��궨��
	enum ENUM_SPECIAL_ICON
	{
		ENUM_SPECIAL_ICON_NONE = 0x0000,		//������ͼ��
		ENUM_SPECIAL_ICON_FRONT_RIGHT = 0x0001,	//ǰ��������ʻ
		ENUM_SPECIAL_ICON_FRONT_LEFT = 0x0010,	//ǰ��������ʻ
		ENUM_SPECIAL_ICON_UTURN_RIGHT = 0x0011,	//�ҵ�ͷ
		ENUM_SPECIAL_ICON_UTURN_LEFT = 0x0100,	//���ͷ
		ENUM_SPECIAL_ICON_ROTARY_UTURN_RIGHT_OUT_SIDE = 0x0101,	//������ʻ������·��ͷ
		ENUM_SPECIAL_ICON_ROTARY_UTURN_LEFT_OUT_SIDE = 0x0110,	//������ʻ������·��ͷ
		ENUM_SPECIAL_ICON_TURN_RIGHT_UTURN = 0x0111,	//����ת�����ͷ
		ENUM_SPECIAL_ICON_TURN_LEFT_UTURN = 0x1000,		//����ת���ҵ�ͷ
		ENUM_SPECIAL_ICON_DEST_POINT = 0x1001,		//Ŀ�ĵأ��㣩
		ENUM_SPECIAL_ICON_VIA_POINT = 0x1010,		//;���㣨�㣩
		ENUM_SPECIAL_ICON_DEST_FLAG = 0x1011,		//Ŀ�ĵأ��죩
		ENUM_SPECIAL_ICON_VIA_FLAG = 0x1100,		//;���㣨�죩
		ENUM_SPECIAL_ICON_ROTARY_UTURN_RIGHT_IN_SIDE = 0x1101,	//������ʻ������·��ͷ
		ENUM_SPECIAL_ICON_ROTARY_UTURN_LEFT_IN_SIDE = 0x1110,	//������ʻ������·��ͷ
		ENUM_SPECIAL_ICON_TOLL_STATION = 0x10000,				// �շ�վ
		ENUM_SPECIAL_ICON_TUNNEL = 0x10001,						// ���
		ENUM_SPECIAL_ICON_DEST_ON_ROTARY_RIGHT_HAND = 0x10010,		// Ŀ�ĵ��ڻ�����������ʻ��
		ENUM_SPECIAL_ICON_START_ON_ROTARY_RIGHT_HAND = 0x10011,		// ��ʼ���ڻ�����������ʻ��
		ENUM_SPECIAL_ICON_DEST_ON_ROTARY_LEFT_HAND = 0x10100,		// Ŀ�ĵ��ڻ�����������ʻ��
		ENUM_SPECIAL_ICON_START_ON_ROTARY_LEFT_HAND = 0x10101,		// ��ʼ���ڻ�����������ʻ��
		ENUM_SPECIAL_ICON_WAYPOINTS1 = 0x10110,						//;����1
		ENUM_SPECIAL_ICON_WAYPOINTS2 = 0x10111,						//;����2
		ENUM_SPECIAL_ICON_WAYPOINTS3 = 0x11000,						//;����3
		ENUM_SPECIAL_ICON_WAYPOINTS4 = 0x11001,						//;����4
		ENUM_SPECIAL_ICON_WAYPOINTS5 = 0x11010,						//;����5

	};

	typedef struct TBTInfo
	{
		BOOL isOffLineTurnIconsOnRotary;		//�Ƿ��ǻ�������ת���
		BOOL isOffLineTurnIcons; 				//�Ƿ�������ת���
		TNaviTurnIconID turnIcon; 				//��ǰ��Ҫ��ʾ��ת���ͼ��ID
		TurnIconsOnRotary turnIconsOnRotary;  	//����ת���ͼ����Ϣ	
		int32 turnIconRemainDistance;   		//��ʾ��ת����ϵľ���ֵ����λ���ס�
		int32 routeLength;		  				//·�ߵ��ܳ��ȣ���λ���ס����û��·�ߣ���Ϊ��0
		int32 travelledDistance;  				//�Գ���·�����Ѿ���ʻ���ľ��룬��λ���ס����û��·�ߣ���Ϊ��0
		uint32 remainingTime;
	} TBTInfo;

	// ��ͼ�����Գ��н�����Ŀ�ĵط���
	typedef struct DirectionInfo
	{
		uint32 mapDirection;					//0δȷ����1�����ϣ�2��ͷ���ϣ�3δȷ��     
		uint32 carDirection; 					//�Գ��н�����
		int32 destDirection; 					//Ŀ�ĵط���
	} DirectionInfo;
	
	// ·��·����Ϣ
	typedef struct NextTBTInfo
	{
		cqWCHAR nextRoadName[64];					//��һ��·����
		uint32 distance; 						//��·�ھ���
		//cqWCHAR unit; 							//���뵥λ m
		uint32 routeBackInfo;  					//ǰ����·32��λ��Ϣ������1��32bit��int������
		uint32 isOnRotary;   					//�Ƿ��л��� 0:��,1����
		uint32 isOutSide;		  				//���������� Ĭ������ 0
		uint32 actualOutLine;  					//ʵ�ʳ���
		uint32 specialIcon;						//����ת���
		uint32 remainingDistance;				//���յ���� ��λm
		uint32 remainingTime;					//���յ�ʣ��ʱ�� ��λs
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

			// ��ȡ��ǰ·��
			//cqWCHAR* getCurrentRoadName(void);
			// ���õ�ǰ·��
			//void setCurrentRoadName(const cqWCHAR* curRoadName, size_t curRoadNameSize);
		private:
			Handle m_NaviDataTimer;//����״̬����֪ͨ����
			cqWCHAR* m_nextRoadName; // ��һ��·��
			cqWCHAR* m_currentRoadName; // ��ǰ·��
			Handle m_NaviCarMapInfoTimer;//������ͼģʽ����ͷ����Ŀ�ĵط���֪ͨTimer
	};
}

#endif /* __SERVICE_MANAGER_EXTERNAL_CLIENT_GUIDANCE_H__ */
