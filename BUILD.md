# submodule

```
git clone https://github.com/ithewei/hplayer.git
cd hplayer
git submodule update --init
```

# Dependents
- msvc2015: <http://download.microsoft.com/download/B/4/8/B4870509-05CB-447C-878F-2F80E4CB464C/vs2015.com_chs.iso>
- Qt: <http://download.qt.io/>
- FFmpeg: <http://ffmpeg.org/>
- SDL2: <https://www.libsdl.org/>

# CLI

- qmake
- nmake / mingw32-make

# IDE

## Qt Creator

- hplayer.pro

## Qt VS Tools

- hplayer.sln

# note

为了控制repo大小，以及专注功能而不是环境，只提供了msvc2015_x64的lib和dll，其它构建环境需要的依赖请自行获取，
必要时需要修改.pro文件（qmake工程文件）
