#ifndef __CJOB_H__
#define __CJOB_H__

#include "cq_stdlib.h"
#include "mapdal/mapbar_dal_pub.h"

class CJob
{
public:
	CJob(void) { m_data = NULL, m_jobNo = 0, m_jobName = NULL; }
	CJob(void *data) { setJobData(data); }

	void setJobData(void* data) { m_data = data; }
	void* getJobdata() { return m_data; }
	int getJobNo(void) const { return m_jobNo; }
	void setJobNo(int jobno){ m_jobNo = jobno; }
	char* getJobName(void) const { return m_jobName; }
	void setJobName(char* jobname) { m_jobName = jobname; }

	virtual	void run() = 0;

private:
	void*	m_data;
	int		m_jobNo;        //The num was assigned to the job 
	char*	m_jobName;      //The job name 
};

#endif