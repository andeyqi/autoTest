#ifndef __SERVICE_MANAGER_OUTPUT_H__
#define __SERVICE_MANAGER_OUTPUT_H__
#include <stdio.h>
#define SM_LOG_TEMP_BUFFER_SIZE 1024
#define SM_LOG_FORMAT "[%04d-%02d-%02d %02d:%02d:%02d][%s:%d|%s][%s]:%s\n"

#ifdef MAPBAR_WIN32
#define __SPRINTF(buffer,buffersize,format,...) { sprintf_s(buffer,buffersize,format, ##__VA_ARGS__); }
#else
#define __SPRINTF(buffer,buffersize,format,...) { buffersize; sprintf(buffer,format, ##__VA_ARGS__); }
#endif

#define SM_INFO_LOG(format,...) { \
	cqCHAR _temp_log_buffer[SM_LOG_TEMP_BUFFER_SIZE]; \
	__SPRINTF(_temp_log_buffer, SM_LOG_TEMP_BUFFER_SIZE, format, ##__VA_ARGS__); \
	ServiceManagerOutput::getInstance()->Log_write(__FILE__, __FUNCTION__, __LINE__, SM_LOG_LEVEL_INFO, _temp_log_buffer);}

#define SM_WARNING_LOG(format,...) { \
	cqCHAR _temp_log_buffer[SM_LOG_TEMP_BUFFER_SIZE]; \
	__SPRINTF(_temp_log_buffer, SM_LOG_TEMP_BUFFER_SIZE, format, ##__VA_ARGS__); \
	ServiceManagerOutput::getInstance()->Log_write(__FILE__, __FUNCTION__, __LINE__, SM_LOG_LEVEL_WARNING, _temp_log_buffer); }

#define SM_ERROR_LOG(format,...) { \
	cqCHAR _temp_log_buffer[SM_LOG_TEMP_BUFFER_SIZE]; \
	__SPRINTF(_temp_log_buffer, SM_LOG_TEMP_BUFFER_SIZE, format, ##__VA_ARGS__); \
	ServiceManagerOutput::getInstance()->Log_write(__FILE__, __FUNCTION__, __LINE__, SM_LOG_LEVEL_ERROR, _temp_log_buffer); }

extern void Mapbar_drclog_printf(const char* format, ...);

namespace ServiceManager
{	
	//extern "C"
	//{
		//json_t* LogToJson();

//		typedef enum AuthError
//		{
//			/// 无错误
//			AuthError_none = 0,
//			/// 设备ID读取错误
//			AuthError_deviceIdReaderError = 1,
//			/// license.dat文件读取错误
//			AuthError_licenseIoError = 2,
//			/// license.dat文件格式错误
//			AuthError_licenseFormatError = 3,
//			/// license.dat不存在
//			AuthError_licenseMissing = 4,
//			/// license.dat存在，且有效，但是不是针对当前产品的
//			AuthError_licenseIncompatible = 5,
//			/// license.dat记录的设备ID与当前设备ID不匹配
//			AuthError_licenseDeviceIdMismatch = 6,
//			/// 数据文件权限已经过期
//			AuthError_expired = 7,
//			/// 数据未授权
//			AuthError_noPermission = 8,
//			/// 其他错误
//			AuthError_otherError = 9,
//			/// 数据版本受限，仅能使用低于某版本的数据，不能升级
//			/// JIRA: NAVICORE-1948
//			AuthError_dataVersionLimit = 10,
//			/// 数据文件中时间不匹配
//			/// 比如：当前时间比上次更新时间小
//			AuthError_datetimeMismatch = 11
//		} AuthError;
//
//		const cqWCHAR* RegionList_buildFileName(const cqWCHAR* filename);
//#define ROUTE_BIN_FILENAME RegionList_buildFileName(L"userdata/route.json")
//		//#define ROUTE_BIN_FILENAME L"other/route.json"
//		BOOL FileSys_delete(const cqWCHAR* fileName);
//		typedef struct GenericFileVTable GenericFileVTable;
//		typedef struct FileObject
//		{
//#define	FILE_OBJECT_MEMBERS	\
//	const GenericFileVTable* m_vt;	\
//	uint32 m_initializeFlag;	/* for debug purpose. If the constructor is called, it will be initialized to 0xfefefefe;*/ \
//	cqWCHAR* m_fileName;	\
//	AuthError m_authError;
//			FILE_OBJECT_MEMBERS;
//			uint32 reserved[24];
//		} FileObject;
	//	void File_construct(FileObject* o);

		//typedef enum FileOpenFlag
		//{
		//	File_OpenForRead = 1,
		//	File_OpenForWrite = 2,
		//	File_OpenForReadWrite = (1 | 2),
		//	File_CreateAlways = 4,
		//	File_Fallible = 8
		//} FileOpenFlag;

	//	BOOL File_open(FileObject* o, const cqWCHAR* fileName, FileOpenFlag openMode);
//#define File_openFallible(o, filename, openMode) File_open(o, filename, (FileOpenFlag)((openMode) | File_Fallible))
	//	size_t File_write(FileObject* o, const void* buffer, size_t size);
	//	void File_close(FileObject* o);
	//	void File_destruct(FileObject* o);
	//	BOOL FileSys_pathFileExists(const cqWCHAR* fileName);

		// if succeed, you should call findNext and findClose.
		// if failed, you shouldn't call findClose.
	//	BOOL FileSys_findFirst(const cqWCHAR* pathAndPattern, // for example "tracks/*.trk"
	//		BOOL* isFile,
	//		cqWCHAR* fileName,
	//		size_t maxLen,
	//		Handle* findHandle);

	//	BOOL FileSys_findNext(Handle findHandle,
	//		BOOL* isFile,
	//		cqWCHAR* fileName,
	//		size_t maxLen);

	//	void FileSys_findClose(Handle findHandle);

		/**	@brief Check if a file is opened.
		@memberof FileObject
		*/
	//	BOOL File_isOpened(FileObject* o);

	//	BOOL FileSys_createDir(const cqWCHAR * path);

	//	ssize_t File_getLength(FileObject* o);
	//	int64 File_getLength64(FileObject* o);
	//	size_t File_read(FileObject* o, void* buffer, size_t size);
//#define cq_min(x, y)    (((x) > (y)) ? (y) : (x))
//#define HISTORY_ROUTEPLAN_MAX_NUM 49

//	}
	
	#define DBUS_MAX_MSG_SIZE 512
	
	enum SM_LOG_LEVEL
	{
		SM_LOG_LEVEL_VERBOSE = 0,	// 最详细
		SM_LOG_LEVEL_TRACE,			// 函数运行情况
		SM_LOG_LEVEL_INFO,			// 较为重要的数据
		SM_LOG_LEVEL_WARNING,		// 警告，可能不会发生问题
		SM_LOG_LEVEL_ERROR,			// 错误，会引发问题
		SM_LOG_LEVEL_MAX,			// 关闭LOG
	};

	enum SM_LOG_OUTPUT
	{
		SM_LOG_OUTPUT_FILE = 0,
		SM_LOG_OUTPUT_CONSOLE,
		SM_LOG_OUTPUT_MAX,
	};

	typedef enum NaviDataType {
		NaviDataType_debus,
		NaviDataType_tbt,
		NaviDataType_gps,
		NaviDataType_rev,
		NaviDataType_default
	} NaviDataType;

	typedef struct NaviDebusInfo
	{
		cqCHAR debusMessage[DBUS_MAX_MSG_SIZE];
		int32 tbtId;

	} NaviDebusInfo;

	typedef struct NaviReceiverInfo
	{
		cqCHAR debusMessage[DBUS_MAX_MSG_SIZE];
		void(*callBackAdd)(void *data);
		int32 receFuncId;
		int32 infoId;

	} NaviReceiverInfo;

	typedef struct NaviData
	{
		union {
			NaviDebusInfo  DebusInfo;
			NaviReceiverInfo ReceInfo;
		};
		//NaviDataType dataType; /* 上传模块内部使用, push data时无需赋值 */
	} NaviData;

	
	
	class ServiceManagerOutput
	{
	public:
		static ServiceManagerOutput* getInstance();
		BOOL outputLog(NaviDataType type, NaviData *data);
		BOOL outPutInfoLog();
		BOOL outPutErrorLog();
		void Log_write(const cqCHAR* file, const cqCHAR* function, int32 line, SM_LOG_LEVEL level, const cqCHAR* log_append);
		void creatLogFile();
		void closeLogFile();

	private:
		ServiceManagerOutput();
		virtual ~ServiceManagerOutput();

		void deleteOverdueFiles(void);
		void fatalLogOutput(const cqCHAR* log);
		const cqCHAR* trimFullPathFileName(const cqCHAR* fileName);
	private:
		void outputKeyAndValue(const cqCHAR* key, int32 value);
		void outputKeyAndLongValue(const cqCHAR* key, int64 value);
		void outputKeyAndChar(const cqCHAR* key, cqCHAR* str);
		void outputKeyAndWChar(const cqWCHAR* key, const cqWCHAR* wstr);
		void outputKeyAndPoint(const cqCHAR* key, const Point* pos);
		void outputKeyAndDateTime(const cqCHAR* key, const DateTime* dt);
		void outputKeyAndpFuncAdd(const cqCHAR* key,int32 value, void(*callBackAdd)(void *data));
	private:
		void outputDebusInfo(NaviDebusInfo* data);
		void outputTBTInfo(NaviDebusInfo* data);
		void outputGpsInfo(NaviDebusInfo* data);
		void outputRevInfo(NaviReceiverInfo* data);
		void outputDefaultInfo(NaviDebusInfo* data);

	private:
		class CGarbo // 它的唯一工作就是在析构函数中删除DatastoreOnlineHandler的实例  
		{
		public:
			~CGarbo()
			{
				if (ServiceManagerOutput::m_instance != NULL)
					delete(ServiceManagerOutput::m_instance);
			}
		};

	private:
		static ServiceManagerOutput* m_instance;

		struct  FileObject *m_file;

		static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
	};
}
#endif //__RECORD_COLLECTION_LOGIC_H__
