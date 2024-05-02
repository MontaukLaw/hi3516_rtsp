#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int strtime_to_inttime(const char* strtime, int *hour, int *minute);
int get_current_time(char * psDateTime);
int set_current_time(const char *psDateTime);
int strcpy_s2(char* strDestination, int numberOfElements, const char* strSource);
const char* get_app_version();
const char* get_sys_version();
#ifdef __cplusplus
}
#endif


#endif /*UTILS_H*/
