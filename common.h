#ifndef _COMMON_H_
#define _COMMON_H_

/**
 * CONSTANTS
 */

#define APP_NAME "bsdsocker"

#define MAX_BPF_DEVICES 50

/**
 * MACROS
 */

// Boolean Macros (because they makes things much more readable)
#define BOOL unsigned int
#define TRUE 1
#define FALSE 0

// Function Macros
#define trace printf(APP_NAME ": "); printf

#endif