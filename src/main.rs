mod util;
mod analyzer;

use std::process::ExitCode;
use std::fs::File;
use std::io::Read;
use util::helper;

fn main() -> ExitCode {
    // let args: Vec<String> = std::env::args().collect();
    // if args.len() < 3 {
    //     println!("Not enough args.");
    //     return ExitCode::from(1);
    // }
    let mut aff = File::open("tests/2.aff").unwrap();
    let mut content = String::new();
    aff.read_to_string(&mut content).unwrap();
    let mut mixer_data = analyzer::arcaea::analyze(&content);
    helper::mix_hit_sound(&mut mixer_data, 0);
    return ExitCode::from(0);
}
