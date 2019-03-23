#ifndef _COMMON_H_
#define _COMMON_H_

/**
 * Various macros.
 */
#define MAX_BPF_DEVICES 99
#define MAX_PATH_LENGTH 256

/**
 * Represents the options that are driving the program. These should come either
 * from default values or from program arguments.
 */
typedef struct {
    char output_file[MAX_PATH_LENGTH];
    char interface_name[MAX_PATH_LENGTH];
} Options;

#endif