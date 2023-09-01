/***
 * @Author: ling78977 1986830413@qq.com
 * @Date: 2023-08-31 17:22:55
 * @LastEditors: ling78977 1986830413@qq.com
 * @LastEditTime: 2023-09-01 10:00:45
 * @FilePath: /Gxcamera/test/test_gxcamera.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */

#include "include/Gxcamera.h"

int main() {
  cv::Mat img_;
  Gxcamera::GxVideoCapture* cap = new Gxcamera::GxVideoCapture(
      Gxcamera::CameraParam(Gxcamera::CAMERAMODE::USE_GXCAMERA,
                            Gxcamera::RESOLUTION::RESOLUTION_640_X_480,
                            Gxcamera::EXPOSURETIME::EXPOSURE_5000));
  while (true) {
    if (cap->isindustryimgInput()) {
      img_ = cap->getImage();
    }
    if (!img_.empty()) {
      cv::imshow("aaa", img_);
      cv::waitKey(50);
    } else {
      std::cout << "空" << std::endl;
    }
  }

  return 0;
}
