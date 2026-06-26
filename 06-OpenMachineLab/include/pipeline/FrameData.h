#pragma once

#include <cstdint>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace oml {
namespace pipeline {

// FrameData carries one frame through all pipeline stages.
// It is intentionally a mutable bag of data; stages read and write the fields
// relevant to them.
struct FrameData {
  uint64_t frame_id = 0;

  // Source image received from caller.
  cv::Mat original_image;

  // Preprocessed tensor data, keyed by input tensor name.
  std::unordered_map<std::string, std::vector<float>> inputs;

  // Raw inference outputs, keyed by output tensor name.
  std::unordered_map<std::string, std::vector<float>> raw_outputs;

  // Final task-specific result, serialized or rendered later.
  std::string result_json;

  // Metadata for observability.
  std::string source;
  uint64_t timestamp_ms = 0;
};

using FrameDataPtr = std::shared_ptr<FrameData>;

} // namespace pipeline
} // namespace oml
