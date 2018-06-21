# hplayer

# 多画面播放器

## 需求分析

- 做一个类似VLC的播放器，能播放本地媒体文件、捕获设备、网络流；
- 界面要求多画面监控网格，可自由切换多画面风格；

## 概要设计

- 使用Qt实现界面；
- 使用OpenCV的VideoCapture获取视频帧；
- 使用OpenGL渲染视频帧；

## 详细设计

### 界面设计截图

![hplayer](https://img-blog.csdn.net/20180621172321266?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0dHX1NpTWlEYQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

![OpenMedia](https://img-blog.csdn.net/2018062117293180?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0dHX1NpTWlEYQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

## 后期计划

- 添加人脸检测与识别功能；
- 添加美颜功能；
- 添加叠加图片（表情）与文字功能；
- 添加多画面合成功能；
- 添加推流功能；
- 添加桌面录制功能；
