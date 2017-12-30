# flopbear

** NOTE: This README does not describe the current state of the project but the desired state for a first release. If you are reading this, flopbear is probably not functional, yet. **

flopbear is (going to be) a configuration-less, single binary, fully [RFC 2123](https://tools.ietf.org/html/rfc2131) compliant DHCP server for use in development, testing and embedded systems.

It's main use cases are situations where a temporary DHCP server is needed quickly. Ease of use is the primary objective, so that no time has to be spent installing and configuring software.

An example scenario is embedded development for an evaluation board that can only be accessed via Network, is configured to use DHCP by default and can not be connected to an existing network but has to be connected to a development machine directly (e.g. for security reasons).

## Getting started

flopbear can simply be started from the command line. The only required argument is the name of the interface to listen on. A suitable address range is determined automatically.

```shell
$ ./flopbear eth0
```

## Building

flopbear is written in [Rust](https://github.com/rust-lang/rust) and uses the [Cargo](https://github.com/rust-lang/cargo) package manager. Therefore it can be built using the standard cargo build process.

```shell
git clone https://github.com/tickelton/flopbear
cd flopbear
cargo build --release
./target/release/flopbear eth0
```

## License

flopbear is primarily distributed under the terms of both the MIT license and the Apache License (Version 2.0).

See LICENSE-APACHE and LICENSE-MIT for details.

