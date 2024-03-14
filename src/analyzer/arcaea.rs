use crate::util::helper;
use crate::util::traits;
use crate::util::types::MixerData;

pub struct Arcaea;

impl traits::Analyzer for Arcaea {
    fn get_hitsound_count(&self) -> i32 { 2 }
    fn print_help_msg(&self) { println!("Hitsound 0: TAP; Hitsound 1: ARC.") }
    fn analyze(&self, content: &String) -> (i32, Vec<MixerData>) {
        let mut mixer_data: Vec<MixerData> = vec![];
        let mut offset: i32 = 0;
        for line in content.lines() {
            if line.starts_with("arc") {
                if line.find("false") != None && line.find("arctap") == None {
                    mixer_data.push(MixerData {
                        timing: helper::get_str_in_between(line, "arc(", ",")
                            .parse()
                            .unwrap(),
                        hs_type: 1,
                    })
                }
                if line.find("arctap") != None {
                    let arctap_str = helper::get_str_in_between(line, "[", "]");
                    for arctap in arctap_str.split_terminator(",") {
                        mixer_data.push(MixerData {
                            timing: helper::get_str_in_between(arctap, "(", ")")
                                .parse()
                                .unwrap(),
                            hs_type: 1,
                        })
                    }
                }
            } else if line.starts_with("hold") {
                mixer_data.push(MixerData {
                    timing: helper::get_str_in_between(line, "hold(", ",")
                        .parse()
                        .unwrap(),
                    hs_type: 0,
                })
            } else if line.starts_with("(") {
                mixer_data.push(MixerData {
                    timing: helper::get_str_in_between(line, "(", ",").parse().unwrap(),
                    hs_type: 0,
                })
            } else if line.starts_with("AudioOffset:") {
                offset = line.strip_prefix("AudioOffset:").unwrap().parse().unwrap();
            }
        }
        return (offset, mixer_data);
    }
}
