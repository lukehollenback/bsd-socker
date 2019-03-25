#ifndef _COMMON_H_
#define _COMMON_H_

// NOTE ~> This macro is expected to be defined during the build. If for some
//  reason it is not, it will default to what is specified here.
#ifndef APP_NAME
#define APP_NAME "bsd_socker"
#endif

#define MAX_BPF_DEVICES 99
#define MAX_PATH_LENGTH 256

#define OCTET 8

#endif