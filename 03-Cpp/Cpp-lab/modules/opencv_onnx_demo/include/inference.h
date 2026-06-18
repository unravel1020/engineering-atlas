#pragma once

#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Inference {
public:
  Inference(const std::string &model_path);

  std::vector<float> run(const cv::Mat &img);

private:
  Ort::Env env;
  Ort::Session session;
  Ort::SessionOptions options;

  std::string input_name;
  std::string output_name;

  void init_io_names();
}; // namespace inference