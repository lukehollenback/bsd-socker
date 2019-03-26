#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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
#include "options.h"
#include "ethernet_frame.h"
#include "logger.h"
#include "limits.h"

static void parseArguments(int argc, char** argv);
static void initializeDevice(int* descriptor, int* buffer_length);
static void sniff(int bpf, int buffer_length);
static void deinitializeDevice(int bpf);
static void signalHandler(int sig_num);

static bool running = true;

int main(int argc, char** argv) {
    int bpf, buffer_length;

    // Make sure that our assumptions about the configuration this program has
    // been compiled and run against ar correct and fatal if not
    verifyConfiguration();

    // Specify a signal handler to catch various signals (like those sent when
    // Ctrl+C is pressed)
    signal(SIGINT, signalHandler); 

    // Set up the logger with some default settings
    setLoggerOptions(LL_TRACE, LO_NOLABEL);
    setLoggerOptions(LL_INFO, LO_NOLABEL);

    // Parse the program arguments and use them to configure any appropriate
    // options
    parseArguments(argc, argv);

    // Initialize a BPF device for the specified interface and run the main
    // program
    initializeDevice(&bpf, &buffer_length);
    sniff(bpf, buffer_length);
    deinitializeDevice(bpf);

    return 0;
}

static void parseArguments(int argc, char** argv) {
    int i;

    // Parse arguments into the options struct
    while ((i = getopt(argc, argv, "ho:i:")) != -1) {
        switch (i) {
            case 'h':
                output(NULL, "USAGE:\tsocker [-h][-o output_file][-i interface_name]\n");
                exit(0);

            case 'o':
                Options_setOutputFile(optarg);
                break;
            
            case 'i':
                Options_setInterfaceName(optarg);
                break;
            
            default:
                fatal("Invalid option specified (-%c).", optopt);
        }
    }

    Options_checkForRequiredOptions();
    Options_logOptions();
}

/**
 * Attempts to grab a descriptor to a valid BPF device from the system and
 * initialize it.
 */
static void initializeDevice(int* descriptor, int* buffer_length) {
    int i, buffer_int, bpf;
    char buffer_char[11] = { 0 };
    struct ifreq bound_if;

    // Attempt to open the next available Berkley Packet Filter device (BPF)
    for (i = 0; i < MAX_BPF_DEVICES; i++) {
        // Generate the path to the next possible BPF
        sprintf(buffer_char, "/dev/bpf%u", i);

        // Attempt to open the next possible BPF; If we don't fail, we have
        //  succeeded in finding an available one
        bpf = open(buffer_char, O_RDWR | O_NONBLOCK);
        
        if (bpf != -1) {
            break;
        } else if (errno == EACCES) {
            fatal("The system is denying permission to its BPF devices. Make sure propper permissions are being used "
                    "(e.g. root).");
        }
    }

    if (bpf == -1) {
        fatal("Failed to open a BPF device after %d tries. The error on the final attempt was \"%s\".", MAX_BPF_DEVICES,
                strerror(errno));
    }
    else {
        info("Opened the BPF device at %s (file descriptor = %d).", buffer_char, bpf);
    }

    // Associate with a particular network interface
    strcpy(bound_if.ifr_name, Options_getInterfaceName());
    if(ioctl(bpf, BIOCSETIF, &bound_if) == -1) {
        fatal("Failed to associate the BPF device with the network interface \"%s\". (%i: %s)",
                Options_getInterfaceName(), errno, strerror(errno));
    }
    else {
        info("Associated the BPF device with the network interface \"%s\".", Options_getInterfaceName());
    }

    // Turn on "immediate" mode
    // NOTE ~> This means that blocking reads will return as soon as new socket
    //  data is available rather than when the read buffer is full or a timeout
    //  occurs.
    buffer_int = 1;
    if (ioctl(bpf, BIOCIMMEDIATE, &buffer_int) == -1) {
        fatal("Failed to turn on the BPF device's \"immediate\" mode. (%i: %s)", errno, strerror(errno));
    }
    else {
        info("Turned on the BPF device's \"immediate\" mode.");
    }

    // Get the buffer length (so that we can traverse multiple entries when
    //  reading from the BPF)
    if (ioctl(bpf, BIOCGBLEN, buffer_length) == -1) {
        fatal("Failed to retrieve the BPF device's buffer length. (%i: %s)", errno, strerror(errno));
    }
    else {
        info("Retrieved the BPF device's buffer length (%i bytes).", *buffer_length);
    }

    // Place the BPF's descriptor into the provided variable reference
    *descriptor = bpf;
}

/**
 * Actually sniffs and logs packets.
 */
static void sniff(int bpf, int buffer_length) {
    EthernetFrame* frame;
    struct bpf_hdr* bpf_buffer = malloc(buffer_length * sizeof(struct bpf_hdr));
    struct bpf_hdr* bpf_packet;
    int read_bytes = 0;

    while (running) {
        memset(bpf_buffer, 0, buffer_length);

        if((read_bytes = read(bpf, bpf_buffer, buffer_length)) > 0) {
            int i = 0;
            char* ptr = (char*) bpf_buffer;

            while (ptr < (((char*) bpf_buffer) + read_bytes)) {
                bpf_packet = (struct bpf_hdr*) ptr;
                frame = (EthernetFrame*)((char*) bpf_packet + bpf_packet->bh_hdrlen);

                EthernetFrame_output(frame);

                ptr += BPF_WORDALIGN(bpf_packet->bh_hdrlen + bpf_packet->bh_caplen);
            }
        }
    }

    // Clean up after ourselves
    // NOTE ~> This should happen automatically, but better be safe than sorry.
    free(bpf_buffer);
}

/**
 * Closes the open BPF device at the provided dscriptor.
 */
static void deinitializeDevice(int bpf) {
    close(bpf);
    info("Closed BPF device with file descriptor %d", bpf);
}

/**
 * Provides overriden signal handling specific to this program's use cases for
 * registered signals.
 */
static void signalHandler(int sig_num) {
    // Reset the signal handler
    // NOTE ~> This is mainly for cases where we might not actually end the
    //  program here so that we are able to catch it next time.
    signal(SIGINT, signalHandler); 

    // Handle possible signals
    switch (sig_num) {
        case SIGINT:
            running = false;
            break;

        default:
            break;
    }
}