#pragma once

#include "modelInfo.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class Inference {
public:
  explicit Inference(const ModelInfo &model_info);

  std::unordered_map<std::string, std::vector<float>>
  run(const std::unordered_map<std::string, cv::Mat> &input_images);

  std::vector<float> run(const cv::Mat &img);

  void printModelInfo() const;

  const ModelInfo &modelInfo() const;

private:
  void syncTensorShape();

  std::vector<int64_t> fixInputShape(const std::vector<int64_t> &shape, int c,
                                     int h, int w) const;

private:
  Ort::Env env_;

  Ort::Session session_;

  Ort::SessionOptions options_;

  ModelInfo model_info_;
};
