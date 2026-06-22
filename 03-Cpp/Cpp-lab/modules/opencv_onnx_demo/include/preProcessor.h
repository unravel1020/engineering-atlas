#pragma once

#include "modelInfo.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <string>
#include <vector>

class PreProcessor {
public:
  std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                   const PreprocessConfig &cfg, int &c, int &h,
                                   int &w, PreprocessInfo *info_out = nullptr);

private:
  void validateConfig(const PreprocessConfig &cfg) const;

  cv::Mat letterbox(const cv::Mat &img, int target_h, int target_w,
                    const std::vector<uint8_t> &pad_color, float &scale,
                    int &pad_top, int &pad_bottom, int &pad_left,
                    int &pad_right) const;
};

namespace preProcessor {

// Default global preprocessor instance for backward compatibility.
std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                 const PreprocessConfig &cfg, int &c, int &h,
                                 int &w, PreprocessInfo *info_out = nullptr);

} // namespace preProcessor
