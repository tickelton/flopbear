extern crate flopbear;

#[macro_use]
extern crate log;
extern crate env_logger;

use std::process;
use flopbear::Config;

fn main() {
    match env_logger::init() {
        Ok(_) => trace!("env_logger successfully initialized"),
        Err(e) => eprintln!("WARNING: unable to initialize env_logger: {}", e),
    };

	let config = Config::new().unwrap_or_else(|err| {
        error!("{}", err);
        process::exit(1);
    });

    println!("config={:?}", config);
}

