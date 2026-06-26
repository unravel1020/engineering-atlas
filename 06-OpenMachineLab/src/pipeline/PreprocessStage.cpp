#include "pipeline/PreprocessStage.h"
#include <algorithm>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <vector>

namespace oml {
namespace pipeline {

PreprocessStage::PreprocessStage(const core::ModelConfig &model_config)
    : model_config_(model_config) {}

FrameDataPtr PreprocessStage::process(FrameDataPtr input) {
  if (!input || input->original_image.empty()) {
    return nullptr;
  }

  if (model_config_.inputs.empty()) {
    return nullptr;
  }

  const auto &tensor_cfg = model_config_.inputs[0];
  const auto &cfg = tensor_cfg.preprocess;

  if (cfg.layout != "NCHW") {
    throw std::runtime_error("Unsupported preprocess layout: " + cfg.layout);
  }

  cv::Mat converted = input->original_image;
  if (cfg.color_format == "RGB") {
    cv::cvtColor(input->original_image, converted, cv::COLOR_BGR2RGB);
  } else if (cfg.color_format != "BGR") {
    throw std::runtime_error("Unsupported color_format: " + cfg.color_format);
  }

  cv::Mat resized;
  if (cfg.resize_mode == "letterbox") {
    float r = std::min(static_cast<float>(cfg.target_w) / converted.cols,
                       static_cast<float>(cfg.target_h) / converted.rows);
    int new_w = static_cast<int>(std::round(converted.cols * r));
    int new_h = static_cast<int>(std::round(converted.rows * r));
    cv::Mat tmp;
    cv::resize(converted, tmp, cv::Size(new_w, new_h), 0, 0, cv::INTER_LINEAR);

    int pad_top = (cfg.target_h - new_h) / 2;
    int pad_bottom = cfg.target_h - new_h - pad_top;
    int pad_left = (cfg.target_w - new_w) / 2;
    int pad_right = cfg.target_w - new_w - pad_left;

    cv::Scalar color(cfg.pad_color[0], cfg.pad_color[1], cfg.pad_color[2]);
    cv::copyMakeBorder(tmp, resized, pad_top, pad_bottom, pad_left, pad_right,
                       cv::BORDER_CONSTANT, color);
  } else if (cfg.resize_mode == "stretch") {
    cv::resize(converted, resized, cv::Size(cfg.target_w, cfg.target_h));
  } else {
    throw std::runtime_error("Unsupported resize_mode: " + cfg.resize_mode);
  }

  resized.convertTo(resized, CV_32F, cfg.scale);

  if (!cfg.mean.empty() && !cfg.std.empty()) {
    std::vector<cv::Mat> channels;
    cv::split(resized, channels);
    for (size_t i = 0; i < channels.size(); ++i) {
      float mean = i < cfg.mean.size() ? cfg.mean[i] : cfg.mean.back();
      float std = i < cfg.std.size() ? cfg.std[i] : cfg.std.back();
      channels[i] = (channels[i] - mean) / std;
    }
    cv::merge(channels, resized);
  }

  std::vector<cv::Mat> channels;
  cv::split(resized, channels);
  std::vector<float> tensor;
  for (auto &ch : channels) {
    tensor.insert(tensor.end(), (float *)ch.datastart, (float *)ch.dataend);
  }

  input->inputs[tensor_cfg.name] = std::move(tensor);
  return input;
}

} // namespace pipeline
} // namespace oml
