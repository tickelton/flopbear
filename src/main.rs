extern crate clap;
#[macro_use]
extern crate log;
extern crate env_logger;

use clap::{Arg, App};

fn main() {
    match env_logger::init() {
        Ok(_) => trace!("env_logger successfully initialized"),
        Err(e) => eprintln!("WARNING: unable to initialize env_logger: {}", e),
    }

    let matches = App::new("flopbear")
                        .version("0.1.0")
                        .author("<tickelton@gmail.com>")
                        .about("A configuration-less DHCP Server")
                        .arg(Arg::with_name("INTERFACE")
                             .help("Network interface to listen on")
                             .required(true)
                             .index(1))
                        .get_matches();
    let iface = matches.value_of("INTERFACE").unwrap();
    debug!("Using interface: {}", iface);
}

