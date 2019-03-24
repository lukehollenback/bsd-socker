#ifndef _ETHERNET_FRAME_H_
#define _ETHERNET_FRAME_H_

#include "common.h"
#include <stdbool.h>

typedef struct EthernetFrame EthernetFrame;

/**
 * Represents all the possible values of an ethernet frame's "EtherType" field.
 */
typedef enum EthernetType {
    ET_IPV4         = 0x0800,
    ET_IPV6         = 0x86DD,
    ET_VLANTAGGED   = 0x8100
} EthernetType;

EthernetFrame* EthernetFrame_new();
bool EthernetFrame_isVLANTagged(EthernetFrame* o);
EthernetType EthernetFrame_getEthernetType(EthernetFrame* o);
unsigned char* EthernetFrame_getPayloadPointer(EthernetFrame* o);

#endif