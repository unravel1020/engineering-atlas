#pragma once

#include "modelInfo.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace preProcessor {
std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                 const PreprocessConfig &cfg, int &c, int &h,
                                 int &w);
} // namespace preProcessor
