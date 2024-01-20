use crate::util::helper;
use crate::util::types::MixerData;

pub fn analyze(aff_content: &String) -> Vec<MixerData> {
    let mut mixer_data: Vec<MixerData> = vec![];
    let lines = aff_content.split_terminator("\n");
    for line in lines {
        if line.starts_with("arc") {
            if line.find("false") != None && line.find("arctap") == None {
                mixer_data.push(MixerData {
                    timing: helper::get_str_in_between(line, "arc(", ",")
                        .parse()
                        .unwrap(),
                    _type: 1,
                })
            }
            if line.find("arctap") != None {
                let arctap_str = helper::get_str_in_between(line, "[", "]");
                for arctap in arctap_str.split_terminator(",") {
                    mixer_data.push(MixerData {
                        timing: helper::get_str_in_between(arctap, "(", ")")
                            .parse()
                            .unwrap(),
                        _type: 1,
                    })
                }
            }
        } else if line.starts_with("hold") {
            mixer_data.push(MixerData {
                timing: helper::get_str_in_between(line, "hold(", ",")
                    .parse()
                    .unwrap(),
                _type: 0,
            })
        } else if line.starts_with("(") {
            mixer_data.push(MixerData {
                timing: helper::get_str_in_between(line, "(", ",")
                    .parse()
                    .unwrap(),
                _type: 0,
            })
        }
    }
    return mixer_data;
}
