# chart2wav
通过谱面文件生成打击音wav，支持用户自行编写分析器以支持更多谱面格式

## TODO
- [x] ~~使用Rust重构~~
- [x] ~~增加自定义分析器支持~~
- [ ] 支持自定义打击音

## 编译
`cargo build --release`

或者看看Releases

## 使用方法
`chart2wav <chart file> <output file>`

`<chart file>`为谱面文件，可使用相对路径

`<output file>`为输出音频文件，可使用相对路径，`.wav`可写可不写

## 编写自定义分析器
在`analyzer`文件夹内编写自定义分析器，需要符合`fn(&String) -> (i32, Vec<MixerData>)`

`MixerData`中`timing`字段为打击音播放时间点，单位为ms；`_type`字段为使用的打击音类型

在`main.rs`中`extension_names`处添加扩展名和对应处理函数，编译
