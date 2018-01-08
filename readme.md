# BSDSocker
![](https://img.shields.io/badge/project%20status-on%20hold-orange.svg)

A basic command-line packet sniffer and injector for BSD-based systems (e.g. macOS).

Because BSD-based systems handle raw sockets differently than other Unix-y systems, packet sniffers and injectors have to be written differently. Specifically, BSD provides a special system device for interacting with the raw network data coming over a given network device. This project is an attempt to make use of said special device.

For more information, check out [Using FreeBSD's BPF device with C/C++](http://bastian.rieck.ru/howtos/bpf/) by Bastian Rieck.
