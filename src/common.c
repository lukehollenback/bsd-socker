#include "common.h"
#include <string.h>
#include <limits.h>
#include "logger.h"

void verifyConfiguration() {
    // Make sure that a char is 8 bits
    if (CHAR_BIT != 8) {
        fatal("In this configuration, chars are %d bits. This program requires that chars are 8 bits.", CHAR_BIT);
    }
}

/**
 * Re-packs the provided octets into the provided integer buffer. The maximum
 * number of octets that this function can re-pack is equal to the number of
 * octets of a regular integer on the architecture being compiled for.
 */
void octetsToInt(OCTET* octets, size_t num_octets, UINT* buff) {
    
    // Make sure that the number of octets provided will fit into the buffer
    if (num_octets > sizeof(*buff)) {
        fatal("Integers are too small (%d octets) on this architecture to hold the re-packed octets.", sizeof(*buff));
    }

    // Clean the buffer
    memset(buff, 0x00, sizeof(*buff));

    // Pack the octets into the int buffer
    int processed_octets = 0;

    while (processed_octets < num_octets) {
        int shift = ((num_octets - 1 - processed_octets) * 8);

        (*buff) = ((*buff) | (octets[processed_octets] << shift));

        processed_octets++;
    }
}

/**
 * Packs the character representation of each of the provided octets into the
 * provided buffer. It is expected that the buffer is big enough to hold the
 * number of octets provided plus one for a null terminator.
 */
void octetsToString(OCTET* octets, size_t num_octets, char* buff) {
    // TODO: Implement this function.
}