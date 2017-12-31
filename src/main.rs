extern crate clap;

use clap::{Arg, App};

fn main() {
    let matches = App::new("flopbear")
                        .version("0.1.0")
                        .author("<tickelton@gmail.com>")
                        .about("A configuration-less DHCP Server")
                        .arg(Arg::with_name("INTERFACE")
                             .help("Network interface to listen on")
                             .required(true)
                             .index(1))
                        .get_matches();

    println!("Using interface: {}", matches.value_of("INTERFACE").unwrap());

}

