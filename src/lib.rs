#[macro_use]
extern crate log;
extern crate env_logger;
extern crate get_if_addrs;
extern crate clap;

use clap::{Arg, App};
use get_if_addrs::IfAddr::V4;

#[derive(Debug)]
pub struct Config {
    pub addr: get_if_addrs::Ifv4Addr,
}

impl Config {
    pub fn new() -> Result<Config, &'static str> {
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
        debug!("Config: using interface: {}", if_name);

        let interfaces = match get_if_addrs::get_if_addrs() {
            Ok(i) => {
                if let Some(x) = i.iter()
                        .filter(|iface| {
                            iface.name == if_name
                            && match iface.addr {
                                V4(_) => true,
                                _ => false
                            }
                        }).next() {

                    match x.addr.clone() {
                        V4(y) => Ok(y),
                        _ => Err("No V4 address found"),
                    }
                } else {
                    Err("Interface not found")
                }
            },
            Err(_) => Err("Error listing interfaces"),
        };
        debug!("Config: interfaces = {:?}", interfaces);

        match interfaces {
            Ok(addr) => Ok(Config { addr }),
            Err(e) => Err(e),
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
