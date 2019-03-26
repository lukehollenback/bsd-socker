#include "ethernet_frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "logger.h"

#define DEST_MAC_SIZE                   6
#define SRC_MAC_SIZE                    6
#define VLAN_TAG_SIZE                   4
#define ETHER_TYPE_SIZE                 2
#define PAYLD_MAX_SIZE                  1508
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
    OCTET destination_mac_address[DEST_MAC_SIZE];

    /**
     * The MAC address of the NIC that the frame (supposedly) originated from.
     */
    OCTET source_mac_address[SRC_MAC_SIZE];

    /**
     * The "EtherType" field of the frame.
     * 
     * If the frame is VLAN tagged, this actually represents the TPID indicating
     * so, and the "EtherType" exists in the payload.
     */
    OCTET ethernet_type[ETHER_TYPE_SIZE];

    /**
     * The payload of the frame.
     * 
     * Normally a max of 1500 octets. If the frame is VLAN tagged, then
     * payload[0] and payload [1] represent the "TCI", payload[2] and payload[3]
     * represent the "EtherType", and the actual payload begins at payload[4].
     */
    OCTET payload[PAYLD_MAX_SIZE];
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
UINT EthernetFrame_getVLANTag(EthernetFrame* o) {
    UINT ethernet_type = 0x0000;

    // Retrieve the 2 octet "EtherType" field of the EthernetFrame
    ethernet_type = ((o->ethernet_type[0] << 8) | o->ethernet_type[1]);

    // If the TPID is specified where the "EtherType" field usually exists,
    // return the two octect "TCI" field that follows
    if (ethernet_type == ET_VLANTAGGED) {
        OCTET* tci_pointer = o->payload;
        UINT buff[1] = { 0x0000 };

        octetsToInt(tci_pointer, 2, buff);
    
        return (*buff);
    }

    return -1;
}

/**
 * Figures out what the "EtherType" field of the provided EthernetFrame is and
 * returns it.
 */
EthernetType EthernetFrame_getEthernetType(EthernetFrame* o) {
    OCTET* ethernet_type_pointer;

    // Find our "EtherType" field and get a pointer to it
    if (EthernetFrame_getVLANTag(o) != -1) {
        ethernet_type_pointer = (OCTET*) (o->payload + PAYLD_VLAN_ETHER_TYPE_OFFSET);
    } else {
        ethernet_type_pointer = o->ethernet_type;
    }
    
    // Retrieve the value of the "EtherType" field and return it
    UINT buff[1] = { 0x0000 };

    octetsToInt(ethernet_type_pointer, 2, buff);
    
    return (*buff);
}

/**
 * Calculates and returns the size of the header (a.k.a. the number of bytes
 * before the payload) of the provided Ethernet Frame.
 */
size_t EthernetFrame_getHeaderSize(EthernetFrame* o) {
    size_t size = (DEST_MAC_SIZE + SRC_MAC_SIZE + ETHER_TYPE_SIZE);

    if (EthernetFrame_getVLANTag(o) != -1)
        size += VLAN_TAG_SIZE;
    
    return size;
}

/**
 * Figures out where the "Payload" field of the provided "EthernetFrame" is and
 * returns a pointer to it
 */
OCTET* EthernetFrame_getPayloadPointer(EthernetFrame* o) {
    // Find our payload and get a pointer to it
    if (EthernetFrame_getVLANTag(o) != -1) {
        return (OCTET*) (o->payload + PAYLD_VLAN_PAYLD_OFFSET);
    } else {
        return o->payload;
    }
}

/**
 * Generates a printable string representation of the provided EthernetFrame
 * according to the program's options.
 */
void EthernetFrame_output(EthernetFrame* o, size_t size) {
    UINT tci;
    char buff[18] = { 0 };

    // Grab the necessary peices
    char et[12] = { 0 };
    EthernetType_toString(EthernetFrame_getEthernetType(o), et, 12);

    // Output a readable version of the EthernetFrame
    output(NULL, "[    ]\t");
    output(LC_BLUE, et);

    output(NULL, "\t");
    if ((tci = EthernetFrame_getVLANTag(o)) != -1)
        output(LC_BLUE, "0x%04x", tci);

    octetsToHexString(o->destination_mac_address, 6, buff, '-', 2);
    output(LC_BLUE, "\tDest MAC: %s", buff);

    octetsToHexString(o->source_mac_address, 6, buff, '-', 2);
    output(LC_BLUE, "\tSource MAC: %s", buff);

    output(NULL, "\t");
    
    OCTET* data_ptr = EthernetFrame_getPayloadPointer(o);
    size_t pyld_size = (size - EthernetFrame_getHeaderSize(o));
    size_t data_read = 0;

    while (data_read < pyld_size) {
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