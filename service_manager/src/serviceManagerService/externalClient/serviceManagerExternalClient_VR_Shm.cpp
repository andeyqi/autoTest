#ifdef MAPBAR_TOYOTA_262AVN_LINUX
#include "stdafx_serviceManagerClient.h"
#include <stdio.h>
#include <string.h>
#include<errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>

#define SHM_NAVI "/SHM_VR_NAVI"
#define SEM_NAVI "/SEM_VR_NAVI"
#define SHMSIZE 15000

static char* ptr = NULL;
static sem_t* sem = NULL;

/**
@brief 打开同步信号量，信号量不存在时则创建
@return         void
@details
*/

static int sem_Open(void)
{
    sem = sem_open( SEM_NAVI, (O_RDWR | O_CREAT), 0666, 1 );
    if( sem == SEM_FAILED )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] sem_open error, the error is %s\n", strerror(errno));
        return -1;
    }
    else
    {
        CQ_LOG_INFO("[Servrce manger VR shm] sem open [ok]\n");
        return 0;
    }
}

/**
@brief 消除信号量资源
@return         void
@details
*/

static int sem_Delete(void)
{
    int ret;
    
    ret = sem_unlink( SEM_NAVI );
    if( ret < 0 )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] sem_unlink error, the error is %s\n", strerror(errno));
        return -1;
    }
    else
    {
        CQ_LOG_INFO("[Servrce manger VR shm] sem_unlink [ok]\n");
        return 0;
    }
}

/**
@brief 获取信号量锁资源
@return         void
@details
*/

static int sem_Take(void)
{
    int ret;
    
    ret = sem_wait( sem );
    if( ret < 0 )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm]  sem_wait error, the error is %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/**
@brief 获取信号量锁资源
@return         void
@details
*/

static int sem_TakeT(int tims)
{
    int ret;
    int sec;
    struct timespec timeout;
    
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec  += (tims / 1000);
    timeout.tv_nsec += (tims % 1000) * 1000 * 1000;
    if (timeout.tv_nsec >= 1000000000)
    {
        sec = timeout.tv_nsec / 1000000000;
        timeout.tv_sec  += sec;
        timeout.tv_nsec -= 1000000000 * sec;
    }
    
RETRY:
    ret = sem_timedwait(sem, &timeout);

    if (ret < 0 )
    {
        if ( errno == EINTR )
        {
            goto RETRY;
        }
        CQ_LOG_ERROR("[Servrce manger VR shm] sem_timedwait error, the error is %s\n", strerror(errno));
    }
    return 0;
}

/**
@brief 释放信号量锁资源
@return         void
@details
*/

static int sem_Give(void)
{
    int ret;
    
    ret = sem_post( sem );
    CQ_LOG_INFO("[Servrce manger VR shm] sem_post ret is [%d]\n",ret);
    if( ret < 0 )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] sem_post error, the error is %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/**
@brief 共有内存初始化
@return         void
@details
*/

static int shm_Init(void)
{
    int ret = 0;
    int fd;
    fd = shm_open( SHM_NAVI, (O_RDWR | O_CREAT), 0666 );
    if( fd < 0 )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] shm_open error, the error is %s\n", strerror(errno));
        return -1;
    }
    CQ_LOG_INFO("[Servrce manger VR shm] shm_open fd is [%d]\n",fd);
    ret = ftruncate(fd, SHMSIZE);
    if( ret < 0 )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] ftruncate error ,the error is %s\n", strerror(errno));
        return -1;
    }
    
    ptr = (char*)mmap( NULL, SHMSIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0 );
    if( ptr == (char*)MAP_FAILED)
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] mmap error, the error is %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/**
@brief 打开共有内存
@return         void
@details
*/

static int shm_Open(void)
{
    int ret = 0;
    int fd;
    fd = shm_open( SHM_NAVI, O_RDWR, 0666 );
    if( fd < 0 )
    {
        CQ_LOG_ERROR( "[Servrce manger VR shm] shm_open error, the error is %s\n", strerror(errno));
        return -1;
    }
    
    ptr = (char*)mmap( NULL, SHMSIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0 );
    if( ptr == (char*)MAP_FAILED)
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] mmap error, the error is %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/**
@brief 删除共有内存
@return         void
@details
*/

static int shm_Delete(void)
{
    int ret;
    
    ret = shm_unlink( SHM_NAVI );
    if( ret < 0 )
    {
        CQ_LOG_ERROR("[Servrce manger VR shm] shm_unlink error, the error is %s\n", strerror(errno));
        return -1;
    }
    return 0;

}

/**
@brief 写共有内存
@return         void
@details
*/

int32_t shm_Write(void * data,int32_t len)
{
    if(data  == NULL)
    {
        return -1;
    }
    //sem_Take();
    memcpy(ptr,data,len);
    //sem_Give();
    return len;
}

/**
@brief 写共有内存,写之前清除原有内容
@return         void
@details
*/

int32_t shm_Clean_Write(void * data,int32_t len)
{
    if(data  == NULL)
    {
        return -1;
    }
    //sem_Take();
    memset(ptr,0,SHMSIZE);
    memcpy(ptr,data,len);
    //sem_Give();
    return len;
}

/**
@brief 读共有内存
@return         void
@details
*/

int32_t shm_Read(void * buff,int32_t len)
{
    /* 打开资源 */
    static int32_t open_Once = 1;
    if(open_Once)
    {
        shm_Open();
        sem_Open();
        open_Once--;
    }
    if(buff  == NULL)
    {
        return -1;
    }
    /* 获取资源锁 */
    //sem_Take();
    memcpy(buff,ptr,len);
    /* 释放资源锁 */
    //sem_Give();
    return len;
}

/**
@brief 初始化共有内存
@return         void
@details
*/

int32_t shm_Server_Init(void)
{
    //sem_Open();
    shm_Init();
}

/**
@brief 清除共有内存使用资源
@return         void
@details
*/

int32_t shm_Server_Cleanup(void)
{
    sem_Delete();
    shm_Delete();
}
#endif