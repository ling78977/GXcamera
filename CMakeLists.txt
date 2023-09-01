# 指定最低的cmake版本
cmake_minimum_required(VERSION 3.1)

# 指定项目名称和语言
project(Gx CXX)

# 指定编译选项
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-O3 -Wall")
endif()

# 查找依赖库
find_package(OpenCV REQUIRED)

# 包含头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

add_library(gxcamera ${PROJECT_SOURCE_DIR}/src/Gxcamera.cpp)
target_link_libraries(gxcamera ${OpenCV_LIBS} ${PROJECT_SOURCE_DIR}/include/sdk/lib/libgxiapi.so.1.0.1904.8241 /${PROJECT_SOURCE_DIR}/include/sdk/lib/libdximageproc.so.1.0.1904.8121)
# # 指定可执行文件和链接库
add_executable(test ${PROJECT_SOURCE_DIR}/test/test_gxcamera.cpp)
target_link_libraries(test  gxcamera)
