/***
 * @Author: ling78977 1986830413@qq.com
 * @Date: 2023-08-31 11:23:32
 * @LastEditors: ling78977 1986830413@qq.com
 * @LastEditTime: 2023-08-31 14:33:53
 * @FilePath: ~/Gxcamera/src/Gxcamera.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include "include/Gxcamera.h"

namespace Gxcamera {
GxVideoCapture::GxVideoCapture(const CameraParam& _camera_param) {
  if (_camera_param.camera_mode == CAMERAMODE::USE_GXCAMERA) {
    cameraInit(_camera_param.resolution.cols, _camera_param.resolution.rows,
               _camera_param.camera_exposuretime);
    iscamera_open_ = true;
    std::cout << "Using GX industrial camera " << std::endl;
  } else {
    iscamera_open_ = false;
    std::cout << "\033[31m Not use GX industrial camera \033[0m" << std::endl;
  }
}
int GxVideoCapture::cameraInit(const int _CAMERA_RESOLUTION_COLS,
                               const int _CAMERA_RESOLUTION_ROWS,
                               const int _CAMERA_EXPOSURETIME) {
  // 初始化库
  status_ = GXInitLib();
  // 枚举设备
  status_ = GXUpdateDeviceList(&device_num_, 1000);
  if (status_ != GX_STATUS_SUCCESS || device_num_ <= 0) {
    std::cout << "\033[31m Error, no GX industrial camera detected \033[0m"
              << std::endl;
    return -1;
  }
  // 打开相机
  status_ = GXOpenDeviceByIndex(camera_index_, &device_);
  if (status_ != GX_STATUS_SUCCESS) {
    std::cout << "\033[31m Error, open GX industrial camera failed! \033[0m"
              << std::endl;
    return -1;
  }
  std::cout << "Info, Init GX industrial camera success" << std::endl;
  int64_t play_load_size = 0;
  status_ = GXGetInt(device_, GX_INT_PAYLOAD_SIZE, &play_load_size);
  frame_data_.pImgBuf = malloc(play_load_size);
  image_size_ = play_load_size;
  praw8_buffer_ = malloc(play_load_size);
  prgb_frame_data_ = malloc(play_load_size * 3);
  // 获取帧信息长度并申请帧信息数据空间
  status_ = GXGetBufferLength(device_, GX_BUFFER_FRAME_INFORMATION,
                              &frame_info_data_size_);
  // pframe_info_data_ = malloc(frame_info_data_size_);

  // 获取相机输出的数据格式和贝尔格式
  status_ = GXGetEnum(device_, GX_ENUM_PIXEL_FORMAT, &pixel_fomat_);
  status_ = GXGetEnum(device_, GX_ENUM_PIXEL_COLOR_FILTER, &color_filtor_);
  // 设置采集速度级别
  status_ = GXSetInt(device_, GX_INT_ACQUISITION_SPEED_LEVEL, 3);
  // 设置AOI
  status_ = GXSetInt(device_, GX_INT_OFFSET_X,
                     static_cast<int>((656 - _CAMERA_RESOLUTION_COLS) >> 1));
  status_ = GXSetInt(device_, GX_INT_OFFSET_Y,
                     static_cast<int>((492 - _CAMERA_RESOLUTION_ROWS) >> 1));
  status_ = GXSetInt(device_, GX_INT_WIDTH, _CAMERA_RESOLUTION_COLS);
  status_ = GXSetInt(device_, GX_INT_HEIGHT, _CAMERA_RESOLUTION_ROWS);
  // 设置曝光时间
  // 先关闭自动曝光
  status_ = GXSetEnum(device_, GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_OFF);
  status_ =
      GXSetFloat(device_, GX_FLOAT_EXPOSURE_TIME, double(_CAMERA_EXPOSURETIME));
  // 打开自动增益
  status_ = GXSetEnum(device_, GX_ENUM_GAIN_AUTO, GX_GAIN_AUTO_CONTINUOUS);

  // 打开自动白平衡
  status_ = GXSetEnum(device_, GX_ENUM_BALANCE_WHITE_AUTO,
                      GX_BALANCE_WHITE_AUTO_CONTINUOUS);
  // 开始采集，相机开始工作
  status_ = GXSendCommand(device_, GX_COMMAND_ACQUISITION_START);
  return 1;
}
void GxVideoCapture::cameraReleasebuff() {
  if (iscamera_open_) {
    free(praw8_buffer_);
    free(prgb_frame_data_);
    // free(pframe_info_data_);
    praw8_buffer_ = NULL;
    prgb_frame_data_ = NULL;
    // pframe_info_data_ = NULL;
  }
}
bool GxVideoCapture::isindustryimgInput() {
  bool isindustry_camera_open = false;
  if (iscamera_open_ == true) {
    if (GXGetImage(device_, &frame_data_, 100) == GX_STATUS_SUCCESS) {
      if (frame_data_.nStatus == GX_STATUS_SUCCESS) {
        processData(frame_data_.pImgBuf, praw8_buffer_, prgb_frame_data_,
                    frame_data_.nWidth, frame_data_.nHeight, pixel_fomat_,
                    color_filtor_);
        if (ipl_image_) {
          cvReleaseImageHeader(&ipl_image_);
        }
        ipl_image_ =
            cvCreateImageHeader(cvSize(frame_data_.nWidth, frame_data_.nHeight),
                                IPL_DEPTH_8U, channel_);
        cvSetData(ipl_image_, prgb_frame_data_, frame_data_.nWidth * channel_);
        cv::Mat image = cv::cvarrToMat(ipl_image_, true);
        // cv::imshow("ddddddd", image);
        isindustry_camera_open = true;
        // printf("采集成功: 宽：%d 高：%d\n", frame_data_.nWidth,
        //        frame_data_.nHeight);
      } else {
        isindustry_camera_open = false;
      }
    } else {
      isindustry_camera_open = false;
    }
  } else {
    isindustry_camera_open = false;
  }
  return isindustry_camera_open;
}
void GxVideoCapture::processData(void* _pimage_buffer,
                                 void* _pimage_raw8_buffer,
                                 void* _pimage_rgb_buffer, int _image_width,
                                 int _image_height, int _pixel_format,
                                 int _pixel_color_filter) {
  switch (_pixel_format) {
      // 当数据格式为12位时，位数转换为4-11
    case GX_PIXEL_FORMAT_BAYER_GR12:
    case GX_PIXEL_FORMAT_BAYER_RG12:
    case GX_PIXEL_FORMAT_BAYER_GB12:
    case GX_PIXEL_FORMAT_BAYER_BG12:
      // 将12位格式图像转换为8位格式
      DxRaw16toRaw8(_pimage_buffer, _pimage_raw8_buffer, _image_width,
                    _image_height, DX_BIT_4_11);
      DxRaw8toRGB24(_pimage_raw8_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, RAW2RGB_NEIGHBOUR, BAYERRG, false);
      break;
      // 当数据格式为10位时，位数转换为2-9
    case GX_PIXEL_FORMAT_BAYER_GR10:
    case GX_PIXEL_FORMAT_BAYER_RG10:
    case GX_PIXEL_FORMAT_BAYER_GB10:
    case GX_PIXEL_FORMAT_BAYER_BG10:
      ////将10位格式的图像转换为8位格式,有效位数2-9
      DxRaw16toRaw8(_pimage_buffer, _pimage_raw8_buffer, _image_width,
                    _image_height, DX_BIT_2_9);
      // 将Raw8图像转换为RGB图像以供显示
      DxRaw8toRGB24(_pimage_raw8_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, RAW2RGB_NEIGHBOUR, BAYERRG, false);
      break;

    case GX_PIXEL_FORMAT_BAYER_GR8:
    case GX_PIXEL_FORMAT_BAYER_RG8:
    case GX_PIXEL_FORMAT_BAYER_GB8:
    case GX_PIXEL_FORMAT_BAYER_BG8:
      // 将Raw8图像转换为RGB图像以供显示

      DxRaw8toRGB24(_pimage_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, RAW2RGB_NEIGHBOUR,
                    DX_PIXEL_COLOR_FILTER(_pixel_color_filter),
                    false);  // RAW2RGB_ADAPTIVE
      break;

    case GX_PIXEL_FORMAT_MONO12:
      // 将12位格式的图像转换为8位格式
      DxRaw16toRaw8(_pimage_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, DX_BIT_4_11);
      // 将Raw8图像转换为RGB图像以供显示
      DxRaw8toRGB24(_pimage_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, RAW2RGB_NEIGHBOUR,
                    DX_PIXEL_COLOR_FILTER(NONE), false);
      break;

    case GX_PIXEL_FORMAT_MONO10:
      // 将10位格式的图像转换为8位格式
      DxRaw16toRaw8(_pimage_buffer, _pimage_raw8_buffer, _image_width,
                    _image_height, DX_BIT_4_11);
      // 将Raw8图像转换为RGB图像以供显示
      DxRaw8toRGB24(_pimage_raw8_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, RAW2RGB_NEIGHBOUR,
                    DX_PIXEL_COLOR_FILTER(NONE), false);
      break;

    case GX_PIXEL_FORMAT_MONO8:
      // 将Raw8图像转换为RGB图像以供显示
      DxRaw8toRGB24(_pimage_buffer, _pimage_rgb_buffer, _image_width,
                    _image_height, RAW2RGB_NEIGHBOUR,
                    DX_PIXEL_COLOR_FILTER(NONE), false);
      break;

    default:
      break;
  }
}
GxVideoCapture::~GxVideoCapture() {
  if (iscamera_open_) {
    cameraReleasebuff();
    GXSendCommand(device_, GX_COMMAND_ACQUISITION_STOP);
    GXCloseDevice(device_);
    GXCloseLib();
  }
}

}  // namespace Gxcamera
