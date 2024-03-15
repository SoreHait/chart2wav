mod util;
mod analyzer;

use std::collections::{HashMap, VecDeque};
use std::process::ExitCode;
use std::fs::File;
use std::io::Read;
use util::helper;
use analyzer::{arcaea, osu};
use util::traits::Analyzer;
use wav;

fn main() -> ExitCode {
    let extension_names: HashMap<&str, Box<dyn Analyzer>> = HashMap::from([
        ("aff", Box::new(arcaea::Arcaea) as Box<dyn Analyzer>),
        ("osu", Box::new(osu::Osu) as Box<dyn Analyzer>),
    ]);

    let mut args: VecDeque<String> = std::env::args().collect();
    args.pop_front(); // Deposit command itself
    let in_name: String;
    if let Some(i) = args.pop_front() {
        in_name = i;
    } else {
        println!("Could not find chart file name.");
        return ExitCode::from(1);
    }
    let in_name_ext: &str;
    if let Some(ext) = in_name.rsplit_once('.') {
        in_name_ext = ext.1;
    } else {
        println!("Could not determine chart extension name.");
        return ExitCode::from(1);
    }

    let analyzer: &Box<dyn Analyzer>;
    if let Some(i) = extension_names.get(in_name_ext) {
        analyzer = i;
        println!("Matched analyzer for ext name: {in_name_ext}.");
    } else {
        println!("Could not match analyzer for ext name: {in_name_ext}.");
        return ExitCode::from(1);
    }

    analyzer.print_help_msg();

    let mut out_name: String;
    if let Some(i) = args.pop_front() {
        out_name = i;
    } else {
        println!("Could not find output path.");
        return ExitCode::from(1);
    }
    if !out_name.ends_with(".wav") {
        out_name.push_str(".wav");
    }

    let hitsound_count = analyzer.get_hitsound_count();
    let hs_len = args.len();
    if hs_len < hitsound_count as usize {
        println!("Hitsound count ({hs_len}) not enough for this type of chart ({hitsound_count}).");
        return ExitCode::from(1);
    }

    let mut hitsound: Vec<Vec<i16>> = vec![];
    for i in 0..hitsound_count {
        let hs_name = args[i as usize].clone();
        let mut wav_file;
        if let Ok(i) = File::open(hs_name.clone()) {
            wav_file = i;
        } else {
            println!("Wav open failed ({hs_name}).");
            return ExitCode::from(1);
        }
        if let Ok((h, d)) = wav::read(&mut wav_file) {
            if h.audio_format != wav::WAV_FORMAT_PCM ||
                h.channel_count != 1 ||
                h.sampling_rate != 44100 ||
                h.bits_per_sample != 16 {
                println!("Wav format needs to be PCM, mono, 44.1kHz, 16bit ({hs_name}).");
                return ExitCode::from(1);
            }
            hitsound.push(d.as_sixteen().unwrap().to_vec());
        } else {
            println!("Wav read failed ({hs_name}), check https://docs.rs/wav/latest/wav/fn.read.html");
            return ExitCode::from(1);
        }
    }

    let mut file = File::open(in_name).unwrap();
    let mut content = String::new();
    file.read_to_string(&mut content).unwrap();
    let mut mixer_data = analyzer.analyze(&content);
    let hit_sound_data = helper::mix_hit_sound(&mut mixer_data, &hitsound);

    let mut wav_fp = File::create(out_name.clone()).unwrap();
    let header = wav::Header::new(
        wav::WAV_FORMAT_PCM, 1, 44100, 16
    );
    wav::write(header, &wav::BitDepth::from(hit_sound_data), &mut wav_fp).expect("");
    println!("Audio file output to {out_name}.");
    return ExitCode::from(0);
}
