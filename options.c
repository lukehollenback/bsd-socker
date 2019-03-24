#include "common.h"
#include <string.h>

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