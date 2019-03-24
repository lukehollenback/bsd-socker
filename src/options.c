#include "common.h"
#include <string.h>
#include "logger.h"

typedef struct {
    char output_file[MAX_PATH_LENGTH];
    char interface_name[MAX_PATH_LENGTH];
} Options;

static Options o = {
    .output_file = { 0 },
    .interface_name = { 0 }
};

void Options_setOutputFile(char* file) {
    strncpy(o.output_file, file, MAX_PATH_LENGTH);
}

char* Options_getOutputFile() {
    return o.output_file;
}

void Options_setInterfaceName(char* name) {
    strncpy(o.interface_name, name, MAX_PATH_LENGTH);
}

char* Options_getInterfaceName() {
    return o.interface_name;
}

/**
 * Verifies that required options are specified, otherwise fatals the program.
 */
void Options_checkForRequiredOptions() {
    if (!*o.interface_name) {
        fatal("A network interface name must be specified.");
    }
}

/**
 * Outputs options (only required & specified) to the log.
 */
void Options_logOptions() {
    info("Interface set to %s.", Options_getInterfaceName());
    if (*o.output_file) {
        info("Output file set to %s.", Options_getOutputFile());
    }
}