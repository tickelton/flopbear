extern crate clap;
#[macro_use]
extern crate log;
extern crate env_logger;
extern crate get_if_addrs;

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
    let if_name = matches.value_of("INTERFACE").unwrap();
    debug!("Using interface: {}", if_name);

	let interfaces = match get_if_addrs::get_if_addrs() {
		Ok(i) => i,
		Err(e) => { eprintln!("Error: {}", e); std::vec::Vec::new() } ,
	};
	println!("interfaces = {:?}", interfaces);

}

