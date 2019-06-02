//#include "stdafx_serviceManagerClient.h"
#include "stdafx_serviceManagerEngine.h"
#include "serviceManagerOutput.h"
#include "dalr/dalr_clock.h"

#define SERVICEMANAGER_LOG_PATH L"./log/service_manager_log"
#define SERVICEMANAGER_LOG_PATH_CAT L"./log/service_manager_log/"
#define SERVICEMANAGER_LOG_FILE_PATTERN L"./service_manager_log/*.*"
#define SERVICEMANAGER_LOG_FILES_MAX_NUM 10
#define CQ_MAX_PATH	128

namespace ServiceManager
{
	
	ServiceManagerOutput* ServiceManagerOutput::m_instance = NULL;

	ServiceManagerOutput* ServiceManagerOutput::getInstance()
	{
		if (m_instance == NULL)
		{
			m_instance = new(ServiceManagerOutput);
		}

		return m_instance;
	}

	ServiceManagerOutput::ServiceManagerOutput()
	{
		// deleteOverdueFiles();
		//creatLogFile();
		m_file = new FileObject();
		File_construct(m_file);
	}

	ServiceManagerOutput::~ServiceManagerOutput()
	{
		closeLogFile();
		File_destruct(m_file);
	}

	void RecordCollection_getSystemTime(DateTime* dateTime)
	{
		DateTime_getSystemTime(dateTime);
		//delete by shenghao 
		//DateTime_addTimeZone(dateTime, 8);
	}

	void ServiceManagerOutput::creatLogFile()
	{
		cqWCHAR fileName[CQ_MAX_PATH];

		if (File_isOpened(m_file))
			return;

		if (!FileSys_pathFileExists(SERVICEMANAGER_LOG_PATH))
		{
			FileSys_createDir(SERVICEMANAGER_LOG_PATH);
		}

		cqWCHAR* filename = (cqWCHAR *)malloc(CQ_MAX_PATH * sizeof(cqWCHAR));

		if (filename != NULL)
		{
			filename[0] = 0;
			uint32 i = 0;
			cqWCHAR num[16];
			do
			{
				cq_itow(i, num, 10);
				cq_wcscpy_s(filename, CQ_MAX_PATH, SERVICEMANAGER_LOG_PATH_CAT);
				cq_wcscat_s(filename, CQ_MAX_PATH, L"service_manager_log_");
				cq_wcscat_s(filename, CQ_MAX_PATH, num);
				cq_wcscat_s(filename, CQ_MAX_PATH, L".log");
				i++;
			} while (FileSys_pathFileExists(filename));

			cq_swprintf(fileName, filename);
			free(filename);

			if (!File_open(m_file, fileName, (FileOpenFlag)(File_OpenForWrite | File_CreateAlways)))
			{
				File_destruct(m_file);
			}
		}
	}

	void ServiceManagerOutput::closeLogFile()
	{
		if (File_isOpened(m_file))
		{
			File_close(m_file);
			File_destruct(m_file);
		}
	}

	void ServiceManagerOutput::deleteOverdueFiles(void)
	{
		cqstd::vector<cqWCHAR*> fileList;
		BOOL isFile;
		cqWCHAR fileName[1024];
		cqWCHAR* logFile;
		Handle findHandle;
		cqWCHAR fileNameWithPath[1024];
		size_t i, num;

		if (!FileSys_pathFileExists(SERVICEMANAGER_LOG_PATH))
		{
			return;
		}

		if (FileSys_findFirst(SERVICEMANAGER_LOG_FILE_PATTERN, &isFile, fileName, element_of(fileName), &findHandle))
		{
			if (isFile)
			{
				logFile = (cqWCHAR*)malloc((cq_wcslen(fileName) + 1) * sizeof(cqWCHAR));
				if (NULL == logFile)
				{
					return;
				}
				cq_wcscpy(logFile, fileName);
				fileList.push_back(logFile);
			}

			while (FileSys_findNext(findHandle, &isFile, fileName, element_of(fileName)))
			{
				if (isFile)
				{
					logFile = (cqWCHAR*)malloc((cq_wcslen(fileName) + 1) * sizeof(cqWCHAR));
					if (NULL == logFile)
					{
						return;
					}
					cq_wcscpy(logFile, fileName);
					fileList.push_back(logFile);
				}
			}

			FileSys_findClose(findHandle);
		}

		num = fileList.size();

		for (i = 0; i < num; i++)
		{
			logFile = fileList.at(i);

			if (logFile == NULL)
				continue;

			if (num - i > SERVICEMANAGER_LOG_FILES_MAX_NUM)
			{
				fileNameWithPath[0] = 0;
				cq_wcscat(fileNameWithPath, SERVICEMANAGER_LOG_PATH);
				cq_wcscat(fileNameWithPath, L"/");
				cq_wcscat(fileNameWithPath, logFile);
				FileSys_delete(fileNameWithPath);
			}
		}
		if (logFile != NULL)
		{
			free(logFile);
			logFile = NULL;
		}

		fileList.clear();
	}

	void ServiceManagerOutput::fatalLogOutput(const cqCHAR* log)
	{
		if (log == NULL)
		{
			return;
		}
		UNUSED_VAR(log);
		cqCHAR* log_buffer = (cqCHAR*)malloc(cq_strlen(log) + 256);
		//cqWCHAR* w_log_buffer = (cqWCHAR*)malloc((cq_strlen(log) + 256) * 2);
		if (log_buffer == NULL)
		{
			return;
		}
		log_buffer[0] = 0;
		//w_log_buffer[0] = 0;

		if (File_isOpened(m_file))
		{
			DateTime dt = { 0 };
			RecordCollection_getSystemTime(&dt);

			cq_sprintf(log_buffer, "DATE: %04d-%02d-%02d %02d:%02d:%02d %s \n",
				dt.m_year, dt.m_month, dt.m_day, dt.m_hours, dt.m_minutes, dt.m_seconds, log);
			File_write(m_file, log_buffer, cq_strlen(log_buffer) * sizeof(cqCHAR));
		}
		if (log_buffer != NULL)
		{
			free(log_buffer);
			log_buffer = NULL;
		}
		//free(w_log_buffer);
		//w_log_buffer = NULL;
		return;
	}

	void ServiceManagerOutput::Log_write(const cqCHAR* file, const cqCHAR* function, int32 line, SM_LOG_LEVEL level, const cqCHAR* log_append)
	{
		cqCHAR log_buffer[1024] = { 0 };
		static int32 g_lineCount = 0;

		if (File_isOpened(m_file))
		{
			DateTime dt = { 0 };
			RecordCollection_getSystemTime(&dt);

			const cqCHAR* levelString = "UNKOWN";
			if (level == SM_LOG_LEVEL_ERROR)	levelString = "ERROR";
			else if (level == SM_LOG_LEVEL_WARNING)	levelString = "WARNING";
			else if (level == SM_LOG_LEVEL_INFO)	levelString = "INFO";
			else if (level == SM_LOG_LEVEL_TRACE)	levelString = "TRACE";
			else if (level == SM_LOG_LEVEL_VERBOSE)	levelString = "VERBOSE";
			
			__SPRINTF(log_buffer, element_of(log_buffer), SM_LOG_FORMAT, dt.m_year, dt.m_month, dt.m_day, dt.m_hours, dt.m_minutes, dt.m_seconds, trimFullPathFileName(file), line, function, levelString, log_append);
		
			File_write(m_file, log_buffer, cq_strlen(log_buffer) * sizeof(cqCHAR));
		}


		/*if (g_logLevel[SM_LOG_OUTPUT_CONSOLE] <= level)
		{*/
			//if (level == SM_LOG_LEVEL_ERROR)
			//{
			//	CQ_LOG_ERROR("[MbApi] %s", log_append);
			//}
			//else if (level == SM_LOG_LEVEL_WARNING)
			//{
			//	CQ_LOG_WARNING("[MbApi] %s", log_append);
			//}
			//else if (level == SM_LOG_LEVEL_INFO)
			//{
			//	cq_sprintf(log_buffer, "[MbApi] FUNCTION: %s LINE: %d %s", function, line, log_append);
			//	CQ_LOG_INFO(log_buffer);
			//}
			//else if (level == SM_LOG_LEVEL_TRACE)
			//{
			//	cq_sprintf(log_buffer, "[MbApi] FUNCTION: %s LINE: %d %s", function, line, log_append);
			//	CQ_TRACE(log_buffer, 0);
			//}
			//else if (level == SM_LOG_LEVEL_VERBOSE)
			//{
			//	cq_sprintf(log_buffer, "[MbApi] FUNCTION: %s LINE: %d %s", function, line, log_append);
			//	CQ_LOG_TRIVIAL(log_buffer);
			//}
		//}
		return;
	}

	const cqCHAR* ServiceManagerOutput::trimFullPathFileName(const cqCHAR* fileName)
	{
		size_t len = cq_strlen(fileName);
		int32 loopIdx;
		for (loopIdx = len - 1; loopIdx >= 0; --loopIdx)
		{
			if (fileName[loopIdx] == '/' || fileName[loopIdx] == '\\')
				break;
		}

		return fileName + loopIdx + 1;	
	}

	void ServiceManagerOutput::outputKeyAndChar(const cqCHAR* key, cqCHAR* str)
	{
		cqCHAR buffer[1024];
		cq_sprintf(buffer, "%s : %s", key, str);

		fatalLogOutput(buffer);
	}

	void ServiceManagerOutput::outputKeyAndValue(const cqCHAR* key, int32 value)
	{
		cqCHAR buffer[1024];
		cq_sprintf(buffer, "%s : %d", key, value);

		fatalLogOutput(buffer);
	}

	void ServiceManagerOutput::outputKeyAndpFuncAdd(const cqCHAR* key,int32 value, void(*callBackAdd)(void *data))
	{
		cqCHAR buffer[1024];
		cq_sprintf(buffer, "%s : %d : %04x", key, value, *callBackAdd);
		fatalLogOutput(buffer);
	}

	void ServiceManagerOutput::outputDebusInfo(NaviDebusInfo* data)
	{
		outputKeyAndChar("NaviDebusInfo.debusMessage", data->debusMessage);
	}

	void ServiceManagerOutput::outputRevInfo(NaviReceiverInfo* data)
	{
		outputKeyAndpFuncAdd(data->debusMessage, data->receFuncId, data->callBackAdd);
	}

	void ServiceManagerOutput::outputTBTInfo(NaviDebusInfo* data)
	{
		outputKeyAndValue("NaviDebusInfo.tbtId", data->tbtId);
	}

	BOOL ServiceManagerOutput::outputLog(NaviDataType type, NaviData *data)
	{
		//outputDebusInfo(&data->DebusInfo);

		switch (type)
		{
		case NaviDataType_debus:
			outputDebusInfo(&data->DebusInfo);
			break;
		case NaviDataType_tbt:
			outputTBTInfo(&data->DebusInfo);
			break;
		case NaviDataType_gps:
			//outputGpsInfo(&data->searchInfo);
			break;
		case NaviDataType_rev:
			outputRevInfo(&data->ReceInfo);
			break;
		case NaviDataType_default:
			//outputDefaultInfo(&data->response);
			break;
		default:
			break;
		}
		return TRUE;
	}

}