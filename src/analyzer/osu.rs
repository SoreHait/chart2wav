use crate::util::types::MixerData;

pub fn analyze(content: &String) -> (i32, Vec<MixerData>) {
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
                _type: 0,
            })
        }
        if line == "[HitObjects]" {
            is_hit_objects = true;
        }
    }
    return (0, mixer_data);
}
