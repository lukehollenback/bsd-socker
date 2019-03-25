#include "ethernet_frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "logger.h"

#define DEST_MAC_SIZE                   6
#define SRC_MAC_SIZE                    6
#define ETHER_TYPE_SIZE                 2
#define PAYLD_SIZE                      1508
#define PAYLD_CRC_OFFSET                1500
#define PAYLD_VLAN_CRC_OFFSET           1504
#define PAYLD_VLAN_ETHER_TYPE_OFFSET    2
#define PAYLD_VLAN_PAYLD_OFFSET         4

/**
 * Represents the basic header of an ethernet frame (not an ethernet packet,
 * thus no "Preamble" or "Delimiter" field). It doesn't inherrintely account for
 * the VLAN-tag that may or may not be present in an ethernet frame, but its
 * supporting functions do.
 */
struct EthernetFrame {
    /**
     * The MAC address of the NIC that the frame is destined for.
     */
    unsigned char destination_mac_address[DEST_MAC_SIZE];

    /**
     * The MAC address of the NIC that the frame (supposedly) originated from.
     */
    unsigned char source_mac_address[SRC_MAC_SIZE];

    /**
     * The "EtherType" field of the frame.
     * 
     * If the frame is VLAN tagged, this actually represents the TPID indicating
     * so, and the "EtherType" exists in the payload.
     */
    unsigned char ethernet_type[ETHER_TYPE_SIZE];

    /**
     * The payload of the frame.
     * 
     * Normally a max of 1500 octets. If the frame is VLAN tagged, then
     * payload[0] and payload [1] represent the "TCI", payload[2] and payload[3]
     * represent the "EtherType", and the actual payload begins at payload[4].
     */
    unsigned char payload[PAYLD_SIZE];
};

/**
 * Allocates and initializes a new EthernetFrame prior to returning a pointer to
 * it.
 */
EthernetFrame* EthernetFrame_new() {
    EthernetFrame* ethernetFrame = (EthernetFrame*) malloc(sizeof(EthernetFrame));

    return ethernetFrame;
}

/**
 * Determines whether or not the provided EthernetFrame has been VLAN-tagged
 * according to IEEE 802.1Q standards (meaning that the retular "EtherType"
 * field actually contains the TPID value) and returns the two octet "TCI" field
 * that follows if so or -1 if not.
 * 
 * NOTE ~> We do not currently support 801.2ad standard extensions to this
 *  section of the frame.
 */
unsigned int EthernetFrame_getVLANTag(EthernetFrame* o) {
    unsigned int ethernet_type = 0x0000;

    // Retrieve the 2 octet "EtherType" field of the EthernetFrame
    ethernet_type = ((o->ethernet_type[0] << 8) | o->ethernet_type[1]);

    // If the TPID is specified where the "EtherType" field usually exists,
    // return the two octect "TCI" field that follows
    // NOTE ~> The easiest way to get the address of the "TCI" field is to use
    //  the knon address of what would be the first octed of the frame's actual
    //  payload if it was NOT VLAN tagged.
    if (ethernet_type == ET_VLANTAGGED) {
        unsigned char* tci_pointer = &o->payload[0];

        return ((tci_pointer[0] << OCTET) | tci_pointer[1]);
    }

    return -1;
}

/**
 * Figures out what the "EtherType" field of the provided EthernetFrame is and
 * returns it.
 */
EthernetType EthernetFrame_getEthernetType(EthernetFrame* o) {
    int offset = 12; // NOTE ~> 12 is the default offset of the "EtherType" field of an EthernetFrame, but only when it isn't VLAN-tagged.
    unsigned char* ethernet_type_pointer;

    // Figure out where our starting position is
    if (EthernetFrame_getVLANTag(o) != -1)
        offset = 16; // NOTE ~> The VLAN-tag is 4 bytes long.
    
    // Retrieve the value of the "EtherType" field and return it
    ethernet_type_pointer = (unsigned char *)((unsigned char *)o + offset);
    return ((ethernet_type_pointer[0] << OCTET) | ethernet_type_pointer[1]);
}

/**
 * Figures out where the "Payload" field of the provided "EthernetFrame" is and
 * returns a pointer to it
 */
unsigned char* EthernetFrame_getPayloadPointer(EthernetFrame* o) {
    int offset = 14; // NOTE ~> 14 is the default offset of the "Payload" field of an Ethernet Frame, but only when it isn't VLAN tagged.

    // Figure out where our starting position is
    if (EthernetFrame_getVLANTag(o) != -1)
        offset = 18; // NOTE ~> The VLAN-tag is 4 bytes long.
    
    // Retrieve a pointer to the "Payload" field and return it
    return (unsigned char *)((unsigned char *)o + offset);
}

/**
 * Generates a printable string representation of the provided EthernetFrame
 * according to the program's options.
 */
void EthernetFrame_output(EthernetFrame* o) {
    unsigned int tci;

    // Grab the necessary peices
    char et[12] = { 0 };
    EthernetType_toString(EthernetFrame_getEthernetType(o), et, 12);

    // Output a readable version of the EthernetFrame in the following format:
    // [flags] type tag payload
    output(NULL, "[    ]\t");
    output(LC_BLUE, et);
    output(NULL, "\t");
    if ((tci = EthernetFrame_getVLANTag(o)) != -1)
        output(LC_YELLOW, "0x%04x", tci);
    output(NULL, "\t");
    

    unsigned char* data_ptr = EthernetFrame_getPayloadPointer(o);
    int data_read = 0;
    while (true) {
        if (data_read >= 1500)
            break;

        if ((data_read % 48) == 0)
            output(NULL, "\n\t");
        
        output(NULL, "%02x ", *data_ptr);

        data_ptr++;
        data_read++;
    }

    output(NULL, "\n");
}

/**
 * Converts an EtherType enum value into a string.
 */
void EthernetType_toString(EthernetType et, char* buff, int buff_size) {
    switch (et) {
        case ET_IPV4:
            strncpy(buff, "IPv4", buff_size);
            break;

        case ET_IPV6:
            strncpy(buff, "IPv6", buff_size);
            break;
        
        case ET_ARP:
            strncpy(buff, "ARP", buff_size);
            break;

        case ET_VLANTAGGED:
            strncpy(buff, "VLAN Tagged", buff_size);
            break;

        default:
            sprintf(buff, "0x%04x", et);
            break;
    }
}