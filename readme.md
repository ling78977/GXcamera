# GXCAMERA
大恒相机水星一代 USB2.0 相机驱动

## 简介

1. 硬件配置
ubuntu amd64

2. 功能
- 打开大恒相机采集模式转换图像格式并用opencv库函数显示图像

## 环境配置

- Ubuntu 20.04 LTS
- gcc 7.5.0
- OpenCV 4.5.1
- cmake 10.1
## 文件结构

```txt
.
├── CMakeLists.txt
├── include
│   ├── Gxcamera.h  //相机类头文件
│   └── sdk         //大恒相机的API头文件
│       ├── DxImageProc.h 
│       ├── GxIAPI.h
│       └── lib     //相机sdk的动态链接库
│           ├── libdximageproc.so.1.0.1904.8121
│           └── libgxiapi.so.1.0.1904.8241
├── readme.md
├── src   
│   └── Gxcamera.cpp
└── test
    └── test_gxcamera.cpp

```
  
## 如何使用？

1. 进入项目根目录
```
    mkdir build 
    cd build
    cmake ..
    make
    sudo ./test

```


## 说明

一个非常简单的相机类demo，使用时需要把该项目集成到自己的相机模块中使用类和对象的方式调用，参照test中的测试程序