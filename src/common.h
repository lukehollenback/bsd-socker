#ifndef _COMMON_H_
#define _COMMON_H_

#include <stddef.h>

// NOTE ~> This macro is expected to be defined during the build. If for some
//  reason it is not, it will default to what is specified here.
#ifndef APP_NAME
#define APP_NAME "bsd_socker"
#endif

#define MAX_BPF_DEVICES 99
#define MAX_PATH_LENGTH 256

typedef unsigned char OCTET;
typedef unsigned int UINT;
typedef unsigned long ULONG;

void verifyConfiguration();
void octetsToInt(OCTET* octets, size_t num_octets, UINT* buff);
void octetsToCharString(OCTET* octets, size_t num_octets, char* buff);
void octetsToHexString(OCTET* octets, size_t num_octets, char* buff, char sep, size_t sep_interval);

#endif