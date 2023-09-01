/***
 * @Author: ling78977 1986830413@qq.com
 * @Date: 2023-08-31 11:20:00
 * @LastEditors: ling78977 1986830413@qq.com
 * @LastEditTime: 2023-08-31 11:25:23
 * @FilePath: /Gxcamera/include/Gxcamera.h
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#ifndef GXCAMERA_INCLUDE_GXCAMERA_H_
#define GXCAMERA_INCLUDE_GXCAMERA_H_
#include <opencv2/imgproc/imgproc_c.h>

#include <iostream>
#include <opencv2/opencv.hpp>

#include "include/sdk/DxImageProc.h"
#include "include/sdk/GxIAPI.h"

namespace Gxcamera {
// typedef unsigned char BYTE;
typedef enum CAMERAMODE {
  USE_GXCAMERA = 1,
  NOT_USE_CAMERA = 0,
} CAMERAMODE;

typedef enum EXPOSURETIME {
  // 相机曝光时间
  EXPOSURE_5000 = 5000,
  EXPOSURE_2500 = 2500,
  EXPOSURE_1200 = 1200,
  EXPOSURE_800 = 800,
  EXPOSURE_600 = 600,
  EXPOSURE_400 = 400,
} EXPOSURETIME;

typedef enum RESOLUTION {
  // 相机分辨率
  RESOLUTION_656_X_492,
  RESOLUTION_640_X_480,
  RESOLUTION_480_X_360,
  RESOLUTION_320_X_240,
  RESOLUTION_160_X_120,
} RESOLUTION;

typedef struct Camera_Resolution {
  int cols;
  int rows;
  // 设置相机分辨率
  explicit Camera_Resolution(const RESOLUTION _resolution) {
    switch (_resolution) {
      case RESOLUTION::RESOLUTION_656_X_492:
        cols = 656;
        rows = 492;
        break;
      case RESOLUTION::RESOLUTION_640_X_480:
        cols = 640;
        rows = 480;
        break;
      case RESOLUTION::RESOLUTION_480_X_360:
        cols = 480;
        rows = 360;
        break;
      case RESOLUTION::RESOLUTION_320_X_240:
        cols = 320;
        rows = 240;
        break;
      case RESOLUTION::RESOLUTION_160_X_120:
        cols = 160;
        rows = 120;
        break;
      default:
        cols = 640;
        rows = 480;
        break;
    }
  }
} Camera_Resolution;

typedef struct CameraParam {
  CAMERAMODE camera_mode;
  Camera_Resolution resolution;
  EXPOSURETIME camera_exposuretime;

  CameraParam(const CAMERAMODE _camera_mode, const RESOLUTION _resolution,
              const EXPOSURETIME _camera_exposuretime)
      : camera_mode(_camera_mode),
        resolution(_resolution),
        camera_exposuretime(_camera_exposuretime) {}
} CameraParam;

class GxVideoCapture {
 public:
  GxVideoCapture() = default;

  explicit GxVideoCapture(const CameraParam& _camera_param);

  /**
   * @brief 判断工业相机是否在线
   *
   * @return true   检测到工业相机
   * @return false  没检测到工业相机
   */
  bool isindustryimgInput();

  /**
   * @brief 返回相机读取图片
   *
   * @return cv::Mat
   */
  inline cv::Mat getImage() {
    cv::cvtColor(cv::cvarrToMat(ipl_image_, true), image_, cv::COLOR_RGB2BGR);
    return image_;
  };

 private:
  // unsigned char* prgb_buffer_;
  cv::Mat image_;
  int camera_index_ = 1;
  int channel_ = 3;
  bool iscamera_open_ = false;
  uint32_t device_num_ = 0;
  int32_t image_size_;
  size_t frame_info_data_size_ = 0;
  int64_t pixel_fomat_ = GX_PIXEL_FORMAT_BAYER_RG8;  // 图像的数据格式
  int64_t color_filtor_ = GX_COLOR_FILTER_BAYER_RG;  // 图像的贝尔格式
  GX_STATUS status_ = GX_STATUS_ERROR;
  GX_DEV_HANDLE device_ = NULL;
  GX_FRAME_DATA frame_data_;  // 采集图像参数

  IplImage* ipl_image_ = nullptr;
  // 将非8位raw数据转换成8位数据时候的中转缓冲buffer
  void* praw8_buffer_ = NULL;
  // Raw数据转换成RGB数据后的储存空间，大小是相机输出数据的三倍
  void* prgb_frame_data_ = NULL;
  // 帧信息数据缓冲区
  // void* pframe_info_data_ = NULL;

  ~GxVideoCapture();

  /**
   * @brief 清空相机内存
   *
   */
  void cameraReleasebuff();

  /**
   * @brief 设置相机参数
   *
   * @param _CAMERA_RESOLUTION_COLS  设置相机宽度
   * @param _CAMERA_RESOLUTION_ROWS  设置相机高度
   * @param _CAMERA_EXPOSURETIME     设置相机曝光
   * @return int
   */
  int cameraInit(const int _CAMERA_RESOLUTION_COLS,
                 const int _CAMERA_RESOLUTION_ROWS,
                 const int _CAMERA_EXPOSURETIME);

  /**
   * @brief 将相机输出的原始数据转换为RGB数据
   *
   * @param _pimage_buffer [in] 指向图像缓冲区的指针
   * @param _pimage_raw8_buffer [in] 中转缓冲buffer
   * @param _pimage_rgb_buffer [in,out] 指向RGB数据缓冲区的指针
   * @param _image_width [in] 图像宽
   * @param _image_height [in] 图像高
   * @param _pixel_format [in] 图像的格式
   * @param _pixel_color_filter [in] Raw数据的像素排列格式
   */
  void processData(void* _pimage_buffer, void* _pimage_raw8_buffer,
                   void* _pimage_rgb_buffer, int _image_width,
                   int _image_height, int _pixel_format,
                   int _pixel_color_filter);
};
}  // namespace Gxcamera
#endif