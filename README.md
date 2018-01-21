# flopbear

**NOTE: This README does not describe the current state of the project but the desired state for a first release. If you are reading this, flopbear is probably not functional, yet.**

flopbear is (going to be) a configuration-less, single binary, non RFC compliant (for now) DHCP server for use in development and testing.

It's main use cases are situations where a temporary DHCP server is needed quickly. Ease of use is the primary objective, so that no time has to be spent installing and configuring software.

An example scenario is embedded development for an evaluation board that can only be accessed via Network, is configured to use DHCP by default and can not be connected to an existing network but has to be connected to a development machine directly (e.g. for security reasons).

For now flopbear is targeting only Linux and OpenBSD. Patches to support other platforms are very welcome.

## Getting started

flopbear can simply be started from the command line. The only required argument is the name of a configured network interface to listen on. Addresses in the range configured on the given interface will automatically be assigned to all DHCP clients. flopbear will assume that all addresses higher than that of the listening interface are free to be used and will assign them in ascending order.

```shell
$ ifconfig eth0 192.168.5.1 netmask 255.255.255.0 up
$ ./flopbear eth0
```

## Building

### Dependencies

**TODO: fill in as they emerge.**
* GNU Make

### Compiling

flopbear is written in C and compiles to a single, statically linked binary. It can simply be build by calling GNU make:

Linux:
```shell
make
```

OpenBSD:
```shell
gmake
```

## License

flopbear is distributed under the terms of the ISC license.

See LICENSE for details.

