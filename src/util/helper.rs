use crate::util::types;

pub fn get_str_in_between<'a>(text: &'a str, begin_with: &str, end_with: &str) -> &'a str {
    let l_pos = text.find(begin_with).unwrap() + begin_with.len();
    let r_pos = text.find(end_with).unwrap();
    return &text[l_pos..r_pos];
}

fn cap_short(x: i16, y: i16) -> i16 {
    let sum: i32 = x as i32 + y as i32;
    return if sum > i16::MAX as i32 {
        i16::MAX
    } else if sum < i16::MIN as i32 {
        i16::MIN
    } else {
        sum as i16
    };
}

pub fn mix_hit_sound(mixer_data: &mut Vec<types::MixerData>, hitsound: &Vec<Vec<i16>>) -> Vec<i16> {
    let mut sample_count: usize = 0;
    if !mixer_data.is_empty() {
        mixer_data.sort_unstable_by_key(|a| a.timing);
        sample_count = (mixer_data.last().unwrap().timing as f64 * 44.1).ceil() as usize
            + hitsound[mixer_data.last().unwrap().hs_type as usize].len();
    }

    let audio_time = sample_count / 44100;
    println!("Audio Time: {audio_time} sec.");
    println!("Sample Count: {sample_count}.");

    let mut mix_buffer: Vec<i16> = vec![0; sample_count];
    for data in mixer_data {
        let mut mix_sample_pos = (data.timing as f64 * 44.1) as usize;
        let sound: &Vec<i16> = &hitsound[data.hs_type as usize];
        for i in 0..sound.len() {
            if mix_sample_pos >= sample_count {
                break;
            }
            mix_buffer[mix_sample_pos] = cap_short(mix_buffer[mix_sample_pos], sound[i]);
            mix_sample_pos += 1;
        }
    }
    return mix_buffer;
}
