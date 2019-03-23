#include "ethernet_frame.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * Determines whether or not the provided EthernetFrame has been VLAN-tagged
 * according to IEEE 802.1Q standards.
 */
bool EthernetFrame_isVLANTagged(EthernetFrame *o) {
    unsigned int ethernet_type = 0x0000;

    // Retrieve the "EtherType" field of the EthernetFrame
    ethernet_type = ((o->ethernet_type[0] << 8) | o->ethernet_type[1]);

    // Check to see if the field is equal to the magic number that indicates
    //  that the frame has a VLAN-tag
    if (ethernet_type == ET_VLANTAGGED)
        return true;

    return false;
}

/**
 * Figures out what the "EtherType" field of the provided EthernetFrame is and
 * returns it.
 */
EthernetType EthernetFrame_getEthernetType(EthernetFrame *o) {
    int offset = 12; // NOTE: 12 is the default offset of the "EtherType" field of an EthernetFrame, but only when it isn't VLAN-tagged.
    unsigned char *ethernet_type_pointer;

    // Figure out where our starting position is
    if (EthernetFrame_isVLANTagged(o))
        offset = 16; // NOTE: The VLAN-tag is 4 bytes long.
    
    // Retrieve the value of the "EtherType" field and return it
    ethernet_type_pointer = (unsigned char *)((unsigned char *)o + offset);
    return ((ethernet_type_pointer[0] << 8) | ethernet_type_pointer[1]);
}

/**
 * Figures out where the "Payload" field of the provided "EthernetFrame" is and
 * returns a pointer to it
 */
unsigned char * EthernetFrame_getPayloadPointer(EthernetFrame *o) {
    int offset = 14; // NOTE: 14 is the default offset of the "Payload" field of an Ethernet Frame, but only when it isn't VLAN tagged.

    // Figure out where our starting position is
    if (EthernetFrame_isVLANTagged(o))
        offset = 18; // NOTE: The VLAN-tag is 4 bytes long.
    
    // Retrieve a pointer to the "Payload" field and return it
    return (unsigned char *)((unsigned char *)o + offset);
}