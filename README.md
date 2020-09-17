# BSDSocker

<img alt="Status: Abandoned (Partially Functional)" src="https://img.shields.io/badge/Status-Abandoned (Partially Functional)-red.svg"/>

_**Author's Note (2020.2.9) ↝** I do not plan to pick this project back up again. A
significant amount of its functionality – such as sniffing packets containing
payloads in a variety of formats and determining many details about them – is
complete. Other features, such as injection, are not – but should be easy to
figure out based on the existing code. Hopefully it can serve as a
starting-point or an educational example for others._

A basic command-line packet sniffer and injector for BSD-based systems (e.g.
macOS). The goal of this project is that it:

- Compiles with any C compiler on any BSD-based system
- Works on any BSD-based system that provides the `BPF` device (try `man bpf`)
- Is written and architected in an extremely clean, readable, and thus
  referenceable way

Because BSD-based systems handle raw sockets differently than other Unix-y
systems, packet sniffers and injectors have to be written differently.
Specifically, BSD provides a special system device for interacting with the raw
network data coming over a given network device. This project is an attempt to
make use of said special device.

For more information, check out
[Using FreeBSD's BPF device with C/C++](http://bastian.rieck.ru/howtos/bpf/) by
Bastian Rieck.
