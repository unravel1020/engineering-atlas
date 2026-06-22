#include "preProcessor.h"
#include <opencv2/opencv.hpp>

namespace preProcessor {

namespace {

cv::Mat letterbox(const cv::Mat &img, int target_h, int target_w,
                  const std::vector<uint8_t> &pad_color, float &scale,
                  int &pad_top, int &pad_bottom, int &pad_left,
                  int &pad_right) {
  float r = std::min(static_cast<float>(target_w) / img.cols,
                     static_cast<float>(target_h) / img.rows);
  scale = r;

  int new_w = static_cast<int>(std::round(img.cols * r));
  int new_h = static_cast<int>(std::round(img.rows * r));

  cv::Mat resized;
  cv::resize(img, resized, cv::Size(new_w, new_h), 0, 0, cv::INTER_LINEAR);

  pad_top = (target_h - new_h) / 2;
  pad_bottom = target_h - new_h - pad_top;
  pad_left = (target_w - new_w) / 2;
  pad_right = target_w - new_w - pad_left;

  cv::Scalar color(pad_color[0], pad_color[1], pad_color[2]);
  cv::Mat padded;
  cv::copyMakeBorder(resized, padded, pad_top, pad_bottom, pad_left, pad_right,
                     cv::BORDER_CONSTANT, color);

  return padded;
}

} // namespace

std::vector<float> mat_to_tensor(const cv::Mat &img, const TensorInfo &info,
                                 const PreprocessConfig &cfg, int &c, int &h,
                                 int &w, PreprocessInfo *info_out) {
  (void)info;

  if (info_out != nullptr) {
    info_out->orig_h = img.rows;
    info_out->orig_w = img.cols;
    info_out->model_h = cfg.target_h;
    info_out->model_w = cfg.target_w;
    info_out->resize_mode = cfg.resize_mode;
  }

  cv::Mat converted = img;
  if (cfg.color_format == "RGB") {
    cv::cvtColor(img, converted, cv::COLOR_BGR2RGB);
  }

  cv::Mat resized;
  float scale = 1.0f;
  int pad_top = 0;
  int pad_bottom = 0;
  int pad_left = 0;
  int pad_right = 0;

  if (cfg.resize_mode == "letterbox") {
    resized = letterbox(converted, cfg.target_h, cfg.target_w, cfg.pad_color,
                        scale, pad_top, pad_bottom, pad_left, pad_right);
  } else {
    cv::resize(converted, resized, cv::Size(cfg.target_w, cfg.target_h));
  }

  if (info_out != nullptr) {
    info_out->scale = scale;
    info_out->pad_top = pad_top;
    info_out->pad_bottom = pad_bottom;
    info_out->pad_left = pad_left;
    info_out->pad_right = pad_right;
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
