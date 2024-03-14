mod util;
mod analyzer;

use std::collections::HashMap;
use std::process::ExitCode;
use std::fs::File;
use std::io::{Read, Write};
use util::helper;
use util::types::Analyzer;
use crate::analyzer::{arcaea, osu};

fn main() -> ExitCode {
    let extension_names: HashMap<&str, Analyzer> = HashMap::from([
        ("aff", arcaea::analyze as Analyzer),
        ("osu", osu::analyze as Analyzer),
    ]);

    let args: Vec<String> = std::env::args().collect();
    if args.len() < 3 {
        println!("Not enough args.");
        return ExitCode::from(1);
    }

    let in_name = args.get(1).unwrap();
    let in_name_ext: &str;
    if let Some(ext) = in_name.rsplit_once('.') {
        in_name_ext = ext.1;
    } else {
        println!("Could not determine extension name.");
        return ExitCode::from(1);
    }

    let analyze: &Analyzer;
    if let Some(analyzer) = extension_names.get(in_name_ext) {
        analyze = analyzer;
        println!("Matched analyzer for ext name: {in_name_ext}.");
    } else {
        println!("Could not match analyzer for ext name: {in_name_ext}.");
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

    let mut wav_fp = File::create(out_name.clone()).unwrap();
    wav_fp.write_all(wav_packed_data.as_slice()).unwrap();
    println!("Audio file output to {out_name}.");
    return ExitCode::from(0);
}
