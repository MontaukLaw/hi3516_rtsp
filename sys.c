#include "sys.h"

int HI_MPI_SYS_SetReg(uint32_t u32Addr, uint32_t u32Value)
{
    HI_U32 *pu32Addr = NULL;
    HI_U32 u32MapLen = sizeof(u32Value);

    pu32Addr = (HI_U32 *)HI_MPI_SYS_Mmap(u32Addr, u32MapLen);
    if(NULL == pu32Addr)
    {
        return -1;
    }
    *pu32Addr = u32Value;
    
    return (HI_MPI_SYS_Munmap(pu32Addr, u32MapLen)== HI_SUCCESS) ? 0 : -1;
}

int HI_MPI_SYS_GetReg(uint32_t u32Addr, uint32_t *pu32Value)
{
    HI_U32 *pu32Addr = NULL;
    HI_U32 u32MapLen;

    if (NULL == pu32Value)
    {
        return -1;
    }

    u32MapLen = sizeof(*pu32Value);
    pu32Addr = (HI_U32 *)HI_MPI_SYS_Mmap(u32Addr, u32MapLen);
    if(NULL == pu32Addr)
    {
        return -1;
    }

    *pu32Value = *pu32Addr;

    return (HI_MPI_SYS_Munmap(pu32Addr, u32MapLen) == HI_SUCCESS) ? 0 : -1;
}
