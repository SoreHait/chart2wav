use crate::util::types;

fn validate_short(x: i16, y: i16) -> i16 {
    let sum: i32 = x as i32 + y as i32;
    return if sum > 32767 {
        32767
    } else if sum < -32768 {
        -32768
    } else {
        sum as i16
    };
}

fn make_word(low: u8, high: u8) -> i16 { return ((high as i16) << 8) + low as i16; }

fn bit_to_time(bit_count: usize) -> f32 { return (bit_count as f32) / 88200.0; }

pub fn get_str_in_between<'a>(text: &'a str, begin_with: &str, end_with: &str) -> &'a str {
    let l_pos = text.find(begin_with).unwrap() + begin_with.len();
    let r_pos = text.find(end_with).unwrap();
    return &text[l_pos..r_pos];
}

pub fn mix_hit_sound(mixer_data: &mut Vec<types::MixerData>, offset: i32, hitsound: &Vec<Vec<u8>>) -> Vec<u8> {
    let mix_time_len: f32;
    if mixer_data.len() == 0 {
        mix_time_len = 0.0;
    } else {
        mixer_data.sort_unstable_by_key(|a| a.timing);
        for i in 0..mixer_data.len() {
            mixer_data[i].timing += offset;
        }
        mix_time_len = (mixer_data[mixer_data.len() - 1].timing as f32) / 1000.0
            + bit_to_time(hitsound[mixer_data[mixer_data.len() - 1].hs_type as usize].len())
    }
    println!("Audio Time: {mix_time_len} sec.");

    let sample_count = mix_time_len as usize * 44100;
    println!("Sample Count: {sample_count}.");

    let mut mix_buffer: Vec<u8> = vec![0; sample_count * 2];
    for data in mixer_data {
        let mut mix_sample_pos = (data.timing as f32 * 44.1) as usize;
        let sound = &hitsound[data.hs_type as usize];
        for i in (0..sound.len()).step_by(2) {
            mix_sample_pos += 1;
            if mix_sample_pos >= sample_count {
                break;
            }
            let tmp = validate_short(
                make_word(mix_buffer[mix_sample_pos * 2], mix_buffer[mix_sample_pos * 2 + 1]),
                make_word(sound[i], sound[i + 1]),
            );
            mix_buffer[mix_sample_pos * 2] = tmp as u8;
            mix_buffer[mix_sample_pos * 2 + 1] = (tmp >> 8) as u8;
        }
    }
    return mix_buffer;
}
