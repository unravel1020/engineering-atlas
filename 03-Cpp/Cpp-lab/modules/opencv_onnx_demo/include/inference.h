#pragma once

#include "modelInfo.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <string>

class Inference {
public:
  Inference(const std::string &model_path);

  std::vector<float> run(const cv::Mat &img);

  void printModelInfo() const;

private:
  ModelInfo buildModelInfo(const std::string &model_path);

private:
  Ort::Env env;

  Ort::Session session;

  Ort::SessionOptions options;

  ModelInfo model_info_;
};
