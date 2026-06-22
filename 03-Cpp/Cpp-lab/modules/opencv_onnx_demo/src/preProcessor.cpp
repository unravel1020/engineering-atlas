#include "preProcessor.h"
#include <opencv2/opencv.hpp>

namespace preProcessor {

std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                 const PreprocessConfig &cfg, int &c, int &h,
                                 int &w) {
  (void)info;

  cv::Mat converted = img;
  if (cfg.color_format == "RGB") {
    cv::cvtColor(img, converted, cv::COLOR_BGR2RGB);
  }

  cv::Mat resized;
  if (cfg.resize_mode == "stretch") {
    cv::resize(converted, resized, cv::Size(cfg.target_w, cfg.target_h));
  } else {
    cv::resize(converted, resized, cv::Size(cfg.target_w, cfg.target_h));
  }

  resized.convertTo(resized, CV_32F, cfg.scale);

  if (!cfg.mean.empty() && !cfg.std.empty()) {
    std::vector<cv::Mat> normalized_channels;
    cv::split(resized, normalized_channels);

    for (size_t i = 0; i < normalized_channels.size(); ++i) {
      float mean = i < cfg.mean.size() ? cfg.mean[i] : cfg.mean.back();
      float std = i < cfg.std.size() ? cfg.std[i] : cfg.std.back();
      normalized_channels[i] = (normalized_channels[i] - mean) / std;
    }

    cv::merge(normalized_channels, resized);
  }

  h = resized.rows;
  w = resized.cols;
  c = resized.channels();

  std::vector<cv::Mat> channels;
  cv::split(resized, channels);

  std::vector<float> tensor;
  for (auto &ch : channels) {
    tensor.insert(tensor.end(), (float *)ch.datastart, (float *)ch.dataend);
  }

  return tensor;
}

} // namespace preProcessor
