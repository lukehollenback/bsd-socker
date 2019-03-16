#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/bpf.h>
#include "common.h"
#include "ethernet_frame.h"
#include "limits.h"

int main(int argc, char **argv) {
    int i, bpf, buffer_int;
    char buffer_char[11] = { 0 };
    struct ifreq bound_if;

    // Attempt to open the next available Berkley Packet Filter device (BPF)
    for (i = 0; i < MAX_BPF_DEVICES; i++) {
        // Generate the path to the next possible BPF
        sprintf(buffer_char, "/dev/bpf%i", i);

        // Attempt to open the next possible BPF; If we don't fail, we have
        //  succeeded in finding an available one
        bpf = open(buffer_char, O_RDWR);
        if (bpf != -1)
            break;
    }

    if (i == 100 && bpf == -1) {
        trace("Failed to open a BPF device after 100 tries.\n");
        exit(1);
    }
    else {
        trace("Successfully opened the BPF device at %s.\n", buffer_char);
    }

    // Associate with a particular network interface
    strcpy(bound_if.ifr_name, "en1");
    if(ioctl(bpf, BIOCSETIF, &bound_if) == -1) {
        trace("Failed to associate the BPF device with the network interface \"%s\". (%i: %s)\n", "en1", errno, strerror(errno));
        exit(1);
    }
    else {
        trace("Successfully associated the BPF device with the network interface \"%s\".\n", "en1");
    }

    // Turn on "immediate" mode (this means that blocking reads will return as
    //  soon as new socket data is available rather than when the read buffer is
    //  full or a timeout occurs)
    buffer_int = 1;
    if (ioctl(bpf, BIOCIMMEDIATE, &buffer_int) == -1) {
        trace("Failed to turn on the BPF device's \"immediate\" mode. (%i: %s)\n", errno, strerror(errno));
        exit(1);
    }
    else {
        trace("Successfully turned on the BPF device's \"immediate\" mode.\n");
    }

    // Get the buffer length (so that we can traverse multiple entries when
    //  reading from the BPF)
    if (ioctl(bpf, BIOCGBLEN, &buffer_int) == -1) {
        trace("Failed to retrieve the BPF device's buffer length. (%i: %s)\n", errno, strerror(errno));
        exit(1);
    }
    else {
        trace("Successfully retrieved the BPF device's buffer length (%i bytes).\n", buffer_int);
    }

    // Clean up after ourselves
    close(bpf);

#if 0
    EthernetFrame ef_one = {
        .destination_mac_address = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 },
        .source_mac_address = { 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B },
        .ethernet_type = { 0x08, 0x00 },
        .payload = { 0xaa, 0xbb, 0xcc, 0xdd }
    };

    printf("Is VLAN-tagged = %i\n", EthernetFrame_isVLANTagged(&ef_one));
    printf("Ethernet Type = 0x%04x\n", EthernetFrame_getEthernetType(&ef_one));
    printf("Payload = 0x%08x\n", *(unsigned int *)EthernetFrame_getPayloadPointer(&ef_one));

    printf("\n");

    EthernetFrame ef_two = {
        .destination_mac_address = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 },
        .source_mac_address = { 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B },
        .ethernet_type = { 0x81, 0x00 },
        .payload = { 0xaa, 0xbb, 0xcc, 0xdd }
    };

    printf("Is VLAN-tagged = %i\n", EthernetFrame_isVLANTagged(&ef_two));
    printf("Ethernet Type = 0x%04x\n", EthernetFrame_getEthernetType(&ef_two));
    printf("Payload = 0x%08x\n", *(unsigned int *)EthernetFrame_getPayloadPointer(&ef_two));
#endif

    return 0;
}