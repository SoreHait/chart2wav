mod util;
mod analyzer;

use std::process::ExitCode;
use std::fs::File;
use std::io::{Read, Write};
use util::helper;
use util::types;

fn main() -> ExitCode {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 3 {
        println!("Not enough args.");
        return ExitCode::from(1);
    }

    let in_name = args.get(1).unwrap();
    let analyze: fn(&String) -> (i32, Vec<types::MixerData>);
    if in_name.ends_with(".aff") {
        analyze = analyzer::arcaea::analyze;
    } else if in_name.ends_with(".osu") {
        analyze = analyzer::osu::analyze;
    } else {
        println!("Unknown file type.");
        return ExitCode::from(1);
    }

    let mut out_name = String::from(args.get(2).unwrap());
    if !out_name.ends_with(".wav") {
        out_name.push_str(".wav");
    }

    let mut file = File::open(in_name).unwrap();
    let mut content = String::new();
    file.read_to_string(&mut content).unwrap();
    let (offset, mut mixer_data) = analyze(&content);
    let mut hit_sound_data = helper::mix_hit_sound(&mut mixer_data, offset);
    let wav_packed_data = helper::pack_wav(&mut hit_sound_data);

    let mut wav_fp = File::create(out_name).unwrap();
    wav_fp.write_all(wav_packed_data.as_slice()).unwrap();
    return ExitCode::from(0);
}
