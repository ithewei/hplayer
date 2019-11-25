# submodule

```
git clone https://github.com/ithewei/hplayer.git
cd hplayer
git submodule update --init
```

# CLI

- qmake
- nmake / mingw32-make

# IDE

## Qt Creator

- hplayer.pro

## Qt VS Tools

- hplayer.sln

# note

为了控制repo大小，以及专注功能而不是环境，只提供了msvc14_x86的lib和dll，其它构建环境需要的依赖请自行获取，
必要时需要修改.pro文件（qmake工程文件）
