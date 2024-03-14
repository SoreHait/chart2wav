use crate::util::traits;
use crate::util::types::MixerData;

pub struct Osu;

impl traits::Analyzer for Osu {
    fn get_hitsound_count(&self) -> i32 { 1 }
    fn print_help_msg(&self) { println!("No hitsound specifications.") }
    fn analyze(&self, content: &String) -> (i32, Vec<MixerData>) {
        let mut mixer_data: Vec<MixerData> = vec![];
        let mut is_hit_objects: bool = false;
        for line in content.lines() {
            if line.len() == 0 {
                continue;
            }
            if is_hit_objects {
                if line.starts_with("[") {
                    is_hit_objects = false;
                    continue;
                }
                mixer_data.push(MixerData {
                    timing: line.splitn(4, ",").collect::<Vec<_>>()[2].parse().unwrap(),
                    hs_type: 0,
                })
            }
            if line == "[HitObjects]" {
                is_hit_objects = true;
            }
        }
        return (0, mixer_data);
    }
}
