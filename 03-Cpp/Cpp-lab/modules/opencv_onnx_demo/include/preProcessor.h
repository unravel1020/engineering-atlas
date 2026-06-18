#pragma once
#include <opencv2/opencv.hpp>

namespace preProcessor {
std::vector<float> mat_to_tensor_nchw(const cv::Mat &img, int &c, int &h,
                                      int &w, int target_h, int target_w);
} // namespace preProcessor