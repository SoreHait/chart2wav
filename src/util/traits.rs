use crate::util::types::MixerData;

pub trait Analyzer {
    fn get_hitsound_count(&self) -> i32;
    fn print_help_msg(&self);
    fn analyze(&self, _: &String) -> Vec<MixerData>;
}
