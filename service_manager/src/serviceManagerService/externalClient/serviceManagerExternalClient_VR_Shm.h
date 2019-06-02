#ifdef MAPBAR_TOYOTA_262AVN_LINUX
#ifndef __SHM__
#define __SHM__


int32_t shm_Server_Init(void);
int32_t shm_Server_Cleanup(void);
int32_t shm_Write(void * data,int32_t len);
int32_t shm_Clean_Write(void * data,int32_t len);
int32_t shm_Read(void * buff,int32_t len);


#endif /* end of __SHM__*/
#endif /* end of MAPBAR_TOYOTA_262AVN_LINUX */