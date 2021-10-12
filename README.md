# aff2wav in C
通过谱面文件aff/osu生成打击音wav

## 使用方法
### **本体**
`aff2wav <map file> <output file>`

`<map file>`为谱面文件，可使用相对路径

`<output file>`为输出音频文件，可使用相对路径，不用写`.wav`程序会自己补

（如果输了`.wav`会导致输出文件变成`.wav.wav`）

### **mixer**
`python mixer.py <music file> <keysound file> <output file> [offset]`

`<music file>`为音乐文件，可相对路径

`<keysound file>`为`aff2wav`生成的keysound音频文件，可相对路径

`<output file>`为音频叠加后输出音频的路径，可相对路径
**注意此处不能忽略扩展名**

`[offset]`为可选参数，单位为ms，若不指定则默认为0

**需要将ffmpeg三件套放在同一目录内才能正常工作，只能使用ffmpeg支持的音频格式**

## 如何编译
cmake项目爱咋来就咋来
