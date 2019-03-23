#ifndef _ETHERNET_FRAME_H_
#define _ETHERNET_FRAME_H_

#include "common.h"
#include <stdbool.h>

/**
 * Represents the basic header of an ethernet frame (not an ethernet packet,
 * thus no "Preamble" or "Delimiter" field). It doesn't inherrintely account for
 * the VLAN-tag that may or may not be present in an ethernet frame, but its
 * supporting functions do.
 */
typedef struct {
    unsigned char destination_mac_address[6];
    unsigned char source_mac_address[6];
    unsigned char ethernet_type[2];
    unsigned char payload[4];
} EthernetFrame;

/**
 * Represents all the possible values of an ethernet frame's "EtherType" field.
 */
typedef enum {
    ET_IPV4         = 0x0800,
    ET_IPV6         = 0x86DD,
    ET_VLANTAGGED   = 0x8100
} EthernetType;

bool EthernetFrame_isVLANTagged(EthernetFrame *o);
EthernetType EthernetFrame_getEthernetType(EthernetFrame *o);
unsigned char * EthernetFrame_getPayloadPointer(EthernetFrame *o);

#endif