pub type Analyzer = fn(&String) -> (i32, Vec<MixerData>);

pub struct MixerData {
    pub timing: i32,
    pub _type: i8
}
