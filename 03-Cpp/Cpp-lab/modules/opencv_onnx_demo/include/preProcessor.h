#pragma once

#include "modelInfo.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// PreProcessor converts an OpenCV image into the flat float tensor expected by
// ONNX Runtime. All transform parameters come from PreprocessConfig so that
// preprocessing is fully metadata-driven.
class PreProcessor {
public:
  std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                   const PreprocessConfig &cfg, int &c, int &h,
                                   int &w, PreprocessInfo *info_out = nullptr);

private:
  void validateConfig(const PreprocessConfig &cfg) const;

  // Letterbox resizes the image to fit inside target_h x target_w while
  // preserving the aspect ratio, then pads the remaining area with pad_color.
  // This is the standard preprocessing for object detection models such as
  // YOLO, because stretching would distort aspect-ratio-sensitive anchors.
  cv::Mat letterbox(const cv::Mat &img, int target_h, int target_w,
                    const std::vector<uint8_t> &pad_color, float &scale,
                    int &pad_top, int &pad_bottom, int &pad_left,
                    int &pad_right) const;
};

namespace preProcessor {

// Default global preprocessor instance retained for backward compatibility
// with existing call sites that do not need a custom PreProcessor object.
std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                 const PreprocessConfig &cfg, int &c, int &h,
                                 int &w, PreprocessInfo *info_out = nullptr);

} // namespace preProcessor
