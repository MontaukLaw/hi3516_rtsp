#ifndef SYS_H
#define SYS_H
#include <stdint.h>
#include <mpi_sys.h>

#ifdef __cplusplus
extern "C" {
#endif


int HI_MPI_SYS_SetReg(uint32_t u32Addr, uint32_t u32Value);

int HI_MPI_SYS_GetReg(uint32_t u32Addr, uint32_t *pu32Value);

#ifdef __cplusplus
}
#endif


#endif //SYS_H
