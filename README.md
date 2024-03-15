# chart2wav
通过谱面文件生成打击音wav，支持自定义打击音和用户自定义分析器

## 编译
`cargo build --release`

或者看看Releases

## 使用方法
`chart2wav chart output hitsound...`

`chart`为谱面文件

`output`为输出音频文件，`.wav`可省略

`hitsound...`为使用的打击音，需要满足对应分析器的数量需求，多个路径之间用空格分隔

例：`chart2wav chart.aff output hs1.wav hs2.wav`

## 编写自定义分析器
在`analyzer`文件夹内编写自定义分析器，需要实现`util/traits.rs`中的`trait Analyzer`

其中`get_hitsound_count`应返回该分析器需要的打击音数量，`print_help_msg`中应输出必要的帮助信息，`analyze`为分析函数

`MixerData`中`timing`字段为打击音播放时间点，单位为ms；`hs_type`字段为使用的打击音类型，从0开始逐个对应输入中的打击音列表

在`main.rs`中`extension_names`处添加扩展名和对应分析器，编译
