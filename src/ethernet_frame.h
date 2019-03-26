#ifndef _ETHERNET_FRAME_H_
#define _ETHERNET_FRAME_H_

#include "common.h"
#include <stdbool.h>

typedef struct EthernetFrame EthernetFrame;

/**
 * Represents possible values of an ethernet frame's "EtherType" field. Not all
 * values are noted here as most are simply not common nowadays. The EtherType
 * Wikipedia page (https://en.wikipedia.org/wiki/EtherType) has a good list if
 * more need to be added in the future.
 */
typedef enum EthernetType {
    ET_IPV4         = 0x0800,
    ET_IPV6         = 0x86DD,
    ET_ARP          = 0x0806,
    ET_VLANTAGGED   = 0x8100
} EthernetType;

void EthernetType_toString(EthernetType et, char* buff, int buff_size);

EthernetFrame* EthernetFrame_new();
UINT EthernetFrame_getVLANTag(EthernetFrame* o);
EthernetType EthernetFrame_getEthernetType(EthernetFrame* o);
OCTET* EthernetFrame_getPayloadPointer(EthernetFrame* o);
void EthernetFrame_output(EthernetFrame* o, size_t size);

#endif